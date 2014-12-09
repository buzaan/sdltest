#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

typedef void (*SceneStart)(Scene *s);
typedef void (*SceneUpdate)(Scene *s, int dt);
typedef void (*SceneDraw)(Scene *s, SDL_Renderer *renderer);
typedef void (*SceneStop)(Scene *s);

Scene *scene_create(Game *owner);
void scene_destroy(Scene *s);
void scene_set_start(Scene *s, SceneStart init);
void scene_set_update(Scene *s, SceneUpdate update);
void scene_set_draw(Scene *s, SceneDraw draw);
void scene_set_stop(Scene *s, SceneStop destroy);

void scene_set_data(Scene *s, void *data);
void *scene_get_data(Scene *s);

Game *scene_get_game(Scene *s);

void scene_start(Scene *s);
void scene_update(Scene *s, int dt);
void scene_draw(Scene *s, SDL_Renderer *renderer);
void scene_stop(Scene *s);

     
