#include "hints.h"
#include "main.h"

void m_disable_hints(worker_t * self)
{
	overlay_disable(self->overlay, "m_run");
	overlay_disable(self->overlay, "m_hide");
}

void m_rise_hint(worker_t * self, char * hint)
{
	if (GetVal(self->data, hint) == 0)
		return;

	overlay_enable(self->overlay, hint);
	SetVal(self->data, hint, 0);
	schedule(self->schedule, m_disable_hints, self, 3 * 60);
}

void m_play_next_hint (worker_t * self)
{
	if (GetVal(self->data, "m_hints") % 2 == 0)
	{
		overlay_enable(self->overlay, "hints");

		IncrVal(self->data, "m_hints");

		play_sound(self->sounds, "typewriter");

		schedule(self->schedule, m_play_next_hint, self, (
			GetVal(self->data, "m_hints") == 1 ? 4 * 60 : 2 * 60			
		));
	}
	else
	{
		overlay_disable(self->overlay, "hints");
		overlay_advance(self->overlay, "hints");

		IncrVal(self->data, "m_hints");

		if (GetVal(self->data, "m_hints") < 10)
			schedule(self->schedule, m_play_next_hint, self, 1 * 60);
		else
			self->engine->beginner_hints = false;
	}
}

void m_hints_init(worker_t * self)
{
	// -- always set these flags

	SetVal(self->data, "m_run", 0);
	SetVal(self->data, "m_hide", 0);

	// -- rest of hints engine

	if (self->engine->beginner_hints == false)
		return;

	load_global_sound(self->sounds, "typewriter");

	overlay_load(self->overlay, "hints", 0, PLAY_BACK);
	SetVal(self->data, "m_hints", 0);

	overlay_supply(self->overlay, "m_run", "data/overlay/hints/run.png");
	overlay_supply(self->overlay, "m_hide", "data/overlay/hints/hide.png");

	SetVal(self->data, "m_run", 1);
	SetVal(self->data, "m_hide", 1);

	schedule(self->schedule, m_play_next_hint, self, 3 * 60);
}