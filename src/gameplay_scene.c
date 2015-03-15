#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "scene.h"
#include "tile_map.h"
#include "gameplay_scene.h"
#include "graphics.h"
#include "path.h"

struct Data
{
    TileMap *map;
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
    data->map = tile_map_create(scene_get_game(s));
    
    TileMapCAParams params;
    params.rule = cell_rule;
    params.generations = 2;
    params.seed_ratio = 0.2;
    tile_map_gen_map(data->map, &params);

    Point start = {.x = 5, .y = 5};
    debug_map_bfs(data->map, &start);
}

void gameplay_scene_update(Scene *s, int dt, const InputState *input)
{

}

void gameplay_scene_draw(Scene *s, SDL_Renderer *renderer)
{
    Data *data = scene_get_data(s);
    SDL_RenderClear(renderer);
    tile_map_draw(data->map, renderer);
}

void gameplay_scene_stop(Scene *s)
{
    fputs("Ending gameplay scene.\n", stderr);
    Data *data = scene_get_data(s);
    tile_map_destroy(data->map);
    free(data);
}
