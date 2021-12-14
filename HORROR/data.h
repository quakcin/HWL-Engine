#pragma once

typedef struct var_s
{
	struct var_s * next;
	char name[0xF];
	unsigned int value;
} var_t;

typedef struct data_s
{
	var_t * variables[26];
} data_t;


data_t * create_data();
void destroy_data(data_t * self);
void parse_file(data_t * self, char * path);
void SetVal(data_t * data, char * name, unsigned int value);
unsigned int GetVal(data_t * data, char * name);
void IncrVal(data_t * self, char * name);
void DecrVal(data_t * self, char * name);
