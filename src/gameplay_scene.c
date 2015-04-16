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
#include "worker.h"

struct Data
{
    TileMap *map;
    struct Worker *worker;
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
    struct Point worker_loc = {.x = 25, .y = 25};
    data->worker = worker_create(data->map, &worker_loc);
    
    TileMapCAParams params;
    params.rule = cell_rule;
    params.generations = 2;
    params.seed_ratio = 0.2;
    tile_map_gen_map(data->map, &params);
}

void gameplay_scene_update(Scene *s, int dt, const InputState *input)
{
    Data *data = scene_get_data(s);
    if(input->cursor.active)
    {
        struct Point dest;
        struct Point pos = {.x = input->cursor.x, .y = input->cursor.y};
        tile_map_tile_at(data->map, &pos, &dest);
        
        struct Path path;
        path_init(&path);
        path_from_to(&path, data->map, worker_get_location(data->worker), &dest);
        if(path.size > 0)
        {
            worker_set_path(data->worker, &path);
        }
    }
    worker_update(data->worker, dt);

#if 0
    if(input->select)
    {
        struct Point start = {.x = 25, .y = 5};
        struct Point end = {.x = 12, .y = 23};
        struct Path path;
        TileInfo path_tile = {.type = TT_STONE, .glyph = '.'};
        Data *data = scene_get_data(s);
        path_init(&path);
        path_from_to(&path, data->map, &start, &end);

        for(int i = 0; i < path.size; i++)
        {
            struct Point *pt = &path.points[i];
            tile_map_set_tile(data->map, pt->x, pt->y, &path_tile);
        }

        path_destroy(&path);
    }
#endif
}

void gameplay_scene_draw(Scene *s, SDL_Renderer *renderer)
{
    Data *data = scene_get_data(s);
    SDL_RenderClear(renderer);
    tile_map_draw(data->map, renderer);
    worker_draw(data->worker, renderer);
}

void gameplay_scene_stop(Scene *s)
{
    fputs("Ending gameplay scene.\n", stderr);
    Data *data = scene_get_data(s);
    tile_map_destroy(data->map);
    worker_destroy(data->worker);
    free(data);
}
