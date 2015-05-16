#pragma once
#include <SDL2/SDL.h>
#include <inttypes.h>
#include <stdbool.h>
#include "sdltest.h"
#include "sprite_sheet.h"

// TODO: fix
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

typedef unsigned int TileID;

TileMap *tile_map_create(Game *game, struct SpriteSheet *sprites);
void tile_map_set_tile(TileMap *map, int x, int y, TileInfo *tile);
TileInfo *tile_map_get_tile(const TileMap *map, int x, int y);
void tile_map_gen_map(TileMap *map, TileMapCAParams *params);
void tile_map_draw(TileMap *map, SDL_Renderer *r);
void tile_map_destroy(TileMap *map);
void tile_switch_type(TileInfo *tile, enum TileType new_type);
void tile_map_dimensions(const TileMap *map, int *out_x, int *out_y);
unsigned int tile_map_tile_id(const TileMap *map, const struct Point *pt);
void tile_map_coord(const TileMap *map, TileID tid, struct Point *out);
unsigned int tile_map_max_id(const TileMap *map);

/* Returns number of neighbors found.
   out_neighbors: filled with TileID of neighboring tiles
   count: capacity (int number of TileIDs) of out_neighbors
*/
unsigned int tile_map_neighbors(const TileMap *map, TileID tid, TileID *out_neighbors, size_t count);
