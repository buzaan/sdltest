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
    q->capacity = sizeof(Point) * QUEUE_DEFAULT_CAPACITY;
    q->data = malloc(q->capacity);
    q->start = q->data[0];
    q->end = q->data[0];
}

static void enqueue(struct PtQueue *q, const Point *pt)
{
    if(q->end == q->data[q->capacity])
    {
    }
    else
    {
        q->end = *pt;
        q->end++;
    }
}

static void dequeue(struct PtQueue *q, Point *out)
{

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

void path_from_to_bfs(Path *out, const TileMap *map,
                      const Point *from,
                      const Point *to)
{
    
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

