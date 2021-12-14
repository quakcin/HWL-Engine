#include "data.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "main.h"

unsigned int GetVal (data_t * data, char * name)
{
	if (name[0] < 'a' || name[0] > 'z')
	{
		printf("[Data] ERROR, ILLEGAL NAME!\n");
		return (unsigned int)-1;
	}

	var_t * ptr;
	for (ptr = data->variables[name[0] - 'a']; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
			return ptr->value;

	printf("[Data] FIELD '%s' DOES NOT EXISTS!\n", name);

	return 0;
}

void SetVal (data_t * data, char * name, unsigned int value)
{
	if (name[0] < 'a' || name[0] > 'z')
	{
		printf("[Data] ERROR, ILLEGAL NAME!\n");
		return;
	}
	// 1. check if variable exists
	var_t * var = NULL;
	var_t * ptr;

	for (ptr = data->variables[name[0] - 'a']; ptr != NULL; ptr = ptr->next)
		if (strcmp(ptr->name, name) == 0)
		{
			var = ptr;
			break;
		}

	if (var != NULL)
	{
		// 2. if so, chage its value
		var->value = value;
	}
	else
	{
		// 3. ELSE , create it  -- ptr should be last list entry!
		var_t * var = malloc(sizeof(struct var_s));
		var->next = data->variables[name[0] - 'a'];
		var->value = value;
		strcpy(var->name, name);
		data->variables[name[0] - 'a'] = var;
		// printf("Set Value: %s to %u at %p\n", name, value, var);
	}
}

void parse_file(data_t * self, char * path)
{
	// -- read file
	FILE * file = fopen(path, "r");
	fseek(file, 0, SEEK_END);
	long bytes = ftell(file);
	rewind(file);

	char * buff = malloc(sizeof(char) * (bytes + 1));

	// -- parse file

	while (fgets(buff, bytes + 1, file))
	{
		char cmd[0xF] = "";
		char val[0xF] = "";
		bool name = true;

		unsigned int c;
		for (c = 0; c < strlen(buff); c++)
		{
			char cc = buff[c];
			// printf("%d %c\n", c, cc);
			if (cc == '\n')
				break;

			else if (cc == ' ')
				name = false;

			else if (name)
				strncat(cmd, &cc, 1);

			else
				strncat(val, &cc, 1);
		}

		if (strcmp(cmd, "-") == 0)
			printf("[Data] Found comment %s\n", val);
		else if (strcmp(cmd, "\n") == 0 || strcmp(cmd, "") == 0)
		{}
		else
		{
			// -- covert string to uint
			unsigned int value = (unsigned int) atoi(val);
			/*
			unsigned int i;
			
			for (i = 0; i < strlen(val); i++)
				value += (unsigned int)(val[i] - '0') * (unsigned int)( powf(
					10, ((float) (strlen(val) - i - 1))
				));													// -- look at this l8er
			*/
			SetVal(self, cmd, value);
			printf("READ %s := %u\n", cmd, value);
		}
	}
	fclose(file);
	free(buff);
}

void IncrVal (data_t * self, char * name)
{
	SetVal(self, name, GetVal(self, name) + 1);
}

void DecrVal(data_t * self, char * name)
{
	SetVal(self, name, GetVal(self, name) - 1);
}

// -- oop

data_t * create_data()
{
	data_t * self = malloc(sizeof(struct data_s));

	if (self == NULL)
		I_Error("Data", "Failed to create object");

	unsigned int i;
	for (i = 0; i < 26; i++)
	{
		self->variables[i] = malloc(sizeof(struct var_s));
		self->variables[i]->next = NULL;
		strcpy(self->variables[i]->name, "__VAR");
	}

	I_Print("Data", "Object created!");
	return self;
}

void destroy_data(data_t * self)
{
	var_t * ptr;
	unsigned int i;
	for (i = 0; i < 26; i++)
	{
		var_t * prev = NULL;
		for (ptr = self->variables[i]; ptr != NULL; ptr = ptr->next)
		{
			if (prev != NULL)
				free(prev);
			prev = ptr;
		}
		if (prev != NULL)
			free(prev);
	}

	I_Print("Data", "Object destroyed!");
	free(self);
}