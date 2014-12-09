#include <stdio.h>
#include "scenes.h"

void test_scene_start(Scene *s)
{
    fprintf(stderr, "Started\n");
}

void test_scene_update(Scene *s, int dt)
{

}

void test_scene_draw(Scene *s, SDL_Renderer *renderer)
{    
}

void test_scene_stop(Scene *s)
{
    fprintf(stderr, "Stopping\n");
}
