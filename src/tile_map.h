#pragma once
#include <SDL2/SDL.h>
#include "sdltest.h"

TileMap *tile_map_create(int x, int y, int tile_width, int tile_height);
void tile_map_load(TileMap *map, const char *file);
Tile *tile_map_get_tile(TileMap *map, int x, int y);
void tile_map_draw(TileMap *map, SDL_Renderer *r);
void tile_map_destroy(TileMap *map);
