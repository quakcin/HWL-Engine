#pragma once

#include "daemon.h"
#include "loader.h"

typedef struct summary_s
{
	worker_t * worker;
	ALLEGRO_BITMAP * overlay;
	ALLEGRO_FONT * font;
	char msg[0xFFF];
	bool dead;
} summary_t;

void summary_init(daemon_t * dae, daemon_process_t * proc);
void summary_update(daemon_t * dae, daemon_process_t * proc, summary_t * self);
void summary_destroy(daemon_t * dae, daemon_process_t * proc, summary_t * self);
