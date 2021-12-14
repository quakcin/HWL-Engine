#pragma once

#include "map.h"

#define DEG 0.0174527

typedef struct player_s
{
	float perception;
	float height;
	float happrox;
	float papprox;
	float health;
	float speed;
	float ang;
	float x;
	float y;
	int energy;
	int penelty;
	bool walk;
	bool panic;
	bool alive;
	uint8_t door_event;
} player_t;


player_t * create_player();
void destroy_player(player_t * player);
void p_move_forward(player_t * player, map_t * map);
void p_move_backwards(player_t * player, map_t * map);
void p_look_left(player_t * player);
void p_look_right(player_t * player);
void p_toggle_sprint(player_t * player);
void player_update(player_t * player);
