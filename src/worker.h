#pragma once
#include "tile_map.h"

struct Worker;

/* Creates a worker on the given map at the (optional) position. If
 * position is not given worker will be created at (0, 0) */
struct Worker *worker_create(TileMap *environs, struct Point *position);

void worker_destroy(struct Worker *worker);
void worker_update(struct Worker *worker, Uint32 dt);

