#pragma once

#include <stdbool.h>

struct game *game_create(char *name, int x_size, int y_size);
void game_destroy(struct game *game);
bool game_should_quit(struct game *game);
void game_tick(struct game *game);
