#pragma once
#include "root.h"

#define PLAY_ONCE 0
#define PLAY_LOOP 1
#define PLAY_BACK 2

typedef struct overlay_s
{
	struct overlay_s * next;
	ALLEGRO_BITMAP * frames[32];
	char name[0xF];
	unsigned int count;
	float frame;
	float speed;
	char enabled;
	char type;
} overlay_t;


void destroy_overlay(overlay_t * self);
void overlay_load(overlay_t * self, char * name, float speed, char type);
overlay_t * create_overlay();
void overlay_update(overlay_t * self);

void overlay_enable(overlay_t * self, char * name);
void overlay_disable(overlay_t * self, char * name);
void overlay_advance(overlay_t * self, char * name);

void overlay_display
(
	overlay_t * self,
	unsigned int switdh,
	unsigned int sheight,
	unsigned int width,
	unsigned int height
);

void overlay_supply
(
	overlay_t * self,
	char * name,
	char * path
);

/*

typedef struct overlay_s
{
	ALLEGRO_BITMAP * overlays[0xFF];
	unsigned int set; // unset: 256
	unsigned int beg;
	unsigned int end;
} overlay_t;

overlay_t * create_overlay();
void destroy_overlay(overlay_t * self);
void overlay_load(overlay_t * self, unsigned int index, char * name);
void overlay_set(overlay_t * self, unsigned int index);
void overlay_animate(overlay_t * self, unsigned int begin, unsigned int end);
void overlay_unset(overlay_t * self);
void overlay_load_aniamtion(overlay_t * self, unsigned int index, unsigned int frame_count, char * name);
*/