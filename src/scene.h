#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

typedef void (*SceneInit)(void *data);
typedef void (*SceneUpdate)(int dt);
typedef void (*SceneDraw)(SDL_Renderer *renderer);

struct scene *scene_create(struct game *owner, SceneInit init, SceneUpdate update, SceneDraw draw);
void scene_update(struct scene *scene, int dt);
void scene_draw(struct scene *scene, SDL_Renderer *renderer);
void scene_destroy(struct scene*);
