#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "scene.h"
#include "gameplay_scene.h"
#include "graphics.h"

struct Data
{
    SDL_Texture *tiles;
};
typedef struct Data Data;

void gameplay_scene_start(Scene *s)
{
    Data *data = malloc(sizeof(Data));
    scene_set_data(s, data);
}

void gameplay_scene_update(Scene *s, int dt, const InputState *input)
{
}

void gameplay_scene_draw(Scene *s, SDL_Renderer *renderer)
{
}

void gameplay_scene_stop(Scene *s)
{
    Data *data = scene_get_data(s);
    free(data);
}
