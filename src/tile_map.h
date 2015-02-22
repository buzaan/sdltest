#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

enum TileType
{
    TT_EMPTY,
    TT_STONE,
    TT_WALL,
    TT_NUM_TYPES,
};

struct TileInfo
{
    enum TileType type;
    int hit_points;
};
typedef struct TileInfo TileInfo;

struct TileMapCAParams
{
    void (*rule)(TileMap *map, int x, int y, TileInfo *result);
    float seed_ratio;
    unsigned int generations;
};
typedef struct TileMapCAParams TileMapCAParams;

TileMap *tile_map_create(Game *game);
void tile_map_set_tile(TileMap *map, int x, int y, TileInfo *tile);
TileInfo *tile_map_get_tile(TileMap *map, int x, int y);
void tile_map_gen_map(TileMap *map, TileMapCAParams *params);
void tile_map_draw(TileMap *map, SDL_Renderer *r);
void tile_map_destroy(TileMap *map);
