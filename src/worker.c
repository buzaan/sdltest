#include "worker.h"
#include "glyph_ids.h"

// TODO: goal/worker/entity representation to be determined later.
struct Goal
{
    int placeholder; 
};

struct Worker
{
    struct Point location;
    TileMap *map;
    struct Goal goal;

    // Temporary pathfinding stuff
    struct Path path;
    size_t progress;
    Uint32 time;

    unsigned int glyph;
};

struct Worker *worker_create(TileMap *environs, struct Point *position)
{
    struct Worker *out = malloc(sizeof(struct Worker));
    out->map = environs;
    if(position)
    {
        out->location = *position;
    }
    else
    {
        out->location.x = 0;
        out->location.y = 0;
    }
    out->goal.placeholder = 0;
    out->glyph = GLYPH_FACE;
    path_init(&out->path);
    return out;
}

void worker_destroy(struct Worker *worker)
{
    free(worker);
}

void worker_update(struct Worker *worker, Uint32 dt)
{
    worker->time += dt;
    if(worker->time > 500) // Worker moves 2x a second.
    {
        worker->time = 0;
        if(worker->progress < worker->path.size)
        {
            worker->progress++;
            worker->location = worker->path.points[worker->progress];
        }
    }
}

void worker_draw(struct Worker *worker, SDL_Renderer *renderer)
{
    struct SpriteSheet *sheet = tile_map_get_sprites(worker->map);
    sprite_sheet_draw(sheet, renderer, 
                      worker->location.x, 
                      worker->location.y,
                      worker->glyph);
}

void worker_set_path(struct Worker *worker, struct Path *path)
{
    path_copy(path, &worker->path);
}

struct Point *worker_get_location(struct Worker *worker)
{
    return &worker->location;
}
