#include "menu.h"
#include "main.h"

#define GHOSTING_TIME 15

void m_switch(menu_t * self, int shift, bool user_input)
{
	char old_overlay[0xF];
	sprintf(old_overlay, "s%d", self->selected);
	overlay_disable(self->overlay, old_overlay);
	overlay_disable(self->overlay, "won");

	self->selected += shift;

	if (self->selected >= self->wrap)
		self->selected = 0;
	else if (self->selected < 0)
		self->selected = self->wrap - 1;

	char overlay_path[0xFF];
	sprintf(overlay_path, "data/%d/splash/menu.png", self->selected - 2);

	char victory_path[0xFF];
	sprintf(victory_path, "data/progress/m%d", self->selected - 2);

	char new_overlay[0xF];
	sprintf(new_overlay, "s%d", self->selected);

	if (self->selected - 2 >= 0)
		overlay_supply(self->overlay, new_overlay, overlay_path);

	overlay_enable(self->overlay, new_overlay);

	if (al_filename_exists(victory_path))
		overlay_enable(self->overlay, "won");

	if (user_input)
		play_sound(self->sounds, "typewriter");
}

void m_disable_ghosting(menu_t * self)
{
	self->ghosting = false;
}

void m_load_map(menu_t * self)
{
	engine_t * engine = get_engine(self->daemon);
	engine->level = self->selected - 2;
	daemon_process_begin(self->daemon, "loader");
	daemon_process_halt(self->daemon, "menu");
}

void m_select(menu_t * self)
{
	if (self->ghosting)
		return;

	// -- selected

	if (self->selected > 1)
	{
		self->listening = false;
		overlay_enable(self->overlay, "fade");
		schedule(self->scheduler, m_load_map, self, 35);
	}
	else if (self->selected == 0)
		m_switch(self, 1, true);
	else
		m_switch(self, -1, true);

	// -- selected ends

	self->ghosting = true;
	schedule(self->scheduler, m_disable_ghosting, self, GHOSTING_TIME);
	play_sound(self->sounds, "typewriter");
}

void m_switch_left (menu_t * self)
{
	if (self->ghosting)
		return;

	m_switch(self, -1, true);

	self->ghosting = true;
	schedule(self->scheduler, m_disable_ghosting, self, GHOSTING_TIME);
}

void m_switch_right (menu_t * self)
{
	if (self->ghosting)
		return;

	m_switch(self, 1, true);

	self->ghosting = true;
	schedule(self->scheduler, m_disable_ghosting, self, GHOSTING_TIME);
}

void m_quit (menu_t * self)
{
	daemon_process_halt(self->daemon, "menu");
	daemon_process_halt(self->daemon, "root");
}

void m_display_config (menu_t * self)
{
	// -- return if loaded
	static bool loaded = false;

	if (loaded)
		return;

	// -- get user screen resolution
	engine_t * engine = get_engine(self->daemon);
	ALLEGRO_MONITOR_INFO minf; 
	al_get_monitor_info(0, &minf);

	SetVal(self->data, "width", (unsigned int) (minf.x2 - minf.x1 + 1));
	SetVal(self->data, "height", (unsigned int) (minf.y2 - minf.y1 + 1));
	SetVal(self->data, "full", 1);

	parse_file(self->data, "data/display");

	engine->width = GetVal(self->data, "width");
	engine->height = GetVal(self->data, "height");

	engine->zopt = (unsigned int) GetVal(self->data, "trspeed");
	engine->vopt = (unsigned int) GetVal(self->data, "vispeed");

	al_resize_display(engine->display, engine->width, engine->height);

	if (GetVal(self->data, "full") == 1)
	{
		al_set_window_position(engine->display, 0, 0);
		al_set_display_flag(engine->display, ALLEGRO_RESIZABLE, false);
		al_set_display_flag(engine->display, ALLEGRO_FULLSCREEN, true);
		al_set_display_flag(engine->display, ALLEGRO_NOFRAME, true);
	}
	else
	{
		al_set_display_flag(engine->display, ALLEGRO_FULLSCREEN, false);
		al_set_display_flag(engine->display, ALLEGRO_NOFRAME, false);
		al_set_display_flag(engine->display, ALLEGRO_RESIZABLE, true);
	}

	engine->brightness = (float)GetVal(self->data, "bright");
	loaded = true;

}

void background_sound_player (menu_t * self)
{
	play_sound(self->sounds, "ambience");
	schedule(self->scheduler, background_sound_player, self, 50 * 60);
}

void menu_init(daemon_t * dae, daemon_process_t * proc)
{
	engine_t * engine = get_engine(dae);
	menu_t * self = malloc(sizeof(struct menu_s));

	self->daemon = dae;
	proc->shared = self;


	self->overlay = create_overlay();
	self->listener = create_listener(engine->keyboard);
	self->scheduler = create_scheduler();
	self->sounds = create_sounds();
	self->data = create_data();

	self->selected = 1;
	self->wrap = engine->lv_count + 2;
	self->ghosting = false;
	self->listening = true;

	load_global_sound(self->sounds, "typewriter");
	load_global_sound(self->sounds, "ambience"); // -- change me
	background_sound_player(self);

	listen(self->listener, ALLEGRO_KEY_LEFT, m_switch_left, self);
	listen(self->listener, ALLEGRO_KEY_RIGHT, m_switch_right, self);
	listen(self->listener, ALLEGRO_KEY_SPACE, m_select, self);
	listen(self->listener, ALLEGRO_KEY_ESCAPE, m_quit, self);

	overlay_load(self->overlay, "fade", 0.25, PLAY_ONCE);
	overlay_supply(self->overlay, "won", "data/overlay/victorious.png");
	overlay_supply(self->overlay, "overlay", "data/overlay/menu.png");
	overlay_supply(self->overlay, "s0", "data/overlay/outro.png");
#ifdef _DIRECTOR_CUT_
	overlay_supply(self->overlay, "s1", "data/overlay/introDC.png");
#else
	overlay_supply(self->overlay, "s1", "data/overlay/intro.png");
#endif

	overlay_enable(self->overlay, "overlay");
	overlay_enable(self->overlay, "s1");

	m_display_config(self);

	unsigned int i;
	for (i = 0; i < engine->level + 1; i++)
		m_switch(self, 1, false);
}

void menu_update(daemon_t * dae, daemon_process_t * proc, menu_t * self)
{
	// DO NOTHING
	engine_t * engine = get_engine(dae);
	scheduler_update(self->scheduler);
	if (self->listening)
		listener_update(self->listener);
	overlay_display(self->overlay, 1280, 720, engine->width, engine->height);
	overlay_update(self->overlay);
}

void menu_destroy(daemon_t * dae, daemon_process_t * proc, menu_t * self)
{
	destroy_listener(self->listener);
	destroy_overlay(self->overlay);
	destroy_scheduler(self->scheduler);
	destroy_sounds(self->sounds);
	destroy_data(self->data);
	free(self);
}
