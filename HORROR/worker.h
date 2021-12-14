#pragma once
#include "daemon.h"
#include "loader.h"

void worker_init(daemon_t * dae, daemon_process_t * proc);
void worker_destroy(daemon_t * dae, daemon_process_t * proc, worker_t * self);
void worker_update(daemon_t * dae, daemon_process_t * proc, worker_t * self);