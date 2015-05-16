#pragma once
#include <SDL2/SDL.h>

// Provides tile-based drawing.

struct Renderer;

struct Renderer*
renderer_create(SDL_Texture *texture,
                SDL_Renderer *renderer,
                unsigned int sheet_width,  // Width of sheet in px
                unsigned int sheet_height, // "
                unsigned int tile_width,   // Width of single tile in px
                unsigned int tile_height); // " 
void renderer_destroy(struct Renderer *r);

// Draws glyph at (x,y) tile location.
void renderer_draw(struct Renderer *r, 
                   unsigned int x, 
                   unsigned int y, 
                   uint8_t glyph);
