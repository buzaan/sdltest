#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "game.h"
#include "tile_map.h"

static const int INFO_BAR_HEIGHT = 64;
static const int TILE_WIDTH = 16;
static const int TILE_HEIGHT = 16;

struct dim
{
    int w;
    int h;
};

struct TileInfo
{
    bool obstacle;
    SDL_Rect rect;
};

struct TileMap
{
    struct dim bounds; // in pixels
    struct dim tiles; // in tiles 
    int *data; // tile data
    struct TileInfo info[2]; // TODO: will need more than 2 tile types...
    SDL_Texture *sprites;
};

// Checks if val is in the half-open interval [min, max)
static bool bounds(int min, int val, int max)
{
    return min <= val && val < max;
}

TileMap *tile_map_create(Game *game)
{
    if(!game) return NULL;

    TileMap *map = malloc(sizeof(TileMap));
    map->bounds.w = WINDOW_WIDTH;
    map->bounds.h = WINDOW_HEIGHT - INFO_BAR_HEIGHT;
    map->tiles.w = map->bounds.w / TILE_WIDTH;
    map->tiles.h = map->bounds.h / TILE_HEIGHT;
    map->data = malloc(sizeof(int) * map->tiles.w * map->tiles.h);
    for(int i = 0; i < map->tiles.w * map->tiles.h; i++)
    {
	map->data[i] = i % 2; // Create stipple pattern for testing
    }

    // TODO: tile props configurable from file
    SDL_Rect r = {.w = TILE_WIDTH, .h = TILE_HEIGHT, .x = 32, .y = 0};
    map->info[0].obstacle = false;
    map->info[0].rect = r;

    // Tile in dostiles.bmp is at <1,11>
    r.x = TILE_WIDTH * 1;
    r.y = TILE_HEIGHT * 11;
    map->info[1].obstacle = true;
    map->info[1].rect = r;

    SDL_Surface *bmp = SDL_LoadBMP("resources/dostiles.bmp");
    if(!bmp)
    {
	fprintf(stderr, "LoadBMP: %s\n", SDL_GetError());
	tile_map_destroy(map);
	return NULL;
    }

    SDL_Renderer *renderer = game_get_renderer(game);
    map->sprites = SDL_CreateTextureFromSurface(renderer, bmp);
    if(!map->sprites)
    {
	fprintf(stderr, "CreateTextureFromSurface: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(bmp);
    return map;
}

void tile_map_destroy(TileMap *map)
{
    if(map)
    {
	free(map->data);
	SDL_DestroyTexture(map->sprites);
	free(map);
   }
}

void tile_map_set_tile(TileMap *map, int x, int y, int tile_index)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
	map->data[y * map->bounds.w + x] = tile_index;
    }
}

void tile_map_draw(TileMap *map, SDL_Renderer *r)
{
    if(!map) return;

    SDL_Rect dst = {.w = TILE_WIDTH, .h = TILE_HEIGHT};
    for(int x = 0; x < map->tiles.w; x++)
    {
	for(int y = 0; y < map->tiles.h; y++)
	{
	    int tileidx = map->data[x + y * map->tiles.w];
	    assert(0 <= tileidx && tileidx < 2);
	    struct TileInfo *info = &map->info[tileidx];
	    dst.x = x * TILE_WIDTH;
	    dst.y = y * TILE_WIDTH;
	    SDL_RenderCopy(r, map->sprites, &info->rect, &dst);
	}
    }
}
