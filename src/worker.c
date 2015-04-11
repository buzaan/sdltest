#include "worker.h"
#include "glyph_ids.h"

// TODO: goal representation to be determined later.
struct Goal
{
    int placeholder; 
};

struct Worker
{
    struct Point location;
    TileMap *map;
    struct Goal goal;
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
    return out;
}

void worker_destroy(struct Worker *worker)
{
    free(worker);
}

void worker_update(struct Worker *worker, Uint32 dt)
{
    
}
