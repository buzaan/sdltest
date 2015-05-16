#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "tile_map.h"

// Temporary A* pathfinding data & functions
struct Path
{
    struct Point *points;
    size_t capacity;
    size_t size;
};

void path_init(struct Path *p);
void path_copy(const struct Path *src, struct Path *dest);

/* Finds a path between the two points on the map.  
 * 
 * Result given in the Path "out". A path size of zero indicates that
 * there was no route found between the two points.
 */
void path_from_to(struct Path *out, 
                  const TileMap *map, 
                  const struct Point *from, 
                  const struct Point *to);
void path_destroy(struct Path *p);

/* Finds nearest tile to start where pred is true. Output stored in
 * found; returns false on failure. */
bool path_nearest_tile(const struct TileMap *map,
                       const struct Point *start,
                       struct Point *found,
                       bool (*pred)(const TileInfo *tile));
void path_fprint(struct Path *path, FILE *file);
