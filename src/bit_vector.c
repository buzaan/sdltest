#include <assert.h>
#include <limits.h>
#include <string.h>
#include "bit_vector.h"

#include <stdio.h>

static size_t uint_bits = sizeof(unsigned int) * CHAR_BIT;

void bv_init(struct BitVec *v, size_t bit_capacity)
{
    size_t words = bit_capacity / uint_bits;
    if(bit_capacity % uint_bits)
    {
        words++;
    }
    
    v->data = calloc(words, sizeof(unsigned int));
    v->words = words;
}

void bv_destroy(struct BitVec *v)
{
    free(v->data);
}

void bv_set(struct BitVec *v, unsigned int loc, bool value)
{
    assert(v);
    assert(loc <= v->words * uint_bits);
    
    size_t word = loc / uint_bits;
    size_t bit = loc % uint_bits;
    v->data[word] |= value << bit;
}

bool bv_get(struct BitVec *v, unsigned int loc)
{
    assert(v);
    assert(loc <= v->words * uint_bits);

    size_t word = loc / uint_bits;
    size_t bit = loc % uint_bits;
    return v->data[word] & (1 << bit);
}
