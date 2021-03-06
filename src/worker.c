#include "worker.h"
#include "glyph_ids.h"
#include "renderer.h"

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

    uint8_t glyph;
};

struct Worker *worker_create(TileMap *environs, struct Point *position)
{
    struct Worker *out = malloc(sizeof(*out));
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
    path_cleanup(&worker->path);
    free(worker);
}

void worker_update(struct Worker *worker, Uint32 dt)
{
    worker->time += dt;
    if(worker->time >= 250) // Worker moves 4x a second.
    {
        unsigned int steps = worker->time / 250;
        worker->time = worker->time % 250;
        while(steps > 0)
        {
            if(worker->progress < worker->path.size)
            {
                worker->location = worker->path.points[worker->progress];
                worker->progress++;
            }
            steps--;
        }
    }
}

void worker_draw(struct Worker *worker, struct Renderer *renderer)
{
    renderer_draw(renderer, 
                  worker->location.x, 
                  worker->location.y, 
                  worker->glyph);
}

void worker_set_path(struct Worker *worker, struct Path *path)
{
    path_copy(path, &worker->path);
    worker->progress = 0;
}

struct Point *worker_get_location(struct Worker *worker)
{
    return &worker->location;
}
