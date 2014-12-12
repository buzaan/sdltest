#include <stdlib.h>
#include "tile_map.h"

struct dim
{
    int w;
    int h;
};

struct TileMap
{
    struct dim bounds; // in tiles
    struct dim tiles; // in pixels
    Tile *data; 
    SDL_Texture *texture;
};

TileMap *tile_map_create(int width, int height, int tile_width, int tile_height)
{    
    TileMap *out = malloc(sizeof(TileMap));
    out->bounds.w = width;
    out->bounds.h = height;
    out->tiles.w = tile_width;
    out->tiles.h = tile_height;
    out->data = malloc(sizeof(Tile) * width * height);
    out->texture = NULL;
    return out;
}

void tile_map_load(TileMap *map, const char *file)
{
}

void tile_map_set_tile(TileMap *map, int x, int y, Tile *tile)
{
    if(map && tile)
    {
	map->data[y * map->bounds.w + x] = *tile;
    }
}

void tile_map_draw(TileMap *map, SDL_Renderer *r)
{
    if(!map) return;

    SDL_Rect src;
    for(int x = 0; x < map->bounds.w; x++)
    {
	for(int y = 0; y < map->bounds.h; y++)
	{
	    Tile *tile = tile_map_get_tile(map, x, y);
	    if(!tile) break;
	    src.x = tile->x;
	    src.y = tile->y;
	    src.w = map->bounds.w;
	    src.h = map->bounds.h;

	    SDL_Rect dst = {.x = x * map->bounds.w,
			    .y = y * map->bounds.h,
			    .w = map->bounds.w,
			    .h = map->bounds.h};
	    SDL_RenderCopy(r, map->texture, &src, &dst);
	}
    }
}

Tile *tile_map_get_tile(TileMap *map, int x, int y)
{
    if(map)
    {
	return &map->data[y * map->bounds.w + x];
    }
    return NULL;
}

void tile_map_destroy(TileMap *map)
{
    if(map)
    {
	free(map->data);
    }
    free(map);
}
