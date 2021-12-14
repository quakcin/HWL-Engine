#pragma once

#include "root.h"
#include "daemon.h"

#include "player.h"
#include "map.h"

#include "listener.h"
#include "scheduler.h"

#include "overlay.h"
#include "sounds.h"

#include "data.h"

typedef struct worker_s
{
	scheduler_t * schedule;
	listener_t * listener;
	overlay_t * overlay;
	engine_t * engine;
	player_t * player;
	sounds_t * sounds;
	data_t * data;
	map_t * map;

	daemon_t * daemon;
	unsigned int ticks;
} worker_t;

void loader_init(daemon_t * dae, daemon_process_t * proc);
void loader_destroy(daemon_t * dae, daemon_process_t * proc, worker_t * self);
void destroy_worker(worker_t * worker);
