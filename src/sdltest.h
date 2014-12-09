#pragma once
#include <stdbool.h>
#include "scene.h"

struct game *game_create(char *name, int x_size, int y_size);
void game_destroy(struct game *game);
bool game_should_quit(struct game *game);
void game_tick(struct game *game);
struct scene *game_add_scene(struct game *game, SceneInit init, SceneUpdate update, SceneDraw draw);
void game_switch_to_scene(struct game *game, struct scene *scene);
