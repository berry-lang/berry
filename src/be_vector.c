#include "be_vector.h"
#include "be_mem.h"
#include "be_string.h"
#include <string.h>

void be_vector_init(bvector *vector, int size)
{
    vector->capacity = 2;
    vector->size = size;
    vector->data = be_malloc((size_t)vector->capacity * size);
    vector->end = (char*)vector->data - size;
}

void be_vector_delete(bvector *vector)
{
    be_free(vector->data);
}

int be_vector_count(bvector *vector)
{
    size_t size = vector->size;
    return vector->data ?
        cast_int(((size_t)vector->end + size - (size_t)vector->data) / size)
        : 0;
}

bbool be_vector_isempty(bvector *vector)
{
    return vector->data && vector->data > vector->end;
}

void* be_vector_at(bvector *vector, int index)
{
    char *pv = (char*)vector->data;
    return pv + (size_t)index * vector->size;
}

void be_vector_append(bvector *vector, void *data)
{
    size_t size = vector->size;
    size_t capacity = vector->capacity;
    size_t count = be_vector_count(vector);
    if (count >= capacity) {
        vector->capacity = be_nextsize(capacity);
        vector->data = be_realloc(vector->data, vector->capacity * size);
        vector->end = (char*)vector->data + count * size;
    } else {
        vector->end = (char*)vector->end + size;
    }
    if (data != NULL) {
        memcpy(vector->end, data, size);
    }
}

void be_vector_remove_end(bvector *vector)
{
    vector->end = (char*)vector->end - vector->size;
}

void be_vector_resize(bvector *vector, int count)
{
    size_t size = vector->size;
    int newcap = be_nextsize(count);
    if (count != be_vector_count(vector)) {
        if (newcap > vector->capacity) {
            vector->capacity = newcap;
            vector->data = be_realloc(vector->data, newcap * size);
        }
        vector->end = (char*)vector->data + size * (count - 1);
    }
}

void be_vector_clear(bvector *vector)
{
    vector->end = (char*)vector->data - vector->size;
}

/* free not used */
void* be_vector_release(bvector *vector)
{
    int count = be_vector_count(vector);
    if (count == 0) {
        be_free(vector->data);
        vector->capacity = 0;
        vector->data = NULL;
        vector->end = NULL;
    } else if (count < vector->capacity) {
        size_t size = vector->size;
        vector->capacity = count;
        vector->data = be_realloc(vector->data, vector->capacity * size);
        vector->end = (char*)vector->data + (count - 1) * size;
    }
    return vector->data;
}

static int binarysearch(int value)
{
    static const uint16_t tab[] = {
        0, 2, 4, 6, 8, 10, 12, 14, 16,
        20, 24, 28, 32, 40, 48, 64, 96, 128,
        192, 256, 384, 512, 768, 1024
    };
    const uint16_t *low = tab;
    const uint16_t *high = low + (int)array_count(tab);
    while (low <= high) {
        const uint16_t *mid = low + ((high - low) >> 1);
        if (*mid == value) {
            return mid[1];
        }
        if (*mid < value) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return *low;
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
        return binarysearch(size);
    }
    return nextpow(size);
}
