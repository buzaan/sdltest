#pragma once
#include "tile_map.h"

// Temporary A* pathfinding data & functions
struct Path
{
    struct Point
    {
        int x;
        int y;
    } *points;
    size_t capacity;
    size_t size;
};
typedef struct Path Path;
typedef struct Point Point;

void path_init(Path *p);

/* Finds a path between the two points on the map.  
 * 
 * Result given in the Path "out". A path size of zero indicates that
 * there was no route found between the two points.
 */
void path_from_to(Path *out, const TileMap *map, 
                  const Point *from, const Point *to);

void path_destroy(Path *p);
void debug_map_bfs(TileMap *map, const Point *start);
