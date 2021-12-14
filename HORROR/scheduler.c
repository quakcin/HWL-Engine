#include "scheduler.h"
#include <stdlib.h>
#include "main.h"


scheduler_t * create_scheduler()
{
	scheduler_t * scheduler = malloc(sizeof(struct scheduler_s));

	if (scheduler == NULL)
		I_Error("Scheduler", "Failed to create object");

	scheduler->next = NULL;
	scheduler->prev = scheduler;
	I_Print("Scheduler", "Object created!");
	return scheduler;
}

void destroy_scheduler(scheduler_t * self)
{
	scheduler_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		free(ptr->prev);
	free(ptr);
	I_Print("Scheduler", "Object destroyed!");
}

void scheduler_update(scheduler_t * scheduler)
{
	scheduler_t * ptr = scheduler->next;
	while (ptr != NULL)
	{
		// printf("[Scheduler] %p %p %u\n", ptr->task, ptr->arg, ptr->ticks);
		if (ptr->ticks == 0)
		{
			// invoke task

			if (ptr->task != NULL)
				ptr->task(ptr->arg);

			// remove ptr from list
			ptr->prev->next = ptr->next;
			if (ptr->next != NULL)
				ptr->next->prev = ptr->prev;
			scheduler_t * nxt = ptr->next;
			free(ptr);
			ptr = nxt;
		}
		else
		{
			ptr->ticks -= 1;
			ptr = ptr->next;
		}
	}
}

void schedule
(
	scheduler_t * scheduler, 
	void(*task)(void * arg), void * arg, unsigned int ticks
)
{
	scheduler_t * self = malloc(sizeof(struct scheduler_s));

	if (scheduler->next != NULL)
		scheduler->next->prev = self;

	self->next = scheduler->next;
	self->prev = scheduler;
	scheduler->next = self;
	self->task = task;
	self->arg = arg;
	self->ticks = ticks;
}