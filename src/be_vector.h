#ifndef __BE_VECTOR_H
#define __BE_VECTOR_H

#include "be_object.h"

/* ========================== data struct define ========================== */
typedef struct bvector {
    int count, capacity, size;
    void *data, *end;
} bvector;

typedef bvector bstack;

/* =============================== defines =============================== */
#define be_vector_count(vector)         ((vector)->count)
#define be_vector_data(vector)          ((vector)->data)
#define be_vector_first(vector)         ((vector)->data)
#define be_vector_isend(vector, item)   ((item) > (vector)->end)
#define be_vector_end(vector)           ((vector)->end)
#define be_stack_new(size)              be_vector_new(size)
#define be_stack_delete(stack)          be_vector_delete(stack)
#define be_stack_clear(stack)           be_vector_clear(stack)
#define be_stack_push(stack, data)      be_vector_append(stack, data)
#define be_stack_pop(stack)             be_vector_remove_end(stack)
#define be_stack_top(stack)             be_vector_end(stack)
#define be_stack_base(stack)            be_vector_first(stack)
#define be_stack_isempty(stack)         (be_vector_count(stack) == 0)

/* ========================== function extern ========================== */
bvector* be_vector_new(int size);
void be_vector_delete(bvector *vector);
void* be_vector_at(bvector *vector, int index);
void be_vector_append(bvector *vector, void *data);
void be_vector_remove_end(bvector *vector);
void be_vector_resize(bvector *vector, int count);
void be_vector_clear(bvector *vector);
void be_vector_release(bvector *vector);
void* be_vector_swap_delete(bvector *vector);
int be_nextpow(int v);

#endif
