#include "worker.h"
#include "loader.h"
#include "util.h"
#include "scripts.h"
#include "hints.h"

// -- predefs

void h_begin(worker_t * self);
void h_chase(worker_t * self);

// -- coverage

void p_coverage(worker_t * self)
{
	int x, y;
	self->player->penelty = 0;

	const int cx = ((int)self->player->x) >> 7;
	const int cy = ((int)self->player->y) >> 7;

	for (y = -1; y <= 1; y += 1)
		for (x = -1; x <= 1; x += 1)
			if (x == 0 || y == 0)
				if (self->map->tiles[((cy + y) << 8) + (cx + x)] != 0)
					self->player->penelty += 18;

	// -- lightmaps reward
	self->player->penelty += self->map->clight * 5;

	if (GetVal(self->data, "f_br_held") == 1 && GetVal(self->data, "f_br_lock") == 1)								// -- reward for holding breath
		self->player->penelty += (self->player->penelty / 5) + 5;

	else if (GetVal(self->data, "f_br_lock") == 1)																		// -- punishment for failing
		self->player->penelty -= 3 * (self->player->penelty / 4);

	self->player->penelty -= 8 * GetVal(self->data, "f_stars");

	self->player->penelty = 100 - self->player->penelty;

	if (GetVal(self->data, "f_smudging") == 1)
		self->player->penelty += 30;
}

uint8_t light_masks[4] = { 3, 12, 48, 192 };

void p_lightmap(worker_t * self)
{
	int px = (int) self->player->x >> 7;
	int py = (int) self->player->y >> 7;
	int pos = (py >> 1) * 128 + (px >> 1);
	int off = (2 * (py & 1) + (px & 1));
	float light = (float)(self->map->light[pos] & light_masks[off] >> (off << 1)); // -- swapped
	self->map->clight = light;
	self->player->papprox = light * 9 + 4;

	if (GetVal(self->data, "f_br_held") == 1 && GetVal(self->data, "f_br_lock") == 1)
		self->player->papprox -= (self->player->papprox - 21 < 0) ? 0 : 21;

	schedule(self->schedule, p_lightmap, self, 30);
}

// -- mechanics

void h_damage(worker_t * self, float damage)
{
	// find a way to deal damage to the player
	play_sound(self->sounds, "pain");
	overlay_enable(self->overlay, "damage");
	self->player->health -= damage;
	printf("Delt Damage: %f, Player HP: %f\n", damage, self->player->health);
}

//
//	-- Completly Rewritten: h_catch
//

void h_catch(worker_t * self)
{
	float dist = p_approx_dist(
		self->player->x / 64, 
		self->player->y / 64,
		(float) GetVal(self->data, "h_pos_x") / 64, 
		(float) GetVal(self->data, "h_pos_y") / 64
	);

	// ~= 25u  should delt least damage!

	float segment_score = 24 * dist;
	float max_segment_dmg = (float) GetVal(self->data, "h_seg_mxdmg");
	float perfect_score = (float) GetVal(self->data, "h_chase_time");
	float damage = max_segment_dmg - (max_segment_dmg * segment_score / perfect_score);

	if ((damage > 0 && damage < GetVal(self->data, "c_dmg_min")) || damage > GetVal(self->data, "c_dmg_max"))
		damage = (float) GetVal(self->data, "c_dmg_min");

	if (damage > 0)
		h_damage(self, damage);

	DecrVal(self->data, "h_chase_count");

	if (GetVal(self->data, "h_chase_count") > 0)
		schedule(self->schedule, h_chase, self, 30);
	else
		h_begin(self);
}

void h_chase (worker_t * self)
{
	if (GetVal(self->data, "h_chase_begin") == 1)
	{
		overlay_enable(self->overlay, "angry");
		play_sound(self->sounds, "angry");
		SetVal(self->data, "h_chase_begin", 0);
	}
	else
	{
		overlay_enable(self->overlay, "howler");
		play_sound(self->sounds, "howler");
	}

	SetVal(self->data, "h_pos_x", (unsigned int) self->player->x);
	SetVal(self->data, "h_pos_y", (unsigned int) self->player->y);

	// SetVal(self->data, "h_chase_time", p_rand(2, 5, 60));
	SetVal(self->data, "h_chase_time", p_rand(
		GetVal(self->data, "c_cht_min"), GetVal(self->data, "c_cht_max"), 60
	));

	schedule(self->schedule, h_catch, self, GetVal(self->data, "h_chase_time"));

	// -- play chase sounds
	play_specific_sound(self->sounds, "chase", (GetVal(self->data, "h_chase_time") / 60) - 2);	// -- FIX ME: Sounds Might not be properly cut
	self->player->panic = true;
	m_rise_hint(self, "m_run");
}

void h_chase_begin (worker_t * self)
{
	unsigned int amplifier = 0;												// -- over time game gets harder
	if (GetVal(self->data, "f_stars") >= 2)
		amplifier = GetVal(self->data, "f_stars") + 1;

	SetVal(self->data, "h_chase_count", p_rand(
		GetVal(self->data, "c_cnt_min"), GetVal(self->data, "c_cnt_max") + amplifier, 1
	));

	SetVal(self->data, "h_seg_mxdmg", GetVal(self->data, "c_seg_dmg") / GetVal(self->data, "h_chase_count"));
	SetVal(self->data, "h_chase_begin", 1);

	schedule(self->schedule, h_chase, self, p_rand(
		GetVal(self->data, "c_chs_min"), GetVal(self->data, "c_chs_max"), 60
	));

}

void h_check (worker_t * self)
{
	p_coverage(self);	// -- calculate player coverage
	int chance = p_rand(0, 100, 1);
	// printf("Attack: %d vs %d\n", chance, self->player->penelty);
	if (chance < self->player->penelty)
		h_chase_begin(self);
	else
		h_begin(self);
}

void h_alert (worker_t * self)
{
	// -- 4% chance of not playing alert sound

	m_rise_hint(self, "m_hide");

	if (p_rand(0, 100, 1) > 4)
	{
		play_sound(self->sounds, "howler");
		overlay_enable(self->overlay, "howler");
	}

	schedule(self->schedule, h_check, self, p_rand(
		GetVal(self->data, "c_alt_min"), GetVal(self->data, "c_alt_max"), 60
	));
}

void h_begin(worker_t * self)
{
	self->player->panic = false;
	schedule(self->schedule, h_alert, self, p_rand(
		GetVal(self->data, "c_wit_min"), GetVal(self->data, "c_wit_max"), 60
	));
}

void p_breathing(worker_t * self)
{
	if (GetVal(self->data, "f_br_held") == 1)
		p_breath_out(self);
}

void p_walking_animation (worker_t * self)
{
	if (self->player->walk)
	{
		const float delta = (self->player->speed > 5) ? 25 : 5;
		static int i = 0;
		i += 1;
		if (i & 1)
			self->player->happrox = delta + (delta / 2);
		else
			self->player->happrox = -1 * (delta / 2);
		self->player->walk = false;
		play_sound(self->sounds, "step");
	}
	schedule(self->schedule, p_walking_animation, self, 30);
}

void p_custom_events(worker_t * self)
{
	if (self->player->door_event != 0)
	{
		play_specific_sound(self->sounds, "door", self->map->textures_count - self->player->door_event);
		overlay_enable(self->overlay, "door");
		self->player->door_event = 0;
	}

	if (self->player->health < 0 && self->player->alive)
		p_kill_player(self);

}

void worker_init (daemon_t * dae, daemon_process_t * proc)
{
	worker_t * self = dae->process->shared;

	// -- load / manage hints

	m_hints_init(self);

	// -- load overlays

	overlay_load(self->overlay, "howler", 0.25, PLAY_ONCE);
	overlay_load(self->overlay, "angry", 0.5, PLAY_ONCE);
	overlay_load(self->overlay, "damage", 0.75, PLAY_ONCE);

	// -- death messesages
	overlay_load(self->overlay, "bloom_red", 0.25, PLAY_ONCE);
	overlay_load(self->overlay, "d_msg1", 0.08, PLAY_ONCE);

	overlay_load(self->overlay, "bloom_white", 0.25, PLAY_ONCE);
	overlay_load(self->overlay, "v_msg1", 0.08, PLAY_ONCE);

	overlay_load(self->overlay, "lt_up", 0.25, PLAY_ONCE);
	overlay_load(self->overlay, "lt_idle", 0.1, PLAY_LOOP);
	overlay_load(self->overlay, "lt_try", 0.45, PLAY_ONCE);
	overlay_load(self->overlay, "lt_success", 0.25, PLAY_ONCE);
	overlay_load(self->overlay, "lt_hide", 0.25, PLAY_ONCE);

	overlay_load(self->overlay, "sm_hide", 0.3, PLAY_ONCE);
	overlay_load(self->overlay, "sm_rise", 0.25, PLAY_ONCE);
	overlay_load(self->overlay, "sm_idle", 1, PLAY_LOOP);
	overlay_load(self->overlay, "sm_burning", 0.25, PLAY_LOOP);

	overlay_load(self->overlay, "door", 0.2, PLAY_ONCE);

	// -- stars
	overlay_load(self->overlay, "stars", 0, PLAY_LOOP);

	overlay_enable(self->overlay, "sm_idle");
	overlay_enable(self->overlay, "stars");

	// -- load sounds
	load_global_sound(self->sounds, "smudge");
	load_global_sound(self->sounds, "angry");
	load_global_sound(self->sounds, "howler");
	load_global_sound(self->sounds, "lighter");
	load_global_sound(self->sounds, "lightup");
	load_global_sound(self->sounds, "burnout");
	load_global_sound(self->sounds, "chase");
	load_global_sound(self->sounds, "pain");
	load_global_sound(self->sounds, "br_short");
	load_global_sound(self->sounds, "br_long");
	load_global_sound(self->sounds, "dead");
	load_global_sound(self->sounds, "calm");
	load_global_sound(self->sounds, "ambience");

	load_local_sound(self->sounds, self->engine, "step");
	load_local_sound(self->sounds, self->engine, "door");

	// -- add smudged stick events

	listen(self->listener, ALLEGRO_KEY_SPACE, p_lighter_use, self);
	listen(self->listener, ALLEGRO_KEY_X, p_toggle_breath, self);
	
	// -- begin the game:

	schedule(self->schedule, h_alert, self, GetVal(self->data, "c_hwl_apr"));

	schedule(self->schedule, p_walking_animation, self, 30);
	schedule(self->schedule, p_breathing, self, 60);

	a_ambient_engine(self);
	smudge_scheduled_update(self);
	p_lightmap(self);
	SetVal(self->data, "w_purge", 0);
}


void worker_update (daemon_t * dae, daemon_process_t * proc, worker_t * self)
{
	// -- update scheduler and listener
	listener_update(self->listener);
	scheduler_update(self->schedule);
	overlay_update(self->overlay);
	player_update(self->player);
	p_custom_events(self);

	if (self->ticks & 127)
	{
		// -- do i drestroy scheduler now?
		if (GetVal(self->data, "w_purge") > 0)
		{
			destroy_scheduler(self->schedule);
			self->schedule = create_scheduler();

			if (self->player->alive)
				schedule(self->schedule, p_victory_player_messesage, self, 4 * 22);
			else
				schedule(self->schedule, p_kill_player_messesage, self, 4 * 24);

			SetVal(self->data, "w_purge", 0);
		}
	}

	self->ticks++;
}

void worker_destroy (daemon_t * dae, daemon_process_t * proc, worker_t * self)
{
	daemon_process_halt(dae, "render");
	destroy_worker(self);
}