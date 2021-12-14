#include "daemon.h"
#include "main.h"

#include <string.h>

daemon_t * create_daemon
(
	void(*init)(struct daemon_s * daemon, struct daemon_process_s * proc),
	void(*update)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared),
	void(*end)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared)
)
{
	// -- create daemon object
	daemon_t * daemon = malloc(sizeof(struct daemon_s));

	if (daemon == NULL)
		I_Error("Daemon", "Failed to create daemon object");

	// -- create process list
	daemon->processes = malloc(sizeof(struct daemon_process_s));
	daemon->processes->init = init;
	daemon->processes->update = update;
	daemon->processes->end = end;
	strcpy(daemon->processes->pname, "root");
	daemon->processes->next = NULL;
	daemon->processes->status = DAEMON_PROCESS_STATUS_ACTIVATING;
	I_Print("Daemon", "Object created!");
	return daemon;
}

void destroy_daemon(daemon_t * self)
{
	// first, destroy processes
	daemon_process_t * ptr = self->processes;
	daemon_process_t * prev = NULL;
	for (ptr; ptr != NULL; ptr = ptr->next)
	{
		if (prev != NULL)
			free(prev);
		prev = ptr;
	}

	if (prev != NULL)
		free(prev);

	// now, destroy daemon object
	free(self);
	I_Print("Daemon", "Object destroyed!");
}

void daemon_update(daemon_t * self)
{
	daemon_process_t * ptr = self->processes;
	for (ptr = self->processes; ptr != NULL; ptr = ptr->next)
	{
		// printf("Updating: %u, Status %u\n", ptr->process_id, ptr->status);
		self->process = ptr;
		if (ptr->status == DAEMON_PROCESS_STATUS_ACTIVE)
		{
			if (ptr->update != NULL)
				ptr->update(self, ptr, ptr->shared);
		}
		else if (ptr->status == DAEMON_PROCESS_STATUS_ACTIVATING)
		{
			ptr->status = DAEMON_PROCESS_STATUS_ACTIVE;
			if (ptr->init != NULL)
				ptr->init(self, ptr);
		}
		else if (ptr->status == DAEMON_PROCESS_STATUS_HALTING)
		{
			ptr->status = DAEMON_PROCESS_STATUS_HALTED;
			if (ptr->end != NULL)
				ptr->end(self, ptr, ptr->shared);
		}
	}
}

bool daemon_alive(daemon_t * self)
{
	return !(self->processes->status == DAEMON_PROCESS_STATUS_HALTED);
}

void daemon_kill(daemon_t * self)
{
	self->processes->status = DAEMON_PROCESS_STATUS_HALTED;
}

daemon_process_t * daemon_find_process(daemon_t * self, char * pname)
{
	daemon_process_t * ptr;
	for (ptr = self->processes; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->pname, pname) == 0)
			return ptr;
	return NULL;
}

void daemon_share_memory(daemon_t * self, char * proc1, char * proc2)
{
	daemon_process_t * p1, * p2;
	p1 = daemon_find_process(self, proc1);
	p2 = daemon_find_process(self, proc2);

	if (p1 == NULL || p2 == NULL)
		printf("[Error] Daemon can't share memory between non existant processes!\n");
	else
		p1->shared = p2->shared;
}

void daemon_register
(
	daemon_t * self,
	char * pname,
	void(*init)(struct daemon_s * daemon, struct daemon_process_s * proc),
	void(*update)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared),
	void(*end)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared)
)
{
	// find last element of the list
	daemon_process_t * ptr = self->processes;
	while (ptr->next != NULL)
		ptr = ptr->next;

	// create new process
	ptr->next = malloc(sizeof(struct daemon_process_s));
	ptr->next->next = NULL;
	ptr->next->shared = NULL;
	ptr->next->end = end;
	ptr->next->init = init;
	ptr->next->update = update;
	ptr->next->status = DAEMON_PROCESS_STATUS_HALTED;
	strcpy(ptr->next->pname, pname);
}

// -- Process Manipulation:

void daemon_process_begin(daemon_t * self, char * pname)
{
	daemon_process_t * proc = daemon_find_process(self, pname);
	if (proc != NULL)
		proc->status = DAEMON_PROCESS_STATUS_ACTIVATING;
	else
		printf("[ERROR] Procces cannot be activated!\n");
}

void daemon_process_halt(daemon_t * self, char * pname)
{
	daemon_process_t * proc = daemon_find_process(self, pname);
	if (proc != NULL)
		proc->status = DAEMON_PROCESS_STATUS_HALTING;
	else
		printf("[ERROR] Procces cannot be halted!\n");
}

void daemon_process_suspend(daemon_t * self, char * pname)
{
	daemon_process_t * proc = daemon_find_process(self, pname);
	if (proc != NULL)
		proc->status = DAEMON_PROCESS_STATUS_SUSPENDED;
	else
		printf("[ERROR] Procces cannot be suspended!\n");
}

void daemon_process_resume(daemon_t * self, char * pname)
{
	daemon_process_t * proc = daemon_find_process(self, pname);
	if (proc != NULL)
		if (proc->status == DAEMON_PROCESS_STATUS_SUSPENDED)
			proc->status = DAEMON_PROCESS_STATUS_ACTIVE;
		else
			printf("[ERROR] Daemon can only resume suspended processes!!!\n");
	else
		printf("[ERROR] Procces cannot be resumed!\n");
}