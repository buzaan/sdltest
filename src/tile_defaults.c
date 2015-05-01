#include "tile_map.h"
#include "tile_defaults.h"

const TileInfo TDEF_EMPTY = {
    .type = TT_EMPTY,
    .hit_points = 0,
    .glyph = '\0'};
const TileInfo TDEF_STONE = {
    .type = TT_STONE,
    .hit_points = 100,
    .glyph = TILE_AT(2, 11)};
const TileInfo TDEF_WALL = {
    .type = TT_WALL,
    .hit_points = 50,
    .glyph = TILE_AT(10, 11)};
