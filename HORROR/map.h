#pragma once

#include "root.h"
#include <stdio.h>

#define TILES_SIZE 0xFFFF

typedef struct map_s
{
	unsigned int textures_count;
	uint8_t smudged[0xFFF];
	ALLEGRO_BITMAP * textures[0xFF + 1];
	ALLEGRO_COLOR floor;
	ALLEGRO_COLOR sky;
	uint8_t * tiles;
	uint8_t * light;
	unsigned int stars;
	unsigned int door;
	unsigned int props;
	unsigned int colliders;
	float clight;

} map_t;

#include "data.h"

void destroy_map(map_t * map);
map_t * create_map();
void map_load(engine_t * engine, map_t * map, data_t * data);