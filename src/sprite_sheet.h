#pragma once
#include <SDL2/SDL.h>

struct SpriteSheet;

struct SpriteSheet*
sprite_sheet_create(SDL_Texture *texture,
                    unsigned int sheet_width,
                    unsigned int sheet_height,
                    unsigned int tile_width,
                    unsigned int tile_height);
void sprite_sheet_destroy(struct SpriteSheet *ss);
void sprite_sheet_draw(struct SpriteSheet *ss, 
                       SDL_Renderer *r, 
                       unsigned int x, 
                       unsigned int y, 
                       char glyph);
