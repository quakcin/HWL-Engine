#include "scripts.h"
#include "util.h"
#include "main.h"

//
// -- Victory And Death Handling
//

void p_switch_to_summary(worker_t * self)
{

#ifndef _DIRECTOR_CUT_
	overlay_disable(self->overlay, "howler");
	overlay_disable(self->overlay, "stars");
	daemon_process_suspend(self->daemon, "worker");
	daemon_share_memory(self->daemon, "summary", "worker");
	daemon_process_begin(self->daemon, "summary");
#else
	daemon_process_halt(self->daemon, "worker");
	daemon_process_begin(self->daemon, "menu");
#endif
}

void p_kill_player_messesage(worker_t * self)
{
	play_sound(self->sounds, "dead");
	overlay_enable(self->overlay, "d_msg1");
	schedule(self->schedule, p_switch_to_summary, self, 400);
}

void p_kill_player(worker_t * self)
{
	// scheduler_purge(self->schedule);
	self->player->alive = false;
	SetVal(self->data, "w_purge", 1);

	overlay_enable(self->overlay, "bloom_red"); 
	schedule(self->schedule, p_kill_player_messesage, self, 24 * 4);
	printf("Player has been killed!\n");
}

void p_victory_player_messesage (worker_t * self)
{
	overlay_enable(self->overlay, "v_msg1");
	schedule(self->schedule, p_switch_to_summary, self, 400);
#ifdef _DIRECTOR_CUT_
	self->engine->level++;
#endif
}


void p_victory_player (worker_t * self)
{
	SetVal(self->data, "w_purge", 1);
	overlay_enable(self->overlay, "bloom_white");
	printf("Player has won!\n");

	// -- create victory file in data/progress/m$map
	char victory_path[0xFF];
	char victory_code[32];

	sprintf(victory_path, "data/progress/m%u", self->engine->level);
	sprintf(victory_code, "%u", (unsigned int) (self->ticks / 60));
	FILE * file = fopen(victory_path, "w+");
	fwrite(victory_code, sizeof(char), strlen(victory_code), file);
	fclose(file);
}

//
// -- Breath Holding Mechanism
//

void p_breath_unlock(worker_t * self)
{
	//self->howler->br_lock = false;
	SetVal(self->data, "f_br_lock", 0);
}

void p_breath_out (worker_t * self)
{
	schedule(self->schedule, p_breath_unlock, self, 8 * 60);
	play_sound(self->sounds, "br_long");
	// self->howler->br_held = false;
	SetVal(self->data, "f_br_held", 0);
}

void p_toggle_breath (worker_t * self)
{
	if (GetVal(self->data, "f_br_held") == 1 || GetVal(self->data, "f_br_lock") == 1)
		return;

	schedule(self->schedule, p_breath_out, self, p_rand(3, 6, 60));
	play_sound(self->sounds, "br_short");
	// self->howler->br_held = true;
	SetVal(self->data, "f_br_held", 1);
	// self->howler->br_lock = true;
	SetVal(self->data, "f_br_lock", 1);
}

//
// -- Smudge Mechanism
//

void p_smudge_burn_out (worker_t * self)
{
	if (GetVal(self->data, "f_smudging") == 0)
		return;

	overlay_disable(self->overlay, "sm_burning");
	overlay_enable(self->overlay, "sm_idle");
	play_sound(self->sounds, "burnout");

	SetVal(self->data, "f_smudging", 0);
	SetVal(self->data, "f_sm_ready", 0);
}

void p_lighter_punishment(worker_t * self)
{
	// actuall disable lighter punishment!
	SetVal(self->data, "f_lt_succ", 30);
}

void p_smudge_burning (worker_t * self)
{
	SetVal(self->data, "f_sm_ready", 1);
	overlay_enable(self->overlay, "sm_burning");
	unsigned int smudging_time = p_rand(6, 26, 60);
	schedule(self->schedule, p_smudge_burn_out, self, smudging_time);

	SetVal(self->data, "f_lt_succ", 0);
	schedule(self->schedule, p_lighter_punishment, self, smudging_time);
}

void p_smudge_light_up (worker_t * self)
{
	overlay_enable(self->overlay, "sm_rise");
	play_sound(self->sounds, "smudge");
	schedule(self->schedule, p_smudge_burning, self, 117);
}

void p_smudge_hide_burnt (worker_t * self)
{
	overlay_disable(self->overlay, "sm_idle");
	overlay_enable(self->overlay, "sm_hide");
	schedule(self->schedule, p_smudge_light_up, self, 100);
}

//
// -- Lighter Mechanism
//

void p_lighter_success (worker_t * self)
{
	SetVal(self->data, "f_lt_ready", 0);
	SetVal(self->data, "f_lt_in_use", 1);
	SetVal(self->data, "f_smudging", 1);
	overlay_disable(self->overlay, "lt_idle");

	overlay_enable(self->overlay, "lt_success");
	play_sound(self->sounds, "lightup");

	schedule(self->schedule, p_smudge_hide_burnt, self, 100);
}

void p_lighter_hide(worker_t * self)
{
	overlay_disable(self->overlay, "lt_idle");
	overlay_enable(self->overlay, "lt_hide");
	SetVal(self->data, "f_lt_ready", 0);
}

void p_lighter_anti_idle (worker_t * self)
{
	if (GetVal(self->data, "f_smudging") == 1)
		return;

	if (GetVal(self->data, "f_lt_in_use") == 0)
		p_lighter_hide(self);
	else
		schedule(self->schedule, p_lighter_anti_idle, self, 300);

	SetVal(self->data, "f_lt_in_use", 0);
}

void p_lighter_ready (worker_t * self)
{
	SetVal(self->data, "f_lt_ready", 1);
	overlay_enable(self->overlay, "lt_idle");
	schedule(self->schedule, p_lighter_anti_idle, self, 300);
}

void p_lighter_unlock(worker_t * self)
{
	// self->howler->lt_lock = false;
	SetVal(self->data, "f_lt_lock", 0);
}

void p_lighter_sparks_disable(worker_t * self)
{
	overlay_disable(self->overlay, "lt_try");
	overlay_enable(self->overlay, "lt_idle");
}

void p_lighter_use (worker_t * self)
{
	if (GetVal(self->data, "f_lt_lock") == 0)
	{
		if (GetVal(self->data, "f_smudging") == 0) // -- lighter
		{
			if (GetVal(self->data, "f_lt_ready") == 1)
			{
				SetVal(self->data, "f_lt_in_use", 1);
				play_sound(self->sounds, "lighter");
				if ((unsigned int) p_rand(0, 100, 1) < GetVal(self->data, "f_lt_succ"))
					p_lighter_success(self);
				else
				{
					overlay_disable(self->overlay, "lt_idle");
					overlay_enable(self->overlay, "lt_try");
					schedule(self->schedule, p_lighter_sparks_disable, self, 26);
				}
			}
			else
			{
				// -- animate and schedule "p_smudge_ready"
				overlay_enable(self->overlay, "lt_up");
				schedule(self->schedule, p_lighter_ready, self, 71);
			}
		}
		else
		{
			// -- turn off smudgestick
			if (GetVal(self->data, "f_sm_ready") == 1)
			{
				p_smudge_burn_out(self);
			}
		}
		SetVal(self->data, "f_lt_lock", 1);
		schedule(self->schedule, p_lighter_unlock, self, 75);
	}
}

void smudge_scheduled_update (worker_t * self)
{
	if (GetVal(self->data, "f_smudging") == 1)
	{
		int px = ((int)self->player->x) >> 9;
		int py = ((int)self->player->y) >> 9;
		int pos = (py << 6) + px;

		if (self->map->smudged[pos] == 0)
		{
			self->map->smudged[pos] = 1;
			IncrVal(self->data, "f_total");

			unsigned int stars = 5 * GetVal(self->data, "f_total") / self->map->stars;

			if (stars != GetVal(self->data, "f_stars"))
				overlay_advance(self->overlay, "stars");

			SetVal(self->data, "f_stars", stars);
		}


		if (GetVal(self->data, "f_total") >= self->map->stars)
		{
			play_sound(self->sounds, "calm");
			p_victory_player(self);
		}

	}
	schedule(self->schedule, smudge_scheduled_update, self, 120);
}

// -- Ambient Engine

void a_ambient_engine (worker_t * self)
{
	play_sound(self->sounds, "ambience");
	schedule(self->schedule, a_ambient_engine, self, p_rand(45, 60, 60));
}