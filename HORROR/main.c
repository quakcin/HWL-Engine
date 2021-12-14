/*
	Copyright (c) 2021 by:
	Marcin Œlusarczyk (lead developer)
	Maciej Bandura
	Szymon Œmiglarski

	Simple and inefficient Ray Casting Engine
	written in C using Allegro 5 Library,
	designed to run a simple horror-like game.
	Made as a school project.
*/

#include "main.h"
#include "root.h"
#include "daemon.h"
#include "test.h"
#include "loader.h"
#include "worker.h"
#include "render.h"
#include "summary.h"
#include "menu.h"

void I_Error (char * obj, char * msg)
{
	printf("\n\n[ERROR] [%s] %s!\n\n", obj, msg);
}

void I_Print (char * obj, char * msg)
{
	printf("[%s] %s\n", obj, msg);
}

int main()
{
	daemon_t * daemon = create_daemon(root_init, root_update, root_destroy);

	daemon_register(daemon, "loader", loader_init, NULL,							 loader_destroy);
	daemon_register(daemon, "worker", worker_init, worker_update,			 worker_destroy);
	daemon_register(daemon, "render", NULL,				 render_proc_update, NULL					 );

	daemon_register(daemon, "summary", summary_init, summary_update, summary_destroy);
	daemon_register(daemon, "menu", menu_init, menu_update, menu_destroy);


#ifndef _DEBUG_MODE_
	daemon_process_begin(daemon, "menu");
#else
	daemon_process_begin(daemon, "loader");
#endif

	while (daemon_alive(daemon))
		daemon_update(daemon);

	destroy_daemon(daemon);

	return 0;
} // ver 1.2