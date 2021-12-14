#pragma once
#include <stdbool.h>

typedef struct keyboard_s
{
	bool keymap[227];
} keyboard_t;

keyboard_t * create_keyboard();
void destroy_keyboard(keyboard_t * keyboard);
void keyboard_update(keyboard_t * keyboard);