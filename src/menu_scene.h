#pragma once
#include "sdltest.h"

void menu_scene_start(Scene *scene);
void menu_scene_update(Scene *scene, int dt);
void menu_scene_draw(Scene *scene, SDL_Renderer *renderer);
void menu_scene_stop(Scene *scene);
