#include "be_vector.h"
#include "be_string.h"
#include "be_mem.h"
#include <string.h>

void be_vector_init(bvm *vm, bvector *vector, int size)
{
    vector->capacity = 2;
    vector->size = size;
    vector->data = be_malloc(vm, (size_t)vector->capacity * size);
    vector->end = (char*)vector->data - size;
    memset(vector->data, 0, (size_t)vector->capacity * size);
}

void be_vector_delete(bvm *vm, bvector *vector)
{
    be_free(vm, vector->data, (size_t)vector->capacity * vector->size);
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
    return cast_bool(vector->data && vector->data > vector->end);
}

void* be_vector_at(bvector *vector, int index)
{
    return (char*)vector->data + (size_t)index * vector->size;
}

void be_vector_append(bvm *vm, bvector *vector, void *data)
{
    size_t size = vector->size;
    size_t capacity = vector->capacity;
    size_t count = be_vector_count(vector);
    if (count >= capacity) {
        int newcap = be_nextsize(vector->capacity);
        vector->data = be_realloc(vm,
                vector->data, vector->capacity * size, newcap * size);
        vector->end = (char*)vector->data + count * size;
        vector->capacity = newcap;
    } else {
        vector->end = (char*)vector->end + size;
    }
    if (data != NULL) {
        memcpy(vector->end, data, size);
    }
}

/* clear the expanded portion if the memory expands */
void be_vector_append_c(bvm *vm, bvector *vector, void *data)
{
    int capacity = vector->capacity + 1;
    be_vector_append(vm, vector, data);
    if (vector->capacity > capacity) {
        size_t size = (vector->capacity - capacity) * vector->size;
        memset(be_vector_at(vector, capacity), 0, size);
    }
}

void be_vector_remove_end(bvector *vector)
{
    vector->end = (char*)vector->end - vector->size;
}

void be_vector_resize(bvm *vm, bvector *vector, int count)
{
    size_t size = vector->size;
    int newcap = be_nextsize(count);
    if (count != be_vector_count(vector)) {
        if (newcap > vector->capacity) {
            vector->data = be_realloc(vm,
                vector->data, vector->capacity * size, newcap * size);
            vector->capacity = newcap;
        }
        vector->end = (char*)vector->data + size * (count - 1);
    }
}

void be_vector_clear(bvector *vector)
{
    vector->end = (char*)vector->data - vector->size;
}

/* free not used */
void* be_vector_release(bvm *vm, bvector *vector)
{
    size_t size = vector->size;
    int count = be_vector_count(vector);
    if (count == 0) {
        be_free(vm, vector->data, vector->capacity * size);
        vector->capacity = 0;
        vector->data = NULL;
        vector->end = NULL;
    } else if (count < vector->capacity) {
        vector->data = be_realloc(vm,
            vector->data, vector->capacity * size, count * size);
        vector->end = (char*)vector->data + (count - 1) * size;
        vector->capacity = count;
    }
    return vector->data;
}

/* use binary search to find the vector capacity between 0-1024 */
static int binary_search(int value)
{
    static const uint16_t tab[] = {
        0, 2, 4, 6, 8, 10, 12, 14, 16,
        20, 24, 28, 32, 40, 48, 64, 96, 128,
        192, 256, 384, 512, 768, 1024
    };
    const uint16_t *low = tab;
    const uint16_t *high = tab + array_count(tab) - 1;
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
        return binary_search(size);
    }
    return nextpow(size);
}
