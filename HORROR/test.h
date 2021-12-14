#pragma once
#include "root.h"
#include "daemon.h"

#include <math.h>

#define DEG 0.0174527

typedef struct test_memory_s
{
	char * map;
	ALLEGRO_BITMAP * texture;
} test_memory_t;

void test_init(daemon_t * dae, daemon_process_t * proc);
void test_update(daemon_t * dae, daemon_process_t * proc, test_memory_t * data);
void test_destroy(daemon_t * dae, daemon_process_t * proc, test_memory_t * data);