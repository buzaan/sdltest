#include "sprite_sheet.h"

struct SpriteSheet
{
    SDL_Texture *sprites;
    unsigned int sheet_width;
    unsigned int sheet_height;
    SDL_Rect src_rect;
    unsigned int tile_width;
    unsigned int tile_height;
};

struct SpriteSheet*
sprite_sheet_create(SDL_Texture *texture,
                    unsigned int sheet_width,
                    unsigned int sheet_height,
                    unsigned int tile_width,
                    unsigned int tile_height)
{
    struct SpriteSheet *ss = malloc(sizeof(struct SpriteSheet));
    ss->sprites = texture;
    ss->sheet_width = sheet_width;
    ss->sheet_height = sheet_height;
    ss->src_rect.w = tile_width;
    ss->src_rect.h = tile_height;
    return ss;
}

void sprite_sheet_destroy(struct SpriteSheet *s)
{
    free(s);
}

void sprite_sheet_draw(struct SpriteSheet *ss, 
                       SDL_Renderer *r,
                       unsigned int x, 
                       unsigned int y, 
                       char glyph)
{
    unsigned int row = glyph * ss->tile_width;
    unsigned int col = glyph * ss->tile_width;
    ss->src_rect.x = row * ss->tile_width;
    ss->src_rect.y = col * ss->tile_height;
    
    SDL_Rect dest;
    dest.x = x * ss->tile_width;
    dest.y = y * ss->tile_height;
    dest.w = ss->tile_width;
    dest.h = ss->tile_height;

    SDL_RenderCopy(r, ss->sprites, &ss->src_rect, &dest);
}
