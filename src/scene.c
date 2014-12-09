#include <stdlib.h>
#include "scene.h"
#include "sdltest.h"

struct scene_s
{
    void (*start)(struct scene_s *s);
    void (*update)(struct scene_s *s, int dt);
    void (*draw)(struct scene_s *s, SDL_Renderer *renderer);
    void (*stop)(struct scene_s *s);
    void *data;
    Game *owner;
};

// Do-nothing stubs
static void default_start(Scene *s) {}
static void default_update(Scene *s, int dt) {}
static void default_draw(Scene *s, SDL_Renderer *r) {}
static void default_stop(Scene *s) {}

Scene *scene_create(Game *owner)
{
    Scene *out = malloc(sizeof(Scene));
    out->start = default_start;
    out->update = default_update;
    out->draw = default_draw;
    out->stop = default_stop;
    out->data = NULL;
    out->owner = owner;
    return out;
}

void scene_destroy(Scene *s)
{
    free(s);
}

void scene_set_start(Scene *s, SceneStart start)
{
    s->start = start;
}

void scene_set_update(Scene *s, SceneUpdate update)
{
    s->update = update;
}

void scene_set_draw(Scene *s, SceneDraw draw)
{
    s->draw = draw;
}

void scene_set_stop(Scene *s, SceneStop stop)
{
    s->stop = stop;
}

void scene_set_data(Scene *s, void *data)
{
    s->data = data;
}

void *scene_get_data(Scene *s)
{
    return s->data;
}

Game *scene_get_game(Scene *s)
{
    return s->owner;
}

void scene_start(Scene *s)
{
    s->start(s);
}

void scene_update(Scene *s, int dt)
{
    s->update(s, dt);
}

void scene_draw(Scene *s, SDL_Renderer *renderer)
{
    s->draw(s, renderer);
}

void scene_stop(Scene *s)
{
    s->stop(s);
}

