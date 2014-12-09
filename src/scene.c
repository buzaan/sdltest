#include <stdlib.h>
#include "scene.h"

struct scene
{
    SceneInit init;
    SceneUpdate update;
    SceneDraw draw;
};

void default_init(void *data) {}
void default_update(int dt) {}
void default_draw(SDL_Renderer *renderer) {}

struct scene *scene_create(
    SceneInit init, 
    SceneUpdate update,
    SceneDraw draw)
{
    struct scene *out = malloc(sizeof(struct scene));
    out->init = init ? init : default_init;
    out->update = update ? update : default_update;
    out->draw = draw ? draw : default_draw;
    return out;
}

void scene_destroy(struct scene *scene)
{
    free(scene);
}

void scene_set_init(struct scene *scene, SceneInit init)
{
    scene->init = init;
}

void scene_set_update(struct scene *scene, SceneUpdate update)
{
    scene->update = update;
}

void scene_set_draw(struct scene *scene, SceneDraw draw)
{
    scene->draw = draw;
}

void scene_update(struct scene *scene, int dt)
{
    scene->update(dt);
}

void scene_draw(struct scene *scene, SDL_Renderer *renderer)
{
    scene->draw(renderer);
}
