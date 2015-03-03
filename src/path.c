#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "path.h"

static const size_t PATH_DEFAULT_CAPACITY = 50;
static const size_t QUEUE_DEFAULT_CAPACITY = 50;

struct PtQueue
{
    Point *data;
    size_t capacity;
    Point *start;
    Point *end;
};

static void queue_init(struct PtQueue *q)
{
    assert(q);
    q->capacity = QUEUE_DEFAULT_CAPACITY;
    q->data = malloc(sizeof(Point) * q->capacity);
    q->start = &q->data[0];
    q->end = q->start + 1;
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
    return i == &q->data[q->capacity] ? &q->data[0] : i + 1;
}

// " previous " 
static Point *q_prev(struct PtQueue *q, Point *i)
{
    return i == &q->data[0] ? &q->data[q->capacity] : i - 1;
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

static void enqueue(struct PtQueue *q, Point *p)
{
    if(q->end == q->start)
    {
        queue_realloc(q);
    }
    *q_prev(q, q->end) = *p;
    q->end = q_next(q, q->end);
}

static void dequeue(struct PtQueue *q, Point *out)
{
    assert(queue_size(q) > 0);

    *out = *q->start;
    if(q_next(q, q->start) != q_prev(q, q->end))
    {
        q->start = q_next(q, q->start);
    }
}

typedef void (*QFE_Function)(Point*);
void queue_print(struct PtQueue *q, QFE_Function func)
{
    for(Point *s = q->start; s != q_prev(q, q->end); s = q_next(q, s))
    {
        func(s);
    }
}

void path_init(struct Path *p)
{
    p->points = malloc(sizeof(p->points[0]) * PATH_DEFAULT_CAPACITY);
    p->capacity = PATH_DEFAULT_CAPACITY;
    p->size = 0;
}

static void path_push_point(struct Path *p, const struct Point *pt)
{
    if(p->size + 1 > p->capacity)
    {
        p->capacity *= 2;
        p->points = realloc(p->points, sizeof(struct Point) * p->capacity);
    }
    p->points[p->size] = *pt;
    p->size++;
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

void path_from_to_astar(Path *out, const TileMap *map,
                        const Point *from,
                        const Point *to)
{
}

void path_destroy(struct Path *p)
{
    free(p->points);
}


