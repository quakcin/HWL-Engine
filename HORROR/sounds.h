#pragma once
#include "root.h"

typedef struct sounds_s
{
	struct sounds_s * next;
	char name[0xF];
	ALLEGRO_SAMPLE * sounds[0xF];
	unsigned int count;
} sounds_t;

void play_sound(sounds_t * self, char * name);
void load_global_sound(sounds_t * self, char * name);
sounds_t * create_sounds();
void destroy_sounds(sounds_t * self);
void load_local_sound(sounds_t * self, engine_t * engine, char * name);
void play_specific_sound(sounds_t * self, char * name, unsigned int id);