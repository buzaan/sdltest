#pragma once
#include <stdbool.h>
#include <stdlib.h>

struct BitVec
{
    unsigned int *data;
    unsigned int words;
};

void bv_init(struct BitVec *v, size_t capacity);
void bv_set(struct BitVec *v, unsigned int loc, bool value);
bool bv_get(struct BitVec *v, unsigned int loc);
void bv_destroy(struct BitVec *v);
