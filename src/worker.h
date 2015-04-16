#pragma once
#include <SDL2/SDL.h>
#include "tile_map.h"
#include "path.h"

struct Worker;

/* Creates a worker on the given map at the (optional) position. If
 * position is not given worker will be created at (0, 0) */
struct Worker *worker_create(TileMap *environs, struct Point *position);
void worker_destroy(struct Worker *worker);
void worker_update(struct Worker *worker, Uint32 dt);
void worker_draw(struct Worker *worker, SDL_Renderer *renderer);
struct Point *worker_get_location(struct Worker *worker);

// Temporary testing API
void worker_set_path(struct Worker *worker, struct Path *p);
