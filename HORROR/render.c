#include "render.h"
#include <math.h>	
#include "util.h"
#include "main.h"

static int r_get_opacity(worker_t * self, float r, float global)
{
#ifdef _USE_LEGACY_SHADER_
	const float percp = global + self->player->perception;
	const float opc = -1 * (10000 * (percp / 8) / (r + 39 * percp)) + 256;
	return (int)opc;
#else

	const float perc = global + self->player->perception;
	const float opc = -1 * ((999 * perc) / r)  + 210;

	if (opc < 20)
		return 20;
	else if (opc > 230)
		return 230;
	else
		return (int)opc;
#endif
}

static float scan_lines[0xFFF];

void render_proc_update (daemon_t * dae, daemon_process_t * proc, worker_t * self)
{
	const int width = self->engine->width;
	const int height = self->engine->height;
	const float bright = self->engine->brightness;

	const int half_hei = height >> 1;

	al_draw_filled_rectangle(0, 0, width, half_hei + self->player->height, self->map->sky);
	al_draw_filled_rectangle(0, half_hei + self->player->height, width, height << 1, self->map->floor);

	// -- floor and ceiling shading

	al_draw_filled_rectangle(0, 0, width, height, 
		al_map_rgba(0, 0, 0, r_get_opacity(self, 512, bright))
	);

	const int fov = 60;
	const int halfFov = fov >> 1;

	const int VOPT = self->engine->vopt;
	const int ZOPT = self->engine->zopt;

	float a, r;
	const float delta = VOPT * ((float)fov) / width;
	int offsetX = 0;

	int prop_x = 0;
	int prop_y = 0;

	for (a = -halfFov; a < halfFov; a += delta)
	{
		float ang = (a + self->player->ang) * DEG;

		const float vx = cos(ang) * ZOPT;
		const float vy = sin(ang) * ZOPT;

		float cx = self->player->x;
		float cy = self->player->y;
		scan_lines[offsetX] = 0;

		for (r = 0; r < 0xFFF; r += ZOPT)
		{
			cx += vx;
			cy += vy;

			const int mx = (int)cx >> 7;
			const int my = (int)cy >> 7;

			const int pos = (my << 8) + mx;

			const uint8_t tile = self->map->tiles[pos];

			if (tile > self->map->props)
			{
				// its a prop
				prop_x = mx;
				prop_y = my;
			}
			else if (tile != 0)
			{
				// -- toying with r here will produce distortion!

				const float z = cos(a * DEG) * r;
				// -- draw

				const float h = height * 256 / z;
				const int offsetY = (((int)(height - h)) >> 1) + self->player->height;

				int i;
				for (i = 0; i < VOPT; i++)
					scan_lines[offsetX + i] = h;

				// -- texturizer

				const int cx_off = (int)cx & 127;
				const int cy_off = (int)cy & 127;
				int tx_off = (cx_off == 0 || cx_off == 127)
					? cy_off : cx_off;

				al_draw_scaled_bitmap(
					self->map->textures[tile],
					tx_off, 0, VOPT, 157,
					offsetX, offsetY,
					VOPT, h, 0
				);
				// -- shader
				al_draw_filled_rectangle(offsetX, offsetY, offsetX + VOPT, offsetY + h, 
					al_map_rgba(0, 0, 0, r_get_opacity(self, r, bright)));
				break;
			}
		}
		offsetX += VOPT;
	}

	// -- render props

	if (prop_x != 0)	// || prop_y != 0
	{
		float dx = (float)(prop_x << 7) + 64 - self->player->x;
		float dy = (float)(prop_y << 7) + 64 - self->player->y;

		float dist = (float)sqrt(dx * dx + dy * dy);

		if (dist < 64)
			goto _escape_prop_renderer;

		const float ang = atan2(dy, dx) - (self->player->ang * DEG);
		const float view_dist = (width / 2) / tan(fov * DEG / 2);
		const int size = (int)((view_dist / (cos(ang) * dist)) * 180);
		const float x = tan(ang) * view_dist;
		const int x_off = (int)(width / 2 + x - (size / 2));
		const int y_off = (int)((height - size) / 2 + self->player->height);


		uint8_t tx = self->map->tiles[(prop_y << 8) + prop_x];

		int clip_left = 0;
		int clip_right = 0;

		for (clip_left = 0; clip_left < size; clip_left++)
		{
			const int pos = x_off + clip_left;
			if (pos < 0 || pos >= width)
				continue;

			if (size >= scan_lines[pos])
				break;
		}


		for (clip_right = 0; clip_right < size; clip_right++)
		{
			const int pos = x_off + size - clip_right;
			if (pos < 0 || pos >= width)
				continue;

			if (size >= scan_lines[x_off + size - clip_right])
				break;
		}

		const float unit = 128 / (float)size;

		if (!((clip_left == clip_right) && clip_left != 0))
			al_draw_scaled_bitmap
			(
				self->map->textures[tx], (int)clip_left * unit,
				0, (size - (clip_left + clip_right)) * unit,
				157, x_off + clip_left, y_off,
				(size - (clip_left + clip_right)),
				size, 0
			);

	}

_escape_prop_renderer:

	// -- render rest of engine stuff

	overlay_display(self->overlay, 640, 360, width, height);

	if (self->player->health < 90)
	{
		float strength = 255 - self->player->health * 2.55;
		al_draw_filled_rectangle(0, 0, width, height, al_map_rgba(50, 0, 0, strength));
	}

}
