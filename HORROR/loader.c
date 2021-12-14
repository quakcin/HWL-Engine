#include "loader.h"
#include "main.h"

worker_t * create_worker(daemon_t * daemon)
{
	worker_t * worker = malloc(sizeof(struct worker_s));

	if (worker == NULL)
		I_Error("Worker", "Failed to create object");

	worker->engine = daemon->processes->shared;
	worker->schedule = create_scheduler();
	worker->overlay = create_overlay();
	worker->player = create_player();
	worker->sounds = create_sounds();
	worker->data = create_data();
	worker->map = create_map();
	worker->listener = create_listener(worker->engine->keyboard);
	worker->daemon = daemon;
	worker->ticks = 0;
	I_Print("Worker", "Object created!");
	return worker;
}

void destroy_worker(worker_t * worker)
{
	destroy_scheduler(worker->schedule);
	destroy_listener(worker->listener);
	destroy_overlay(worker->overlay);
	destroy_player(worker->player);
	destroy_sounds(worker->sounds);
	destroy_data(worker->data);
	destroy_map(worker->map);
	free(worker);
	I_Print("Worker", "Object destroyed!");
}

void inc_darkness(player_t * player)
{
	player->health += 1;
	printf("HP: %f\n", player->health);

	return;

	player->papprox += 2;
	 printf("Perception: %f\n", player->perception);
}


void dec_darkness(player_t * player)
{
	player->health -= 1;
	printf("HP: %f\n", player->health);

	return;
	player->papprox -= 2;
	printf("Perception: %f\n", player->perception);
}

void p_escape_key_event (worker_t * self)
{
#ifdef _DEBUG_MODE_
	daemon_process_halt(self->daemon, "worker");
	daemon_process_halt(self->daemon, "root");
#else
	daemon_process_halt(self->daemon, "worker");
	daemon_process_begin(self->daemon, "menu");
#endif
}

void render_loading_screen(daemon_t * dae)
{
	engine_t * engine = get_engine(dae);

	char loading_splash_path[0xFF];
	sprintf(loading_splash_path, "data/%u/splash/loading.png", engine->level);
	ALLEGRO_BITMAP * splash = al_load_bitmap(loading_splash_path);

	if (splash == NULL)
	{
		printf("[Loader] Faile to load splash screen: %s!\n", 
			loading_splash_path);
		return;
	}

	ALLEGRO_BITMAP * overlay = al_load_bitmap("data/overlay/loading.png");

	al_draw_scaled_bitmap(splash, 0, 0, 1280, 720, 0, 0, engine->width, engine->height, 0);
	al_draw_scaled_bitmap(overlay, 0, 0, 1280, 720, 0, 0, engine->width, engine->height, 0);
	al_flip_display();

	al_destroy_bitmap(splash);
	al_destroy_bitmap(overlay);
}

void loader_init(daemon_t * dae, daemon_process_t * proc)
{
	// -- alloc and load ressources for leading process (game worker)
	worker_t * worker = create_worker(dae);
	proc->shared = worker;

	// load main config file
	parse_file(worker->data, "data/config");

	// player_load_save(worker->player);
	map_load(worker->engine, worker->map, worker->data);

	// -- setup listener for keyboard events

	listen2(worker->listener, ALLEGRO_KEY_UP, p_move_forward, worker->player, worker->map);
	listen2(worker->listener, ALLEGRO_KEY_DOWN, p_move_backwards, worker->player, worker->map);

	listen(worker->listener, ALLEGRO_KEY_LEFT, p_look_left, worker->player);
	listen(worker->listener, ALLEGRO_KEY_RIGHT, p_look_right, worker->player);

#ifdef _DEBUG_MODE_
	listen(worker->listener, ALLEGRO_KEY_Q, inc_darkness, worker->player);
	listen(worker->listener, ALLEGRO_KEY_E, dec_darkness, worker->player);
#endif

	listen(worker->listener, ALLEGRO_KEY_V, p_toggle_sprint, worker->player);

	listen(worker->listener, ALLEGRO_KEY_ESCAPE, p_escape_key_event, worker);

	listener_debug(worker->listener);

	// -- render splash screen

	render_loading_screen(dae);

	// -- self kill
	daemon_process_halt(dae, "loader");
}


void loader_destroy(daemon_t * dae, daemon_process_t * proc, worker_t * self)
{
	// -- share pointers and raise leading process (game worker)
	daemon_share_memory(dae, "worker", "loader");
	daemon_share_memory(dae, "render", "loader");
	daemon_process_begin(dae, "render");
	daemon_process_begin(dae, "worker");
	// WARNING: From now on, there is a blob of data floating in zone,
	// make sure that WORKER_PROCESS will get rid of it when possible.
}
