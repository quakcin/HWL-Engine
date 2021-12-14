#pragma once
#include "daemon.h"
#include "loader.h"

#define DEG 0.0174527

void render_proc_update(daemon_t * dae, daemon_process_t * proc, worker_t * self);