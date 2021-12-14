#include "root.h"
#include "main.h"

void root_init (daemon_t * dae, daemon_process_t * proc)
{
	engine_t * shared = malloc(sizeof(struct engine_s));
	proc->shared = shared;

	shared->keyboard = create_keyboard();
	shared->level = -1;
	shared->lv_count = 0;
	shared->brightness = 0;
	shared->beginner_hints = true;

	// -- conenct to allegro5

	al_init();

	shared->width = 1280;
	shared->height = 720;
	al_set_new_display_flags(ALLEGRO_RESIZABLE);

	shared->timer = al_create_timer(1.0 / 60);
	shared->display = al_create_display(shared->width, shared->height);

	al_install_mouse();
	al_init_image_addon();
	al_install_keyboard();

	al_install_audio();
	al_init_acodec_addon();
	al_reserve_samples(8);
	al_init_primitives_addon();

	al_init_font_addon();
	al_init_ttf_addon();

	al_start_timer(shared->timer);

	// -- set window things

	al_set_window_title(shared->display, "HWL Engine");
	ALLEGRO_BITMAP * icon = al_load_bitmap("data/icon.png");
	al_set_display_icon(shared->display, icon);


	// -- count levels in data directoery
	unsigned int i;
	for (i = 0; i < 0xFFFF; i++)
	{
		char path[0xFF];
		sprintf(path, "data/%u/tiles", i);
		if (al_filename_exists(path))
			shared->lv_count++;
		else
			break;
	}

}

void root_destroy(daemon_t * dae, daemon_process_t * proc, engine_t * data)
{
	destroy_keyboard(data->keyboard);
	al_destroy_timer(data->timer);
	al_destroy_display(data->display);
	free(data);
}

void root_update(daemon_t * dae, daemon_process_t * proc, engine_t * data)
{
	long long int original = al_get_timer_count(data->timer);
	while (original == al_get_timer_count(data->timer));
	keyboard_update(data->keyboard);
	// -- really bad idea: al_wait_for_vsync();
	al_flip_display();
}

engine_t * get_engine(daemon_t * dae)
{
	return dae->processes->shared;
}