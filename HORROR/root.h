#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#include "keyboard.h"
#include "daemon.h"

#define LOADER_PROCESS	 10
#define WORKER_PROCESS	 20
#define RENDER_PROCESS   30

typedef struct engine_s
{
	ALLEGRO_DISPLAY * display;
	ALLEGRO_TIMER * timer;

	keyboard_t * keyboard;

	unsigned int height;
	unsigned int width;

	unsigned int level;
	unsigned int lv_count;

	unsigned int brightness;

	int zopt;
	int vopt;

	bool beginner_hints;

} engine_t;

void root_init(daemon_t * dae, daemon_process_t * proc);
void root_destroy(daemon_t * dae, daemon_process_t * proc, engine_t * data);
void root_update(daemon_t * dae, daemon_process_t * proc, engine_t * data);
engine_t * get_engine(daemon_t * dae);