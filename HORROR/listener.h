#pragma once
#include "keyboard.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct listning_s
{
	struct listning_s * next;
	void(*proxy)(void * arg);
	void(*proxy2)(void * arg, void * arg2);
	void(*proxy3)(void * arg, void * arg2, void * arg3);
	void * arg;
	void * arg2;
	void * arg3;
	char key;
} listning_t;

typedef struct listener_s
{
	struct listning_s * events;
	keyboard_t * keyboard;
} listener_t;

listener_t * create_listener(keyboard_t * keyboard);
void listener_debug(listener_t * listener);
void destroy_listener(listener_t * listener);
// void listener_event(listener_t * listener, char key, void(*proxy)(void * ptr), void * ptr);
void listen
(
	listener_t * listener,
	char key,
	void(*proxy)(void * arg),
	void * arg
);
void listen2
(
	listener_t * listener,
	char key,
	void(*proxy2)(void * arg, void * arg2),
	void * arg,
	void * arg2
);
void listen3
(
	listener_t * listener,
	char key,
	void(*proxy3)(void * arg, void * arg2, void * arg3),
	void * arg,
	void * arg2,
	void * arg3
);

void listener_update(listener_t * listener);