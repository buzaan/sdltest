#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "sdltest.h"
#include "game.h"
#include "tile_map.h"
#include "renderer.h"

static const struct Point DIRECTIONS[4] = {
    {.x = 0, .y = 1}, 
    {.x = 0, .y = -1},
    {.x = 1, .y = 0},
    {.x = -1, .y = 0}};
static const size_t NUM_DIRECTIONS = sizeof(DIRECTIONS) / sizeof(DIRECTIONS[0]);

struct dim
{
    int w;
    int h;
};

struct TileMap
{
    struct dim tiles; // in tiles 
    TileInfo *data; // tile data
    size_t data_size;
};

// Checks if val is in the half-open interval [min, max)
static bool bounds(int min, int val, int max)
{
    return min <= val && val < max;
}

TileMap *tile_map_create(unsigned int x_tiles, unsigned int y_tiles)
{
    TileMap *map = malloc(sizeof(*map));
    map->tiles.w = x_tiles;
    map->tiles.h = y_tiles;

    map->data_size = map->tiles.w * map->tiles.h * sizeof(*map->data);
    map->data = malloc(map->data_size);
    memset(map->data, 0, map->data_size);

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
            tile_switch_type(&tile, random() <= p ? TT_EMPTY : TT_STONE);
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
        free(map);
   }
}

TileInfo *tile_map_get_tile(const TileMap *map, int x, int y)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
        return &map->data[y * map->tiles.w + x];
    }
    return NULL;
}

TileInfo *tile_map_get_tilei(const TileMap *map, TileID id)
{
    assert(0 < id && id < tile_map_max_id(map));
    int x = id % map->tiles.w;
    int y = id / map->tiles.w;
    return tile_map_get_tile(map, x, y);
}

void tile_map_set_tile(TileMap *map, int x, int y, TileInfo *tile)
{
    if(map && bounds(0, x, map->tiles.w) && bounds(0, y, map->tiles.h))
    {
        *tile_map_get_tile(map, x, y) = *tile;
    }
}

void tile_map_draw(TileMap *map, struct Renderer *r)
{
    if(!map) return;
    for(int x = 0; x < map->tiles.w; x++)
    {
        for(int y = 0; y < map->tiles.h; y++)
        {
            TileInfo *tile = tile_map_get_tile(map, x, y);
            assert(tile);
            renderer_draw(r, x, y, tile->glyph);
        }
    }
}

void tile_switch_type(TileInfo *tile, enum TileType new_type)
{
    assert(tile);
    tile->type = new_type;

    //TODO: Tile will change glyph later based on state and relation
    //to other tiles. This'll have to do for now.
    switch(tile->type)
    {
    case TT_EMPTY:
        tile->glyph = 0;
        break;
    case TT_STONE:
        tile->glyph = TILE_AT(2, 11);
        break;
    case TT_WALL:
        tile->glyph = TILE_AT(10, 11);
        break;
    default:
        tile->glyph = 1;
    }
}

void tile_map_dimensions(const TileMap *map, int *out_x, int *out_y)
{
    assert(map);
    if(out_x)
    {
        *out_x = map->tiles.w;
    }

    if(out_y)
    {
        *out_y = map->tiles.h;
    }
}

unsigned int tile_map_tile_id(const TileMap *map, const struct Point *pt)
{
    assert(map); assert(pt);
    return pt->x + pt->y * map->tiles.w;
}

void tile_map_coord(const TileMap *map, TileID tid, struct Point *pt_out)
{
    assert(map); assert(pt_out);
    pt_out->x = tid % map->tiles.w;
    pt_out->y = tid / map->tiles.w;
}

unsigned int tile_map_max_id(const TileMap *map)
{
    return map->tiles.w * map->tiles.h;
}

unsigned int tile_map_neighbors(const TileMap *map, TileID tid,
                                TileID *out_neighbors, size_t count)
{
    assert(map);
    assert(tid < tile_map_max_id(map));
    assert(out_neighbors);

    unsigned int num_neighbors = 0;
    TileID *insert = &out_neighbors[0];
    struct Point p;
    tile_map_coord(map, tid, &p);
    for(int i = 0; i < NUM_DIRECTIONS; i++)
    {
        struct Point neighbor = p;
        neighbor.x += DIRECTIONS[i].x;
        neighbor.y += DIRECTIONS[i].y;
        TileInfo *info = tile_map_get_tile(map, neighbor.x, neighbor.y);
        if(info && info->type == TT_EMPTY) // TODO - don't hard-code type.
        {
            num_neighbors++;
            if(insert < &out_neighbors[count])
            {
                *insert = tile_map_tile_id(map, &neighbor);
                insert++;
            }
        }
    }
    return num_neighbors;
}
