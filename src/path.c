#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "bit_vector.h"
#include "path.h"
#include "tile_map.h"

static const size_t PATH_DEFAULT_CAPACITY = 50;
static const size_t QUEUE_DEFAULT_CAPACITY = 50;

/* PriorityQueue for TileIDs implemented via min heap */
struct PriorityQueue
{
    struct HeapItem
    {
        TileID value;
        int priority;
    } *items;
    size_t size; // Number of items currently in queue
    size_t capacity; // Maximum number of items that could be in queue
};

static size_t pqueue_parent(size_t location)
{
    assert(location != 0);
    return (location - 1) / 2;
}

static size_t pqueue_lchild(size_t location)
{
    return 2 * location + 1;
}

static size_t pqueue_rchild(size_t location)
{
    return 2 * location + 2;
}

static void pqueue_tree_print(struct PriorityQueue *q, size_t n, int l)
{
    if(n < q->size)
    {
        for(int i = 0; i < l; i++) fputs("  ", stderr);
        printf("%d p(%d)\n", q->items[n].value, q->items[n].priority);
        pqueue_tree_print(q, pqueue_lchild(n), l + 1);
        pqueue_tree_print(q, pqueue_rchild(n), l + 1);
    }
}

void pqueue_debug_print(struct PriorityQueue *q)
{
    fprintf(stderr, "<%ld/%ld>\n", q->size, q->capacity);
    pqueue_tree_print(q, 0, 0);
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
    q->items = malloc(capacity * sizeof(*q->items));
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
    q->items = realloc(q->items, q->capacity * sizeof(*q->items));

    assert(pqueue_valid(q));
}

static void pqueue_init(struct PriorityQueue *q)
{
    pqueue_init_capacity(q, QUEUE_DEFAULT_CAPACITY);
}

static void pqueue_bubble_up(struct PriorityQueue *q, size_t location)
{
    assert(q);
    if(location != 0)
    {
        size_t parent = pqueue_parent(location);
        if(q->items[parent].priority > q->items[location].priority)
        {
            pqueue_swap(q, parent, location);
            pqueue_bubble_up(q, parent);
        }
    }
}

static void pqueue_insert(struct PriorityQueue *q, TileID value, int priority)
{
    assert(pqueue_valid(q));

    if(q->size == q->capacity)
    {
        pqueue_realloc(q);
    }
    struct HeapItem *item = &q->items[q->size];
    item->value = value;
    item->priority = priority;
    pqueue_bubble_up(q, q->size);
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

static TileID pqueue_extract(struct PriorityQueue *q)
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
struct Queue
{
    TileID *data;
    size_t capacity;
    TileID *start;
    TileID *end;
};

// Verifies invariants of queue
static bool queue_valid(struct Queue *q)
{
    TileID *data_start = &q->data[0];
    TileID *data_end = &q->data[q->capacity];
    return (data_start <= q->start && q->start < data_end)
        && (data_start <= q->end && q->end < data_end);
}

static void queue_init_capacity(struct Queue *q, size_t size)
{
    assert(q);

    q->capacity = size;
    q->data = malloc(sizeof(*q->data) * q->capacity);
    q->start = &q->data[0];
    q->end = q->start + 1;

    assert(queue_valid(q));
}

static void queue_init(struct Queue *q)
{
    queue_init_capacity(q, QUEUE_DEFAULT_CAPACITY);
}

static void queue_destroy(struct Queue *q)
{
    free(q->data);
}

static size_t queue_size(struct Queue *q)
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
static TileID *q_next(struct Queue *q, TileID *i)
{
    return i == &q->data[q->capacity - 1] ? &q->data[0] : i + 1;
}

// " previous "
static TileID *q_prev(struct Queue *q, TileID *i)
{
    return i == &q->data[0] ? &q->data[q->capacity - 1] : i - 1;
}

static bool queue_empty(struct Queue *q)
{
    return q_next(q, q->start) == q->end;
}

// Resizes the queue, copying old elements to the new space.
static void queue_realloc(struct Queue *q)
{
    size_t old_size = queue_size(q);
    q->capacity *= 2;
    TileID *new = malloc(sizeof(*new) * q->capacity);
    for(TileID *i = &new[0];
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

static void enqueue(struct Queue *q, TileID tile)
{
    assert(queue_valid(q));

    if(q->end == q->start)
    {
        queue_realloc(q);
    }
    *q_prev(q, q->end) = tile;
    q->end = q_next(q, q->end);

    assert(queue_valid(q));
}

static TileID dequeue(struct Queue *q)
{
    assert(queue_valid(q));
    assert(queue_size(q) > 0);
    
    TileID out = *q->start;
    if(!queue_empty(q))
    {
        q->start = q_next(q, q->start);
    }
    return out;

    assert(queue_valid(q));
}

static void queue_reset(struct Queue *q)
{
    assert(q);
    q->start = &q->data[0];
    q->end = q->start + 1;
}

void path_init_capacity(struct Path *p, size_t capacity)
{
    assert(p);
    p->capacity = capacity;
    p->points = malloc(sizeof(p->points[0]) * p->capacity);
    p->size = 0;

}

void path_init(struct Path *p)
{
    path_init_capacity(p, PATH_DEFAULT_CAPACITY);
}

void path_copy(const struct Path *src, struct Path *dest)
{
    assert(src);
    assert(dest);

    free(dest->points);
    path_init_capacity(dest, src->capacity);
    dest->size = src->size;
    memcpy(dest->points, src->points, sizeof(src->points[0]) * src->size);
}

static void path_push_point(struct Path *p, const struct Point *pt)
{
    if(p->size == p->capacity)
    {
        p->capacity *= 2;
        p->points = realloc(p->points, sizeof(*p->points) * p->capacity);
    }
    p->points[p->size] = *pt;
    p->size++;
}

static void path_reverse(struct Path *p)
{
    assert(p);
    for(size_t i = 0; i < p->size / 2; i++)
    {
        struct Point tmp;
        struct Point *a = &p->points[i];
        struct Point *b = &p->points[p->size - i - 1];
        tmp = *a;
        *a = *b;
        *b = tmp;
    }
}

static void path_reset(struct Path *p)
{
    p->size = 0;
}

static void path_from_to_astar(struct Path *out, const TileMap *map,
                               const struct Point *from,
                               const struct Point *to)
{
    assert(out); assert(map); assert(from); assert(to);

    struct PriorityQueue queue;
    struct BitVec seen;
    unsigned int max_id = tile_map_max_id(map);
    int *parent = calloc(max_id, sizeof(*parent));
    int *cost = calloc(max_id, sizeof(*cost));
    TileID tid = tile_map_tile_id(map, from);

    pqueue_init(&queue);
    bv_init(&seen, max_id);
    path_reset(out);

    pqueue_insert(&queue, tid, 0);
    bv_set(&seen, tid, true);
    parent[tid] = -1;
    cost[tid] = 1;

    while(!pqueue_empty(&queue))
    {
        TileID current = pqueue_extract(&queue);
        struct Point cur_pt;
        tile_map_coord(map, current, &cur_pt);

        if(cur_pt.x == to->x && cur_pt.y == to->y)
        {
            do
            {
                tile_map_coord(map, current, &cur_pt);
                path_push_point(out, &cur_pt);
                current = parent[current];
            }while(current != -1);
            path_reverse(out);
            break;
        }

        TileID neighbors[4]; //TODO: remove magic #
        unsigned int nbr_count = tile_map_neighbors(map, current, neighbors, 4);
        assert(nbr_count <= 4);
        for(int i = 0; i < nbr_count; i++)
        {
            struct Point pt;
            tile_map_coord(map, neighbors[i], &pt);
            if(!bv_get(&seen, neighbors[i]))
            {
                bv_set(&seen, neighbors[i], true);
                parent[neighbors[i]] = current;
                cost[neighbors[i]] = cost[current] + 1;
                unsigned int priority = cost[neighbors[i]] + (abs(pt.x - to->x) + abs(pt.y - to->y));
                pqueue_insert(&queue, neighbors[i], priority);
            }
        }
    }

    pqueue_destroy(&queue);
    bv_destroy(&seen);
    free(parent);
    free(cost);
}

bool path_nearest_tile(const struct TileMap *map,
                       const struct Point *start,
                       struct Point *found,
                       bool (*pred)(const TileInfo *tile))
{
    assert(map);
    assert(start);
    assert(pred);
    assert(found);
    bool found_tile = false;
    struct Queue queue;
    struct BitVec seen;

    bv_init(&seen, tile_map_max_id(map));
    queue_init(&queue);
    
    TileID start_id = tile_map_tile_id(map, start);
    enqueue(&queue, start_id); 
    bv_set(&seen, start_id, true);

    while(!queue_empty(&queue))
    {
        TileID tid = dequeue(&queue);
        TileInfo *info = tile_map_get_tilei(map, tid);
        if(pred(info))
        {
            tile_map_coord(map, tid, found);
            found_tile = true;
            break;
        }

        TileID neighbors[4];
        unsigned int count = tile_map_neighbors(map, tid, neighbors, 4);
        for(int i = 0; i < count; i++)
        {
            if(!bv_get(&seen, neighbors[i]))
            {
                bv_set(&seen, neighbors[i], true);
                enqueue(&queue, neighbors[i]);
            }
        }
    }

    queue_destroy(&queue);
    bv_destroy(&seen);
    return found_tile;
}

void path_from_to(struct Path *out, const TileMap *map,
                  const struct Point *from,
                  const struct Point *to)
{
    assert(out); assert(map); assert(from); assert(to);
    path_from_to_astar(out, map, from, to);
}

void path_fprint(struct Path *path, FILE *file)
{
    assert(path);
    struct Point *end = &path->points[path->size];
    fprintf(file, "Path[");
    for(struct Point *i = &path->points[0]; i != end; i++)
    {
        fprintf(file, "(%d,%d) ", i->x, i->y);
    }
    fprintf(file, "]\n");
}

void path_cleanup(struct Path *p)
{
    free(p->points);
}

