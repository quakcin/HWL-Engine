#include "test.h"

#include <stdio.h>

void test_init (daemon_t * dae, daemon_process_t * proc)
{
	test_memory_t * shared = malloc(sizeof(struct test_memory_s));
	proc->shared = shared;

	shared->map = malloc(sizeof(char) * 65);
	shared->map = "1212121220000001100000022000000110003002200000011000000212121212";

	shared->texture = al_load_bitmap("data/textures/2.png"); 

}

void test_update(daemon_t * dae, daemon_process_t * proc, test_memory_t * data)
{

	int fov = 60;
	int halfFov = fov >> 1;

	static float x = 3 << 7;
	static float y = 3 << 7;
	static int ans = 270;

	const int movement_speed = 8;
	const int turning_speed = 2;

	// -- awt

	ALLEGRO_KEYBOARD_STATE kbdstate; 
	bool pressed_key[ALLEGRO_KEY_MAX];
		al_get_keyboard_state(&kbdstate);

	for (int i = 0; i < ALLEGRO_KEY_MAX; ++i)
	{
		if (al_key_down(&kbdstate, i))
			pressed_key[i] = true;
		else
			pressed_key[i] = false;
	}

	if (pressed_key[1])
	{
		ans -= turning_speed;
		if (ans < 0)
			ans = 360;
	}
	if (pressed_key[4])
	{
		ans += turning_speed;
		if (ans >= 360)
			ans = 0;
	}

	if (pressed_key[23])
	{
		x += cos(ans * DEG) * movement_speed;
		y += sin(ans * DEG) * movement_speed;
	}

	if (pressed_key[19])
	{
		x -= cos(ans * DEG) * movement_speed;
		y -= sin(ans * DEG) * movement_speed;
	}

	// -- sim consts

	const int s_width = 960;
	const int s_height = 540;

	// -- renderer

	al_draw_filled_rectangle(0, 0, s_width, s_height >> 1, al_map_rgb(110, 231, 255));
	al_draw_filled_rectangle(0, (s_height >> 1), s_width, s_height, al_map_rgb(130, 130, 130));

	float a, r;
	float delta = ((float)fov) / s_width;
	int offsetX = 0;

	for (a = -halfFov; a < halfFov; a += delta)
	{
		float ang = (a + ans) * DEG;

		float vx = cos(ang);
		float vy = sin(ang);

		float cx = x;
		float cy = y;

		for (r = 0; r < 0xFFF; r++)
		{
			cx += vx;
			cy += vy;

			int mx = (int)cx >> 7;
			int my = (int)cy >> 7;

			if (mx < 0 || my < 0 || mx > 7 || my > 7)
				continue;

			int pos = (my << 3) + mx;

			if (data->map[pos] != '0')
			{
				float z = cos(a * DEG) * r;
				// -- draw

				float h = (s_height << 8) / z;
				int offsetY = (((int)(s_height - h)) >> 1);

				// -- texturizer

				int cx_off = (int)cx & 127;
				int cy_off = (int)cy & 127;
				int tx_off = (cx_off == 0 || cx_off == 127) 
										 ? cy_off : cx_off;

				al_draw_scaled_bitmap(data->texture, tx_off, 0, 1, 157, offsetX, offsetY, 1, h, 0);

				// -- shader
				const int opc = ((int)r >> 2) + ((int)r >> 4);
				al_draw_line(offsetX, offsetY, offsetX, offsetY + h, 
					al_map_rgba(0, 0, 0, opc > 255 ? 255 : opc), 1);

				break;
			}
		}
		offsetX += 1;
	}
}

void test_destroy (daemon_t * dae, daemon_process_t * proc, test_memory_t * data)
{
	al_destroy_bitmap(data->texture);
	free(data->map);
	free(data);
}