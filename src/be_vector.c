#include "be_vector.h"
#include "be_mem.h"
#include "be_string.h"
#include <string.h>

#define VECTOR_DEFAULT_SIZE     4

#define addr_pos(a, p)          ((void*)((char*)(a) + (p)))
#define addr_region(a, s)       (addr_pos((a), sizeof(s)))
#define addr_base(a, s)         (addr_pos((a), -(int)sizeof(s)))

bvector* be_vector_new(int size)
{
    bvector *vector = be_malloc(sizeof(bvector));
    vector->capacity = VECTOR_DEFAULT_SIZE;
    vector->count = 0;
    vector->size = size;
    vector->data = be_malloc((size_t)vector->capacity * size);
    vector->end = (char*)vector->data - size;
    return vector;
}

void be_vector_delete(bvector *vector)
{
    bvector *obj = vector;
    be_free(obj->data);
    be_free(obj);
}

void* be_vector_at(bvector *vector, int index)
{
    char *pv = (char*)vector->data;
    return pv + (size_t)index * vector->size;
}

void be_vector_append(bvector *vector, void *data)
{
    size_t size = vector->size;
    if (vector->count >= vector->capacity) {
        vector->capacity <<= 1; /* capacity *= 2 */
        vector->data = be_realloc(vector->data, vector->capacity * size);
        vector->end = (char*)vector->data + vector->count * size;
    } else {
        vector->end = (char*)vector->end + size;
    }
    ++vector->count;
    if (data != NULL) {
        memcpy(vector->end, data, size);
    }
}

void be_vector_remove_end(bvector *vector)
{
    if (vector->count > 0) {
        --vector->count;
        vector->end = (char*)vector->end - vector->size;
    }
}

void be_vector_resize(bvector *vector, int count)
{
    size_t size = vector->size;
    int newcap = be_nextpow(count);
    if (count != vector->count) {
        vector->count = count;
        if (newcap > vector->capacity) {
            vector->capacity = newcap;
            vector->data = be_realloc(vector->data, newcap * size);
        }
        vector->end = (char*)vector->data + size * (count - 1);
    }
}

void be_vector_clear(bvector *vector)
{
    vector->count = 0;
    vector->end = (char*)vector->data - vector->size;
}

/* free not used */
void be_vector_release(bvector *vector)
{
    if (vector->count < vector->capacity) {
        size_t size = vector->size;
        /* vector->capacity minimum size is 1 */
        vector->capacity = vector->count < 1 ? 1 : vector->count;
        vector->data = be_realloc(vector->data, vector->capacity * size);
        vector->end = (char*)vector->data + (vector->count - 1) * size;
    }
}

/* delete vector but exclude data */
void* be_vector_swap_delete(bvector *vector)
{
    void *data;
    be_vector_release(vector);
    data = vector->data;
    be_free(vector);
    return data;
}

int be_nextpow(int v)
{
    int i;
    for (i = 4; i < v; i <<= 1);
    return i;
}
