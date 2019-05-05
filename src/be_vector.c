#include "be_vector.h"
#include "be_mem.h"
#include "be_string.h"
#include <string.h>

void be_vector_init(bvector *vector, int size)
{
    vector->capacity = 2;
    vector->count = 0;
    vector->size = size;
    vector->data = be_malloc((size_t)vector->capacity * size);
    vector->end = (char*)vector->data - size;
}

void be_vector_delete(bvector *vector)
{
    be_free(vector->data);
}

void* be_vector_at(bvector *vector, int index)
{
    char *pv = (char*)vector->data;
    return pv + (size_t)index * vector->size;
}

void be_vector_append(bvector *vector, void *data)
{
    int capacity = vector->capacity;
    size_t size = vector->size;
    if (vector->count >= capacity) {
        vector->capacity = be_nextsize(capacity);
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
    int newcap = be_nextsize(count);
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
void* be_vector_release(bvector *vector)
{
    if (vector->count == 0) {
        be_free(vector->data);
        vector->capacity = 0;
        vector->data = NULL;
        vector->end = NULL;
    } else if (vector->count < vector->capacity) {
        size_t size = vector->size;
        /* vector->capacity minimum size is 1 */
        vector->capacity = vector->count < 1 ? 1 : vector->count;
        vector->data = be_realloc(vector->data, vector->capacity * size);
        vector->end = (char*)vector->data + (vector->count - 1) * size;
    }
    return vector->data;
}

static int binaryfind(int value)
{
    static const uint16_t tab[] = {
        0, 2, 4, 6, 8, 10, 12, 14, 16,
        20, 24, 28, 32, 40, 48, 64, 96, 128,
        192, 256, 384, 512, 768, 1024
    };
    const uint16_t *p = tab;
    const uint16_t *q = p + (int)array_count(tab);
    while (p <= q) {
        const uint16_t *m = p + ((q - p) >> 1);
        if (*m == value) {
            return m[1];
        }
        if (*m < value) {
            p = m + 1;
        } else {
            q = m - 1;
        }
    }
    return *p;
}

static int nextpow(int value)
{
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return value + 1;
}

int be_nextsize(int size)
{
    if (size < 1024) {
        return binaryfind(size);
    }
    return nextpow(size);
}
