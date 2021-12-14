#include "listener.h"
#include "main.h"

void listen
(
	listener_t * listener,
	char key,
	void(*proxy)(void * arg),
	void * arg
)
{
	listning_t * listning = malloc(sizeof(struct listning_s));

	if (listning == NULL)
		I_Error("Listener", "Failed to create listener subobject");

	listning->next = listener->events;
	listning->key = key;
	listning->proxy = proxy;
	listning->proxy2 = NULL;
	listning->proxy3 = NULL;
	listning->arg = arg;
	listener->events = listning;
}

void listen2
(
	listener_t * listener,
	char key,
	void(*proxy2)(void * arg, void * arg2),
	void * arg,
	void * arg2
)
{
	listning_t * listning = malloc(sizeof(struct listning_s));

	if (listning == NULL)
		I_Error("Listener", "Failed to create listener subobject");

	listning->next = listener->events;
	listning->key = key;
	listning->proxy = NULL;
	listning->proxy2 = proxy2;
	listning->proxy3 = NULL;
	listning->arg = arg;
	listning->arg2 = arg2;
	listener->events = listning;
}

void listen3
(
	listener_t * listener,
	char key,
	void(*proxy3)(void * arg, void * arg2, void * arg3),
	void * arg,
	void * arg2,
	void * arg3
)
{
	listning_t * listning = malloc(sizeof(struct listning_s));

	if (listning == NULL)
		I_Error("Listener", "Failed to create listener subobject");

	listning->next = listener->events;
	listning->key = key;
	listning->proxy = NULL;
	listning->proxy2 = NULL;
	listning->proxy3 = proxy3;
	listning->arg = arg;
	listning->arg2 = arg2;
	listning->arg2 = arg3;
	listener->events = listning;
}

listener_t * create_listener(keyboard_t * keyboard)
{
	listener_t * listener = malloc(sizeof(struct listener_s));

	if (listener == NULL)
		I_Error("Listener", "Failed to create object");

	listener->keyboard = keyboard;
	listener->events = NULL;
	I_Print("Listener", "Object created!");
	return listener;
}

void listener_debug(listener_t * listener)
{
	listning_t * ptr; int i = 0;
	for (ptr = listener->events; ptr != NULL; ptr = ptr->next)
	{
		printf("[%i] awaiting key: %c at addr: %p %p %p\n", i++, ptr->key, ptr->proxy, ptr->proxy2, ptr->proxy3);
	}
}

void listener_update (listener_t * listener)
{
	listning_t * ptr; int i = 0;
	for (ptr = listener->events; ptr != NULL; ptr = ptr->next)
		if (listener->keyboard->keymap[ptr->key])
			if (ptr->proxy != NULL)
				ptr->proxy(ptr->arg);
			else if (ptr->proxy2 != NULL)
				ptr->proxy2(ptr->arg, ptr->arg2);
			else if (ptr->proxy3 != NULL)
				ptr->proxy3(ptr->arg, ptr->arg2, ptr->arg3);
}

void destroy_listener(listener_t * listener)
{
	listning_t * prev = NULL;
	listning_t * ptr;
	for (ptr = listener->events; ptr != NULL; ptr = ptr->next)
	{
		if (prev != NULL)
			free(prev);
		prev = ptr;
	}
	if (prev != NULL)
		free(prev);
	free(listener);
	I_Print("Listener", "Object destroyed!");
}