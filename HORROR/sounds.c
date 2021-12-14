#include "sounds.h"
#include <stdlib.h>
#include "util.h"
#include "main.h"

sounds_t * create_sounds()
{
	sounds_t * self = malloc(sizeof(struct sounds_s));


	if (self == NULL)
		I_Error("Sounds", "Failed to create object");

	strcpy(self->name, "__LIST");
	self->next = NULL;
	self->count = 0;
	I_Print("Sounds", "Object created!");
	return self;
}

void destroy_sounds(sounds_t * self)
{
	sounds_t * ptr = NULL;
	unsigned int i;
	for (ptr = self; ptr != NULL; ptr = ptr->next)
		for (i = 0; i < ptr->count; i++)
			al_destroy_sample(ptr->sounds[i]);

	sounds_t * prev = NULL;
	for (ptr = self; ptr != NULL; ptr = ptr->next)
	{
		if (prev != NULL)
			free(prev);
		prev = ptr;
	}
	free(prev);
	I_Print("Sounds", "Object destroyed!");
}

void load_sound(sounds_t * self, char * location, char * name)
{
	sounds_t * sound = malloc(sizeof(struct sounds_s));

	sound->next = self->next;
	self->next = sound;

	strcpy(sound->name, name);

	unsigned int i;
	for (i = 0; i < 0xF; i++)
	{
		char path[64];
		sprintf(path, "%s/%s-%u.ogg", location, name, i);

		if (al_filename_exists(path) == false)
			break;

		sound->sounds[i] = al_load_sample(path);
		sound->count = i + 1;

		if (sound->sounds[i] == NULL)
			printf("[Sounds] Failed to load: %s\n", path);
		else
			printf("[Sounds] Finished Loading: %s\n", path);
	}

}

void load_local_sound(sounds_t * self, engine_t * engine, char * name)
{
	char path[32];
	sprintf(path, "data/%u/sounds", engine->level);
	if (al_filename_exists(path))
		load_sound(self, path, name);
	else
		printf("[Sounds] Missing File: %s\n", path);

}

void load_global_sound (sounds_t * self, char * name)
{
	load_sound(self, "data/sounds", name);
}

void play_sound (sounds_t * self, char * name)
{
	sounds_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
			break;

	if (ptr == NULL)
		printf("[Sounds] Failed to play: %s\n", name);
	else
	{
		unsigned int id = p_rand(0, ptr->count - 1, 1);
		if (ptr->sounds[id] != NULL)
			al_play_sample(ptr->sounds[id], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
	}
}

void play_specific_sound(sounds_t * self, char * name, unsigned int id)
{
	sounds_t * ptr;
	for (ptr = self->next; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
			break;

	if (ptr == NULL)
		printf("[Sounds] Failed to play specific sound: %s:%u\n", name, id);
	else
		if (id < ptr->count)
			if (ptr->sounds[id] != NULL)
				al_play_sample(ptr->sounds[id], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, 0);
}