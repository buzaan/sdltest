#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

struct TileInfo;

struct TileMapCAParams
{
    int (*rule)(TileMap *map, int x, int y);
    float seed_ratio;
    unsigned int generations;
};
typedef struct TileMapCAParams TileMapCAParams;

TileMap *tile_map_create(Game *game);
void tile_map_set_tile(TileMap *map, int x, int y, int tile_index);
int tile_map_get_tile(TileMap *map, int x, int y);
void tile_map_gen_map(TileMap *map, TileMapCAParams *params);
void tile_map_draw(TileMap *map, SDL_Renderer *r);
void tile_map_destroy(TileMap *map);
