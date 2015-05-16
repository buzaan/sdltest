#include "renderer.h"

struct Renderer
{
    SDL_Texture *sprites;
    SDL_Renderer *renderer;
    unsigned int sheet_width;
    unsigned int sheet_height;
    SDL_Rect src_rect;
    unsigned int tile_width;
    unsigned int tile_height;
    unsigned int stride;
};

struct Renderer*
renderer_create(SDL_Texture *texture,
                SDL_Renderer *renderer,
                unsigned int sheet_width,
                unsigned int sheet_height,
                unsigned int tile_width,
                unsigned int tile_height)
{
    struct Renderer *out = malloc(sizeof(struct Renderer));
    out->sprites = texture;
    out->renderer = renderer;
    out->sheet_width = sheet_width;
    out->sheet_height = sheet_height;
    out->src_rect.w = tile_width;
    out->src_rect.h = tile_height;
    out->stride = sheet_width / tile_width;
    out->tile_width = tile_width;
    out->tile_height = tile_height;
    return out;
}

void renderer_destroy(struct Renderer *r)
{
    free(r);
}

void renderer_draw(struct Renderer *r, 
                   unsigned int x, 
                   unsigned int y, 
                   uint8_t glyph)
{
    unsigned int row = glyph / r->stride;
    unsigned int col = glyph % r->stride;
    r->src_rect.x = col * r->tile_height;
    r->src_rect.y = row * r->tile_width;
    
    SDL_Rect dest;
    dest.x = x * r->tile_width;
    dest.y = y * r->tile_height;
    dest.w = r->tile_width;
    dest.h = r->tile_height;
    SDL_RenderCopy(r->renderer, r->sprites, &r->src_rect, &dest);
}
