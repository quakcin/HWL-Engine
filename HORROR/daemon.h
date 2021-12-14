#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define DAEMON_PROCESS_STATUS_ACTIVE		 0
#define DAEMON_PROCESS_STATUS_ACTIVATING 1
#define DAEMON_PROCESS_STATUS_HALTED		 2
#define DAEMON_PROCESS_STATUS_HALTING		 3
#define DAEMON_PROCESS_STATUS_SUSPENDED  4

typedef struct daemon_process_s
{
	struct daemon_process_s * next;
	void(*init)(struct daemon_s * daemon, struct daemon_process_s * proc);
	void(*update)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared);
	void(*end)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared);
	void * shared;
	char pname[0xF];
	unsigned int status;
} daemon_process_t;

typedef struct daemon_s
{
	struct daemon_process_s * processes;
	struct daemon_process_s * process;
} daemon_t;

daemon_t * create_daemon
(
	void(*init)(struct daemon_s * daemon, struct daemon_process_s * proc),
	void(*update)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared),
	void(*end)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared)
);
void destroy_daemon(daemon_t * self);
void daemon_update(daemon_t * self);
bool daemon_alive(daemon_t * self);
void daemon_kill(daemon_t * self);
daemon_process_t * daemon_find_process(daemon_t * self, char * pname);
void daemon_share_memory(daemon_t * self, char * proc1, char * proc2);
void daemon_register
(
	daemon_t * self,
	char * pname,
	void(*init)(struct daemon_s * daemon, struct daemon_process_s * proc),
	void(*update)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared),
	void(*end)(struct daemon_s * daemon, struct daemon_process_s * proc, void * shared)
);
void daemon_process_begin(daemon_t * self, char * pname);
void daemon_process_halt(daemon_t * self, char * pname);
void daemon_process_suspend(daemon_t * self, char * pname);
void daemon_process_resume(daemon_t * self, char * pname);