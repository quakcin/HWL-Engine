#include "player.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "main.h"
#include "sounds.h"

player_t * create_player ()
{
	player_t * player = malloc(sizeof(struct player_s));

	if (player == NULL)
		I_Error("Player", "Failed to create object");

	player->x = (float)((128 << 7) + (128 >> 1));
	player->y = (float)((128 << 7) + (128 >> 1));
	player->ang = 270;
	player->health = 100;
	player->height = 0;
	player->happrox = 0;
	player->speed = 5;
	player->energy = 100;
	player->penelty = 0;
	player->perception = 0;
	player->papprox = 0;
	player->walk = false;
	player->panic = false;
	player->door_event = 0;
	player->alive = true;
	I_Print("Player", "Object created!");
	return player;
}

void destroy_player(player_t * player)
{
	free(player);
	I_Print("Player", "Object destroyed!");
}

void player_update(player_t * player)
{
	// -- update player speed

	if (player->speed > 5)
	{
		player->speed -= 0.05;
		player->energy -= (player->panic) ? 8 : 5;
	}
	else
	{
		player->energy += 1;
		if (player->energy > 100)
			player->energy = 100;
	}
	// -- update perception
	if (player->perception < player->papprox)
		player->perception += 0.5;
	else if (player->perception > player->papprox)
		player->perception -= 0.25;

	// -- update eyes height

	const float height_step = (player->speed > 5) ? 1.5 : 0.5;

	if (player->height < player->happrox)
		player->height += height_step;
	else if (player->height > player->happrox)
		player->height -= height_step;
}

#define COLLIDER_SIZE 8
#define DOOR_SIZE 256

void p_move
(
	player_t * player,
	map_t * map,
	float vx, 
	float vy
)
{
	if (player->alive == false)
		return;

	float lx, ly;
	for (lx = -COLLIDER_SIZE; lx <= COLLIDER_SIZE; lx += COLLIDER_SIZE * 2)
		for (ly = -COLLIDER_SIZE; ly <= COLLIDER_SIZE; ly += COLLIDER_SIZE * 2)
		{
			int cx = ((int)floor(vx + lx)) >> 7;
			int cy = ((int)floor(vy + ly)) >> 7;
			unsigned int tile = (map->tiles[(cy << 8) + cx]);

			if (tile > map->door && tile <= map->props)
			{
				int px = ((int)vx) >> 7;
				int py = ((int)vy) >> 7;

				const int vdx = cx - px;
				const int vdy = cy - py;

				if ((abs(vdx) == 1 && abs(vdy) == 1) || (abs(vdx) == 0 && abs(vdy) == 0))
					return;

				player->x = (64 + (float)(cx << 7)) + vdx * DOOR_SIZE;
				player->y = (64 + (float)(cy << 7)) + vdy * DOOR_SIZE;

				player->door_event = tile;
				return;
			}
			else if (tile != 0 && tile <= map->colliders)
				return; // -- colliding with wall
		}

	player->x = vx;
	player->y = vy;
	player->walk = true;
}

void p_move_forward(player_t * player, map_t * map)
{
	p_move
	(
		player, map,
		player->x + cos(player->ang * DEG) * player->speed,
		player->y + sin(player->ang * DEG) * player->speed
	);
}

void p_move_backwards (player_t * player, map_t * map)
{
	p_move
	(
		player, map,
		player->x - cos(player->ang * DEG) * 5,
		player->y - sin(player->ang * DEG) * 5
	);
}

void p_toggle_sprint(player_t * player)
{
	if (player->energy >= 90)
		player->speed = 13;
}

void p_look_left (player_t * player)
{
	player->ang -= (player->panic) ? 5 : 2;
	if (player->ang < 0)
		player->ang = 360;
}

void p_look_right (player_t * player)
{
	player->ang += (player->panic) ? 5 : 2;
	if (player->ang >= 360)
		player->ang = 0;
}