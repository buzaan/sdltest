#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bit_vector.h"
#include "path.h"
#include "tile_map.h"

static const size_t PATH_DEFAULT_CAPACITY = 50;
static const size_t QUEUE_DEFAULT_CAPACITY = 50;

/* PriorityQueue for ints implemented via min heap */
struct PriorityQueue
{
    struct HeapItem
    {
        int value;
        int priority;
    } *items;
    size_t size; // Number of items currently in queue
    size_t capacity; // Maximum number of items that could be in queue
};

static size_t pqueue_parent(size_t location)
{
    assert(location != 0);
    return location / 2;
}

static size_t pqueue_lchild(size_t location)
{
    return 2 * location + 1;
}

static size_t pqueue_rchild(size_t location)
{
    return 2 * location + 2;
}

static void pqueue_swap(struct PriorityQueue *q, size_t a, size_t b)
{
    struct HeapItem tmp = q->items[a];
    q->items[a] = q->items[b];
    q->items[b] = tmp;
}

static bool pqueue_valid_subtree(struct PriorityQueue *q, size_t location)
{
    assert(q);

    if(location > q->size) return true;

    size_t lchild = pqueue_lchild(location);
    size_t rchild = pqueue_rchild(location);
    int priority = q->items[location].priority;
    return (lchild < q->size ? priority <= q->items[lchild].priority : true)
        && (rchild < q->size ? priority <= q->items[rchild].priority : true)
        && pqueue_valid_subtree(q, lchild)
        && pqueue_valid_subtree(q, rchild);
}

static bool pqueue_valid(struct PriorityQueue *q)
{
    return pqueue_valid_subtree(q, 0);
}

static void pqueue_init_capacity(struct PriorityQueue *q, size_t capacity)
{
    assert(q);
    q->items = malloc(capacity * sizeof(struct HeapItem));
    q->size = 0;
    q->capacity = capacity;
}

static void pqueue_destroy(struct PriorityQueue *q)
{
    assert(q);
    free(q->items);
}

static void pqueue_realloc(struct PriorityQueue *q)
{
    assert(q);

    q->capacity *= 2;
    q->items = realloc(q->items, q->capacity * sizeof(struct HeapItem));

    assert(pqueue_valid(q));
}

static void pqueue_init(struct PriorityQueue *q)
{
    pqueue_init_capacity(q, QUEUE_DEFAULT_CAPACITY);
}

static void pqueue_heapify_up(struct PriorityQueue *q, size_t location)
{
    assert(q);
    if(location != 0)
    {
        size_t parent = pqueue_parent(location);
        if(q->items[parent].priority > q->items[location].priority)
        {
            pqueue_swap(q, parent, location);
            pqueue_heapify_up(q, parent);
        }
    }    
}

static void pqueue_insert(struct PriorityQueue *q, int value, int priority)
{
    assert(pqueue_valid(q));

    if(q->size == q->capacity)
    {
        pqueue_realloc(q);
    }
    struct HeapItem *item = &q->items[q->size];
    item->value = value;
    item->priority = priority;
    pqueue_heapify_up(q, q->size);
    q->size++;

    assert(pqueue_valid(q));
}

static bool pqueue_empty(struct PriorityQueue *q)
{
    return q->size == 0;
}

static void pqueue_heapify(struct PriorityQueue *q, size_t location)
{
    assert(q);
    
    int priority = q->items[location].priority;
    size_t lchild = pqueue_lchild(location);
    size_t new_min_loc = location;

    if(lchild < q->size && q->items[lchild].priority < priority)
    {
        new_min_loc = lchild;
    }

    size_t rchild = pqueue_rchild(location);
    if(rchild < q->size && q->items[rchild].priority < q->items[new_min_loc].priority)
    {
        new_min_loc = rchild;
    }

    if(new_min_loc != location)
    {
        pqueue_swap(q, new_min_loc, location);
        pqueue_heapify(q, new_min_loc);
    }
}

static int pqueue_extract(struct PriorityQueue *q)
{
    assert(q);
    assert(!pqueue_empty(q));

    int out = q->items[0].value;

    pqueue_swap(q, 0, q->size - 1);
    q->size--;
    pqueue_heapify(q, 0);

    assert(pqueue_valid(q));

    return out;
}

/* Queue of points backed by a circular buffer. Reallocated with
 * expanded capacity when maximum queue size is reached. */
struct PtQueue
{
    Point *data;
    size_t capacity;
    Point *start;
    Point *end;
};

// Verifies invariants of queue
static bool queue_valid(struct PtQueue *q)
{
    Point *data_start = &q->data[0];
    Point *data_end = &q->data[q->capacity];
    return (data_start <= q->start && q->start < data_end)
        && (data_start <= q->end && q->end < data_end);
}

static void queue_init_capacity(struct PtQueue *q, size_t size)
{
    assert(q);
    q->capacity = size;
    q->data = malloc(sizeof(Point) * q->capacity);
    q->start = &q->data[0];
    q->end = q->start + 1;
}

static void queue_init(struct PtQueue *q)
{
    assert(q);

    queue_init_capacity(q, QUEUE_DEFAULT_CAPACITY);

    assert(queue_valid(q));
}

static void queue_destroy(struct PtQueue *q)
{
    free(q->data);
}

static size_t queue_size(struct PtQueue *q)
{
    if(q->end <= q->start)
    {
        return q->end - &q->data[0] + &q->data[q->capacity] - q->start - 1;
    }
    else
    {
        return q->end - q->start - 1;
    }
}

// Returns the next location in the cyclic queue buffer
static Point *q_next(struct PtQueue *q, Point *i)
{
    return i == &q->data[q->capacity - 1] ? &q->data[0] : i + 1;
}

// " previous " 
static Point *q_prev(struct PtQueue *q, Point *i)
{
    return i == &q->data[0] ? &q->data[q->capacity - 1] : i - 1;
}

static bool queue_empty(struct PtQueue *q)
{
    return q_next(q, q->start) == q->end;
}

// Resizes the queue, copying old elements to the new space.
static void queue_realloc(struct PtQueue *q)
{
    size_t old_size = queue_size(q);
    q->capacity *= 2;
    Point *new = malloc(sizeof(Point) * q->capacity);
    for(Point *i = &new[0];
        q->start != q_prev(q, q->end);
        q->start = q_next(q, q->start), i++)
    {
        *i = *q->start;
    }
    free(q->data);

    q->data = new;
    q->start = &new[0];
    q->end = q->start + old_size + 1;
}

static void enqueue(struct PtQueue *q, const Point *p)
{
    assert(queue_valid(q));

    if(q->end == q->start)
    {
        queue_realloc(q);
    }
    *q_prev(q, q->end) = *p;
    q->end = q_next(q, q->end);

    assert(queue_valid(q));
}

static void dequeue(struct PtQueue *q, Point *out)
{
    assert(queue_valid(q));
    assert(queue_size(q) > 0);

    if(out)
    {
        *out = *q->start;
    }
    
    if(!queue_empty(q))
    {
        q->start = q_next(q, q->start);
    }

    assert(queue_valid(q));
}

static void queue_reset(struct PtQueue *q)
{
    assert(q);
    q->start = &q->data[0];
    q->end = q->start + 1;
}

void path_init(struct Path *p)
{
    p->points = malloc(sizeof(p->points[0]) * PATH_DEFAULT_CAPACITY);
    p->capacity = PATH_DEFAULT_CAPACITY;
    p->size = 0;
}

static void path_push_point(struct Path *p, const struct Point *pt)
{
    if(p->size == p->capacity)
    {
        p->capacity *= 2;
        p->points = realloc(p->points, sizeof(struct Point) * p->capacity);
    }
    p->points[p->size] = *pt;
    p->size++;
}

void path_from_to_astar(Path *out, const TileMap *map,
                        const Point *from,
                        const Point *to)
{
}

void path_from_to_bfs(Path *out, const TileMap *map,
                      const Point *from,
                      const Point *to)
{
    
}

void path_from_to(Path *out, const TileMap *map, 
                  const Point *from, 
                  const Point *to)
{
    assert(out);
    assert(map);
    assert(from);
    assert(to);
    path_from_to_bfs(out, map, from, to);
}

void debug_map_bfs(TileMap *map, const Point *start)
{
    assert(map);
    assert(start);

    const static Point directions[4] = {
        {.x = 0, .y = 1}, 
        {.x = 0, .y = -1},
        {.x = 1, .y = 0},
        {.x = -1, .y = 0}};
    const static size_t num_dirs = sizeof(directions) / sizeof(directions[0]);

    struct PtQueue queue;
    struct BitVec seen;
    Point goal = {.x = 25, .y = 30};
    unsigned int tile_ord;
    int w;
    int h;
    tile_map_dimensions(map, &w, &h);    
    TileInfo visited_tile = {.type = TT_STONE,
                             .hit_points = 100,
                             .glyph = '0'};
    int *parent = calloc(w * h, sizeof(int));

    bv_init(&seen, w * h);
    queue_init(&queue);
    enqueue(&queue, start);

    tile_ord = start->x + start->y * w;
    bv_set(&seen, tile_ord, true);
    parent[tile_ord] = -1;
    
    while(!queue_empty(&queue))
    {
        Point current;
        dequeue(&queue, &current);
        unsigned int current_ord = current.x + current.y * w;
        
        if(current.x == goal.x && current.y == goal.y)
        {
            TileInfo path_tile = {.type = TT_STONE, .glyph = '1'};
            tile_map_set_tile(map, current.x, current.y, &path_tile);
            int *p = &parent[current_ord];
            while(*p != -1)
            {
                int x = *p % w;
                int y = *p / w;
                tile_map_set_tile(map, x, y, &path_tile);
                p = &parent[*p];
            }
            break;
        }

        for(int dir = 0; dir < num_dirs; dir++)
        {
            Point neighbor = current;
            neighbor.x += directions[dir].x;
            neighbor.y += directions[dir].y;
            tile_ord = neighbor.x + neighbor.y * w;

            // Relies on tile map returning NULL for out-of-bounds tiles.
            TileInfo *tile = tile_map_get_tile(map, neighbor.x, neighbor.y);
            if(tile && tile->type == TT_EMPTY && !bv_get(&seen, tile_ord))
            {
                enqueue(&queue, &neighbor);
                bv_set(&seen, tile_ord, true);
                parent[tile_ord] = current_ord;
            }
        }        
        tile_map_set_tile(map, current.x, current.y, &visited_tile);
    }

    queue_destroy(&queue);
    bv_destroy(&seen);
    free(parent);
}
             

void path_destroy(struct Path *p)
{
    free(p->points);
}


