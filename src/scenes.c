#include <stdio.h>
#include "scenes.h"

void test_scene_init(void *data)
{
}

void test_scene_update(int dt)
{
    fprintf(stderr, "%d\n", dt);
}

void test_scene_draw(SDL_Renderer *renderer)
{
}
