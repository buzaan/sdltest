#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

void test_scene_start(Scene *s);
void test_scene_update(Scene *s, int dt);
void test_scene_draw(Scene *s, SDL_Renderer *renderer);
void test_scene_stop(Scene *s);
