#pragma once
#include "worker.h"
#include "map.h"

void p_lighter_use (worker_t * self);
void smudge_scheduled_update(worker_t * self);
void p_toggle_breath(worker_t * self);
void p_breath_out(worker_t * self);
void p_switch_to_summary(worker_t * self);
void p_kill_player(worker_t * self);
void a_ambient_engine(worker_t * self);

void p_victory_player_messesage(worker_t * self);
void p_kill_player_messesage(worker_t * self);