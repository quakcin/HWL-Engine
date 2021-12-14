#include "keyboard.h"
#include "root.h"
#include "main.h"

keyboard_t * create_keyboard()
{
	keyboard_t * keyboard = malloc(sizeof(struct keyboard_s));

	if (keyboard == NULL)
		I_Error("Scheduler", "Failed to create object");

	I_Print("Keyboard", "Object created!");
	return keyboard;
}

void destroy_keyboard(keyboard_t * keyboard)
{
	free(keyboard);
	I_Print("Keyboard", "Object destroyed!");
}

void keyboard_update(keyboard_t * keyboard)
{
	ALLEGRO_KEYBOARD_STATE kbdstate;
	al_get_keyboard_state(&kbdstate);

	for (int i = 0; i < ALLEGRO_KEY_MAX; ++i)
		if (al_key_down(&kbdstate, i))
			keyboard->keymap[i] = true;
		else
			keyboard->keymap[i] = false;
}