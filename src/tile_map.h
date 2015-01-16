#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

struct TileInfo;

TileMap *tile_map_create(Game *game);
void tile_map_set_tile(TileMap *map, int x, int y, int tile_index);
void tile_map_draw(TileMap *map, SDL_Renderer *r);
void tile_map_destroy(TileMap *map);
