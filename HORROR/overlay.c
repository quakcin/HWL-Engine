#include "overlay.h"
#include "main.h"

#include <math.h>

overlay_t * create_overlay()
{
	overlay_t * self = malloc(sizeof(struct overlay_s));


	if (self == NULL)
		I_Error("Overlay", "Failed to create object");

	self->next = NULL;
	self->count = 0;
	self->frame = 0;
	strcpy(self->name, "__LIST");

	I_Print("Overlay", "Object created!");
	return self;
}

void destroy_overlay(overlay_t * self)
{
	// -- destroy bitmaps
	overlay_t * ptr;
	unsigned int i;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		for (i = 0; i < ptr->count; i++)
			al_destroy_bitmap(ptr->frames[i]);

	// -- destroy list
	overlay_t * prev = NULL;
	for (ptr = self; ptr != NULL; ptr = ptr->next)
	{
		if (prev != NULL)
			free(prev);
		prev = ptr;
	}
	free(prev);
	I_Print("Overlay", "Object destroyed!");
}

void overlay_enable (overlay_t * self, char * name)
{
	overlay_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
		{
			if (ptr->type != PLAY_BACK)
				ptr->frame = 0;
			else
			{
				if (ptr->frame >= ptr->count)
					ptr->frame = 0;
			}

			ptr->enabled = 1;
			return;
		}
}

void overlay_disable (overlay_t * self, char * name)
{
	overlay_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
		{
			ptr->enabled = 0;
			return;
		}
}

void overlay_update(overlay_t * self)
{
	overlay_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (ptr->enabled == 1)
		{
			ptr->frame += ptr->speed;
			if (ptr->frame >= ptr->count)
				if (ptr->type == PLAY_ONCE)
					ptr->enabled = 0;
				else if (ptr->type == PLAY_LOOP)
					ptr->frame = 0;
		}
}

void overlay_advance(overlay_t * self, char * name)
{
	overlay_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
		{
			ptr->frame += 1;
			if (ptr->frame >= ptr->count)
				if (ptr->type == PLAY_ONCE)
					ptr->enabled = 0;
				else if (ptr->type == PLAY_LOOP)
					ptr->frame = 0;

			return;
		}
}

void overlay_supply
(
	overlay_t * self, 
	char * name, 
	char * path
)
{
	// -- check for dupes
	overlay_t * iter;
	for (iter = self->next; iter != NULL; iter = iter->next)
		if (strcmp(iter->name, name) == 0)
			return;	// means data is allready there

	// -- if no dupes just load data in
	overlay_t * ptr = malloc(sizeof(struct overlay_s));

	ptr->next = self->next;
	self->next = ptr;

	ptr->count = 1;
	ptr->enabled = 0;
	ptr->frame = 0;
	ptr->speed = 0;
	ptr->type = PLAY_LOOP;

	strcpy(ptr->name, name);
	ptr->frames[0] = al_load_bitmap(path);
	if (ptr->frames[0] != NULL)
		printf("[Overlay] Finished loading: %s\n", path);
	else
		printf("[Overlay] Failed to load: %s\n", path);
}

// -- overlay types: 0 - once, 1 - loop, 2 - mirror loop

void overlay_load (overlay_t * self, char * name, float speed, char type)
{
	overlay_t * ptr = malloc(sizeof(struct overlay_s));

	ptr->next = self->next;
	self->next = ptr;

	ptr->enabled = 0;
	ptr->frame = 0;
	ptr->speed = speed;
	ptr->type = type;

	strcpy(ptr->name, name);

	// -- load frames
	
	unsigned int i;
	for (i = 0; i < 32; i++)
	{
		char path[0xFF];
		sprintf(path, "data/overlay/%s/%u.png", name, i);

		if (al_filename_exists(path) == false)
			break;

		ptr->frames[i] = al_load_bitmap(path);
		ptr->count = i + 1;

		if (ptr->frames[i] != NULL)
			printf("[Overlay] Finished loading: %s\n", path);
		else
			printf("[Overlay] Failed to load: %s\n", path);
	}

}

void overlay_display
(
	overlay_t * self, 
	unsigned int switdh, 
	unsigned int sheight, 
	unsigned int width, 
	unsigned int height
)
{
	overlay_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (ptr->enabled == 1)
			al_draw_scaled_bitmap(ptr->frames[(unsigned int) floor(ptr->frame)],
				0, 0, switdh, sheight, 0, 0, width, height, 0);
}