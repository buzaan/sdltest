#pragma once
#include <SDL2/SDL.h>

struct SpriteSheet;

struct SpriteSheet*
sprite_sheet_create(SDL_Texture *texture,
                    unsigned int sheet_width,  // Width of sheet in px
                    unsigned int sheet_height, // "
                    unsigned int tile_width,   // Width of single tile in px
                    unsigned int tile_height); // " 
void sprite_sheet_destroy(struct SpriteSheet *ss);
void sprite_sheet_draw(struct SpriteSheet *ss, 
                       SDL_Renderer *r, 
                       unsigned int x, 
                       unsigned int y, 
                       uint8_t glyph);
