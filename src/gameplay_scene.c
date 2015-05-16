#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "scene.h"
#include "tile_map.h"
#include "game.h"
#include "gameplay_scene.h"
#include "renderer.h"
#include "path.h"

static const int INFO_BAR_HEIGHT = 64; // UI info bar height in px
static const int TILE_WIDTH = 16;
static const int TILE_HEIGHT = 16;

struct Data
{
    TileMap *map;
    struct Renderer *tile_renderer;
};
typedef struct Data Data;

/* Rule used for generating CA-based random map */
static void cell_rule(TileMap *map, int x, int y, TileInfo *tile)
{
    // loops over 3x3 grid surrounding <x,y> cell, excluding cell itself
    int neighbors = 0;
    for(int cx = x - 1; cx < x + 1; cx++)
    {
        for(int cy = y - 1; cy < y + 1; cy++)
        {
            if(!(cx == x && cy == y))
            {
                TileInfo *tile = tile_map_get_tile(map, cx, cy);
                assert(tile);
                if(tile->type != TT_EMPTY)
                {
                    neighbors++;
                }
            }
        }
    }
    tile_switch_type(tile, neighbors >= 3 ? TT_STONE : TT_EMPTY);
}

void gameplay_scene_start(Scene *s)
{
    fputs("Starting gameplay scene.\n", stderr);

    Data *data = malloc(sizeof(Data));
    scene_set_data(s, data);

    Game *game = scene_get_game(s);
    SDL_Texture *tiles = game_load_texture(game, "resources/dostiles.bmp");
    SDL_Renderer *renderer = game_get_renderer(game);

    data->tile_renderer = renderer_create(tiles, renderer, 256, 256, 16, 16);
    data->map = tile_map_create(
        WINDOW_WIDTH / TILE_WIDTH,
        (WINDOW_HEIGHT - INFO_BAR_HEIGHT) / TILE_HEIGHT);

    TileMapCAParams params;
    params.rule = cell_rule;
    params.generations = 2;
    params.seed_ratio = 0.2;
    tile_map_gen_map(data->map, &params);
}

void gameplay_scene_stop(Scene *s)
{
    fputs("Ending gameplay scene.\n", stderr);

    Data *data = scene_get_data(s);
    tile_map_destroy(data->map);
    renderer_destroy(data->tile_renderer);
    free(data);
}

void gameplay_scene_update(Scene *s, int dt, const InputState *input)
{
    Data *data = scene_get_data(s);
    assert(data);
    assert(input);
    if(input->select)
    {
        struct Point start = {.x = 25, .y = 5};
        struct Point end = {.x = 12, .y = 23};
        struct Path path;
        TileInfo path_tile = {.type = TT_STONE, .glyph = '.'};
        path_init(&path);
        path_from_to(&path, data->map, &start, &end);

        for(int i = 0; i < path.size; i++)
        {
            struct Point *pt = &path.points[i];
            tile_map_set_tile(data->map, pt->x, pt->y, &path_tile);
        }

        path_destroy(&path);
    }
    
    if(input->cursor.active)
    {
        fprintf(stderr, "Click at %d %d\n", input->cursor.x, input->cursor.y);
    }
}

void gameplay_scene_draw(Scene *s, SDL_Renderer *renderer)
{
    Data *data = scene_get_data(s);
    SDL_RenderClear(renderer);
    tile_map_draw(data->map, data->tile_renderer);
}
