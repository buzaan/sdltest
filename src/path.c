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
struct PtQueue
{
    struct Point *data;
    size_t capacity;
    struct Point *start;
    struct Point *end;
};

// Verifies invariants of queue
static bool queue_valid(struct PtQueue *q)
{
    struct Point *data_start = &q->data[0];
    struct Point *data_end = &q->data[q->capacity];
    return (data_start <= q->start && q->start < data_end)
        && (data_start <= q->end && q->end < data_end);
}

static void queue_init_capacity(struct PtQueue *q, size_t size)
{
    assert(q);

    q->capacity = size;
    q->data = malloc(sizeof(struct Point) * q->capacity);
    q->start = &q->data[0];
    q->end = q->start + 1;

    assert(queue_valid(q));
}

static void queue_init(struct PtQueue *q)
{
    queue_init_capacity(q, QUEUE_DEFAULT_CAPACITY);
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
static struct Point *q_next(struct PtQueue *q, struct Point *i)
{
    return i == &q->data[q->capacity - 1] ? &q->data[0] : i + 1;
}

// " previous "
static struct Point *q_prev(struct PtQueue *q, struct Point *i)
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
    struct Point *new = malloc(sizeof(struct Point) * q->capacity);
    for(struct Point *i = &new[0];
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

static void enqueue(struct PtQueue *q, const struct Point *p)
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

static void dequeue(struct PtQueue *q, struct Point *out)
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
    assert(p);
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
    int *parent = calloc(max_id, sizeof(int));
    int *cost = calloc(max_id, sizeof(int));
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

void path_from_to_bfs(struct Path *out, const TileMap *map,
                      const struct Point *from,
                      const struct Point *to)
{

}

void path_from_to(struct Path *out, const TileMap *map,
                  const struct Point *from,
                  const struct Point *to)
{
    assert(out); assert(map); assert(from); assert(to);
    path_from_to_astar(out, map, from, to);
}

void path_destroy(struct Path *p)
{
    free(p->points);
}
