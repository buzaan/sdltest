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

struct TileInfo
{
    bool obstacle;
    SDL_Rect rect; // Bounds within sprite sheet
};

struct TileMap
{
    struct dim bounds; // in pixels
    struct dim tiles; // in tiles 
    int *data; // tile data
    size_t data_size;
    struct TileInfo info[2]; // TODO: will need more than 2 tile types...
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
    map->data_size = map->tiles.w * map->tiles.h * sizeof(int);
    map->data = malloc(map->data_size);
    memset(map->data, 0, map->data_size);

    // TODO: tile props configurable from file
    SDL_Rect r = {.w = TILE_WIDTH, .h = TILE_HEIGHT, .x = 32, .y = 0};
    map->info[0].obstacle = false;
    map->info[0].rect = r;

    // Tile in dostiles.bmp is at <1,11>
    r.x = TILE_WIDTH * 1;
    r.y = TILE_HEIGHT * 11;
    map->info[1].obstacle = true;
    map->info[1].rect = r;

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
            tile_map_set_tile(map, x, y, random() <= p ? 0 : 1);
        }
    }
}

static void ca_generation(TileMap *map, int *buf, TileMapCAParams *params)
{
    if(!params->rule) return;

    for(int x = 1; x < map->tiles.w - 1; x++)
    {
        for(int y = 1; y < map->tiles.h - 1; y++)
        {
            buf[x + y * map->tiles.w] = params->rule(map, x, y);
        }
    }
    memcpy(map->data, buf, map->data_size);
}

void tile_map_gen_map(TileMap *map, TileMapCAParams *params)
{
    if(!map || !params) return;
    seed_map(map, params);

    int *buf = malloc(map->data_size);
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

int tile_map_get_tile(TileMap *map, int x, int y)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
        return map->data[y * map->tiles.w + x];
    }
    fprintf(stderr, "Bad tile %d %d\n", x, y);
    return -1;
}

void tile_map_set_tile(TileMap *map, int x, int y, int tile_index)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
        map->data[y * map->tiles.w + x] = tile_index;
    }
}

void tile_map_draw(TileMap *map, SDL_Renderer *r)
{
    if(!map) return;

    SDL_Rect dst = {.w = TILE_WIDTH, .h = TILE_HEIGHT};
    for(int x = 0; x < map->tiles.w; x++)
    {
        for(int y = 0; y < map->tiles.h; y++)
        {
            int tileidx = map->data[x + y * map->tiles.w];
            if(!(0 <= tileidx && tileidx < 2))
            {
                fprintf(stderr, "%d\n", tileidx);
            }
            assert(0 <= tileidx && tileidx < 2);
            struct TileInfo *info = &map->info[tileidx];
            dst.x = x * TILE_WIDTH;
            dst.y = y * TILE_WIDTH;
            SDL_RenderCopy(r, map->sprites, &info->rect, &dst);
        }
    }
}
