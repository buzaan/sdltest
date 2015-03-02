#pragma once
#include <SDL2/SDL.h>
#include <inttypes.h>
#include "sdltest.h"

// *sound of Marge Simpson going 'hmrrrrm' *
#define FONT_SHEET_CELL_STRIDE 16
#define TILE_AT(x, y) ((x) + ((y) * FONT_SHEET_CELL_STRIDE))

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
    uint8_t glyph;
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
void tile_switch_type(TileInfo *tile, enum TileType new_type);
