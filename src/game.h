#pragma once
#include <stdbool.h>
#include "sdltest.h"

Game *game_create(char *name, int x_size, int y_size);
void game_destroy(Game *game);
bool game_should_quit(Game *game);
void game_tick(Game *game);
void game_add_scene(Game *game, Scene *scene);
void game_register_scene(Game *game, Scene *scene, SceneID scene_id);
void game_switch_to_scene(Game *game, SceneID scene_id);
SDL_Renderer *game_get_renderer(Game *game);
