#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "scene.h"
#include "tile_map.h"
#include "gameplay_scene.h"
#include "graphics.h"

struct Data
{
    TileMap *map;
};
typedef struct Data Data;

static int cell_rule(TileMap *map, int x, int y)
{
    // loops over 3x3 grid surrounding <x,y> cell, excluding cell itself
    int neighbors = 0;
    for(int cx = x - 1; cx < x + 1; cx++)
    {
        for(int cy = y - 1; cy < y + 1; cy++)
        {
            if(!(cx == x && cy == y) && 
               tile_map_get_tile(map, cx, cy) != TT_EMPTY)
            {
                neighbors++;
            }
        }
    }

    return neighbors >= 3 ? 1 : 0;
}

void gameplay_scene_start(Scene *s)
{
    fputs("Starting gameplay scene.\n", stderr);
    Data *data = malloc(sizeof(Data));
    scene_set_data(s, data);
    data->map = tile_map_create(scene_get_game(s));
    
    TileMapCAParams params;
    params.rule = cell_rule;
    params.generations = 3;
    params.seed_ratio = 0.2;
    tile_map_gen_map(data->map, &params);
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
