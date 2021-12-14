#pragma once

// -- szatanie, dopomósz

typedef struct scheduler_s
{
	struct scheduler_s * next;
	struct scheduler_s * prev;
	void(*task)(void * arg);
	void * arg;
	unsigned int ticks;
} scheduler_t;


void schedule
(
	scheduler_t * scheduler,
	void(*task)(void * arg), void * arg, unsigned int ticks
);

void scheduler_update(scheduler_t * scheduler);
scheduler_t * create_scheduler();
void destroy_scheduler(scheduler_t * self);