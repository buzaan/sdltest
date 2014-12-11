#pragma once
#include <stdbool.h>
#include "sdltest.h"

bool input_read_keymap(char const *file, InputKeymap *keymap);
void input_update(InputState *state, InputKeymap *keymap);
