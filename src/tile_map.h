#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

TileMap *tile_map_alloc();
TileMap *tile_map_init_from_file(TileMap *map, const char *file);
Tile *tile_map_get_tile(TileMap *map, int x, int y);
void tile_map_draw(TileMap *map, SDL_Renderer *r);
void tile_map_destroy(TileMap *map);
