#pragma once
#include "tile_map.h"

// Temporary A* pathfinding data & functions
struct Path
{
    struct Point *points;
    size_t capacity;
    size_t size;
};

void path_init(struct Path *p);

/* Finds a path between the two points on the map.  
 * 
 * Result given in the Path "out". A path size of zero indicates that
 * there was no route found between the two points.
 */
void path_from_to(struct Path *out, const TileMap *map, 
                  const struct Point *from, const struct Point *to);
void path_destroy(struct Path *p);
