#pragma once
#include "root.h"

#include "overlay.h"
#include "listener.h"
#include "scheduler.h"
#include "data.h"
#include "sounds.h"


typedef struct menu_s
{
	scheduler_t * scheduler;
	overlay_t * overlay;
	listener_t * listener;
	sounds_t * sounds;
	data_t * data;
	int selected;
	int wrap;
	bool ghosting;
	bool listening;
	daemon_t * daemon;
} menu_t;


void menu_init(daemon_t * dae, daemon_process_t * proc);
void menu_update(daemon_t * dae, daemon_process_t * proc, menu_t * self);
void menu_destroy(daemon_t * dae, daemon_process_t * proc, menu_t * self);
