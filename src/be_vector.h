#ifndef BE_VECTOR_H
#define BE_VECTOR_H

#include "be_object.h"

/* =============================== defines =============================== */
#define be_vector_data(vector)          ((vector)->data)
#define be_vector_first(vector)         ((vector)->data)
#define be_vector_isend(vector, item)   ((item) > (vector)->end)
#define be_vector_end(vector)           ((vector)->end)
#define be_vector_capacity(vector)      ((vector)->capacity)
#define be_stack_init(vm, stack, size)  be_vector_init(vm, stack, size)
#define be_stack_delete(vm, stack)      be_vector_delete(vm, stack)
#define be_stack_clear(stack)           be_vector_clear(stack)
#define be_stack_push(vm, stack, data)  be_vector_append(vm, stack, data)
#define be_stack_pop(stack)             be_vector_remove_end(stack)
#define be_stack_top(stack)             be_vector_end(stack)
#define be_stack_base(stack)            be_vector_first(stack)
#define be_stack_count(stack)           be_vector_count(stack)
#define be_stack_isempty(stack)         be_vector_isempty(stack)

/* ========================== function extern ========================== */
void be_vector_init(bvm *vm, bvector *vector, int size);
void be_vector_delete(bvm *vm, bvector *vector);
int be_vector_count(bvector *vector);
bbool be_vector_isempty(bvector *vector);
void* be_vector_at(bvector *vector, int index);
void be_vector_append(bvm *vm, bvector *vector, void *data);
void be_vector_append_c(bvm *vm, bvector *vector, void *data);
void be_vector_remove_end(bvector *vector);
void be_vector_resize(bvm *vm, bvector *vector, int count);
void be_vector_clear(bvector *vector);
void* be_vector_release(bvm *vm, bvector *vector);
int be_nextsize(int value);

#endif
