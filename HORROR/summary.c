#include "summary.h"
#include "main.h"

void s_quit_to_menu (summary_t * self)
{
	daemon_process_halt(self->worker->daemon, "summary");
	daemon_process_halt(self->worker->daemon, "worker");

#ifndef _DEBUG_MODE_
	daemon_process_begin(self->worker->daemon, "menu");
#else
	daemon_process_halt(self->worker->daemon, "root");
#endif

}

void s_continue (summary_t * self)
{
	daemon_process_halt(self->worker->daemon, "summary");
	daemon_process_halt(self->worker->daemon, "worker");

	if (self->dead == false)
		self->worker->engine->level++;

	if (self->worker->engine->level < self->worker->engine->lv_count)
		daemon_process_begin(self->worker->daemon, "loader");
	else
		daemon_process_begin(self->worker->daemon, "menu");
}

void summary_init (daemon_t * dae, daemon_process_t * proc)
{
	summary_t * self = malloc(sizeof(struct summary_s));
	self->worker = proc->shared;
	proc->shared = self;

	// -- hijack workers listener

	destroy_listener(self->worker->listener);
	self->worker->listener = create_listener(self->worker->engine->keyboard);

	listen(self->worker->listener, ALLEGRO_KEY_ESCAPE, s_quit_to_menu, self);
	listen(self->worker->listener, ALLEGRO_KEY_SPACE, s_continue, self);

	// -- load summary screen data
	self->dead = (self->worker->player->health < 0);

	if (self->dead)
		self->overlay = al_load_bitmap("data/overlay/dead.png");
	else
		self->overlay = al_load_bitmap("data/overlay/victory.png");

	if (self->overlay == NULL)
		I_Print("Summary", "Failed to load overlay!");

	// -- prepare game summary (self->msg)

	self->msg[0] = '\0';

	unsigned int sec = self->worker->ticks / 60;
	unsigned int min = sec / 60;
	sec = sec - min * 60;

	sprintf (
		self->msg, 
		"Play Time:  %um %us \r\n\r\n"
		"Smudged:  %u / %u \r\n"
		"Stars: %.*s \r\n"
		, min, sec, 
		(GetVal(self->worker->data, "f_total") > self->worker->map->stars
			? self->worker->map->stars
			: GetVal(self->worker->data, "f_total")),
		self->worker->map->stars,
		(GetVal(self->worker->data, "f_stars") != 0
			? GetVal(self->worker->data, "f_stars")
			: 5 ),
		(GetVal(self->worker->data, "f_stars") != 0
			? "******"
			: "none")
	);

	self->font = al_load_ttf_font (
		"data/fonts/PermanentMarker-Regular.ttf", 
		self->worker->engine->width / 30, 0
	);

}

void summary_update (daemon_t * dae, daemon_process_t * proc, summary_t * self)
{
	int width = self->worker->engine->width;
	int height = self->worker->engine->height;

	int unit = height / 5;
	// -- render overlay
	al_draw_scaled_bitmap(self->overlay, 0, 0, 1280, 720, 0, 0, width, height, 0);

	// -- render summary msg

	al_draw_multiline_text(self->font, al_map_rgb(255, 255, 255), width / 5, height / 2 - height / 12, width / 3, height / 14, 0, self->msg);

	// -- update hijacked listener
	listener_update(self->worker->listener);
}

void summary_destroy (daemon_t * dae, daemon_process_t * proc, summary_t * self)
{
	al_destroy_font(self->font);
	al_destroy_bitmap(self->overlay);
	// -- worker.c will handle the rest
}
