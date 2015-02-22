#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "game.h"
#include "tile_map.h"

static const int INFO_BAR_HEIGHT = 64; 
static const int TILE_WIDTH = 16;
static const int TILE_HEIGHT = 16;

struct dim
{
    int w;
    int h;
};

struct TileMap
{
    struct dim bounds; // in pixels
    struct dim tiles; // in tiles 
    TileInfo *data; // tile data
    size_t data_size;
    SDL_Texture *sprites;
};

// Checks if val is in the half-open interval [min, max)
static bool bounds(int min, int val, int max)
{
    return min <= val && val < max;
}

TileMap *tile_map_create(Game *game)
{
    if(!game) return NULL;

    TileMap *map = malloc(sizeof(TileMap));
    map->bounds.w = WINDOW_WIDTH;
    map->bounds.h = WINDOW_HEIGHT - INFO_BAR_HEIGHT;
    map->tiles.w = map->bounds.w / TILE_WIDTH;
    map->tiles.h = map->bounds.h / TILE_HEIGHT;

    map->data_size = map->tiles.w * map->tiles.h * sizeof(TileInfo);
    map->data = malloc(map->data_size);
    memset(map->data, 0, map->data_size);

    SDL_Surface *bmp = SDL_LoadBMP("resources/dostiles.bmp");
    if(!bmp)
    {
        fprintf(stderr, "LoadBMP: %s\n", SDL_GetError());
        tile_map_destroy(map);
        return NULL;
    }

    SDL_Renderer *renderer = game_get_renderer(game);
    map->sprites = SDL_CreateTextureFromSurface(renderer, bmp);
    if(!map->sprites)
    {
        fprintf(stderr, "CreateTextureFromSurface: %s\n", SDL_GetError());
    }
    SDL_FreeSurface(bmp);
    return map;
}

static void seed_map(TileMap *map, TileMapCAParams *params)
{
    long p = params->seed_ratio * RAND_MAX;
    
    // Seed with random tiles
    for(int x = 0; x < map->tiles.w; x++)
    {
        for(int y = 0; y < map->tiles.h; y++)
        {
            TileInfo tile;
            tile.type = random() <= p ? TT_EMPTY : TT_STONE;
            tile.hit_points = 100;
            tile_map_set_tile(map, x, y, &tile);
        }
    }
}

static void ca_generation(TileMap *map, TileInfo *buf, TileMapCAParams *params)
{
    if(!params->rule) return;

    for(int x = 1; x < map->tiles.w - 1; x++)
    {
        for(int y = 1; y < map->tiles.h - 1; y++)
        {
            TileInfo *tile = &buf[x + y * map->tiles.w];
            params->rule(map, x, y, tile);
        }
    }
    memcpy(map->data, buf, map->data_size);
}

/** Generates a map using a simple cellular automaton.  Gives a
 * cave-like appearance. Map is seeded with random stone/empty tiles
 * then the CA rules are run for a few generations based on params. 
 * 
 * Map is basically double-buffered, which is why a copy of the tile
 * data is made.
 */
void tile_map_gen_map(TileMap *map, TileMapCAParams *params)
{
    if(!map || !params) return;
    seed_map(map, params);

    TileInfo *buf = malloc(map->data_size);
    memcpy(buf, map->data, map->data_size);
    for(int i = 0; i < params->generations; i++)
    {
        ca_generation(map, buf, params);
    }
    free(buf);
}

void tile_map_destroy(TileMap *map)
{
    if(map)
    {
        free(map->data);
        SDL_DestroyTexture(map->sprites);
        free(map);
   }
}

TileInfo *tile_map_get_tile(TileMap *map, int x, int y)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
        return &map->data[y * map->tiles.w + x];
    }
    fprintf(stderr, "Bad tile %d %d\n", x, y);
    return NULL;
}

void tile_map_set_tile(TileMap *map, int x, int y, TileInfo *tile)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
        *tile_map_get_tile(map, x, y) = *tile;
    }
}

/** Fills SDL_Rect `out` with x,y positions in sprite sheet */
static void ss_pos(SDL_Rect *out, int x, int y)
{
    assert(out);
    out->x = x * TILE_WIDTH;
    out->y = y * TILE_HEIGHT;
    out->w = TILE_WIDTH;
    out->h = TILE_HEIGHT;
}

/* TODO: Will need to change later since a single tile type will need
 * to have multiple glyphs (e.g. a wall with straight and corner
 * sections. */
static SDL_Rect *tile_rect(const TileInfo *tile)
{
    static SDL_Rect table[TT_NUM_TYPES];
    static bool init = false;

    if(!init)
    {
        ss_pos(&table[TT_EMPTY], 0, 0);
        ss_pos(&table[TT_STONE], 2, 11);
        ss_pos(&table[TT_WALL],  0, 11);
        init = true;
    }

    assert(tile);
    assert(tile->type < TT_NUM_TYPES);
    return &table[tile->type];
}

void tile_map_draw(TileMap *map, SDL_Renderer *r)
{
    if(!map) return;

    SDL_Rect dst = {.w = TILE_WIDTH, .h = TILE_HEIGHT};

    for(int x = 0; x < map->tiles.w; x++)
    {
        for(int y = 0; y < map->tiles.h; y++)
        {
            TileInfo *tile = tile_map_get_tile(map, x, y);
            assert(tile);
            
            dst.x = x * TILE_WIDTH;
            dst.y = y * TILE_WIDTH;
            SDL_RenderCopy(r, map->sprites, tile_rect(tile), &dst);
        }
    }
}
