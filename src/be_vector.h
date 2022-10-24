/**
 * @file       be_vector.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_VECTOR_H
#define BE_VECTOR_H

#include "be_object.h"

/**
 * @note Defines
 * @{
 */
#define be_vector_data(vector)          ((vector)->data)                /**< be_vector_data */
#define be_vector_first(vector)         ((vector)->data)                /**< be_vector_first */
#define be_vector_isend(vector, item)   ((item) > (vector)->end)        /**< be_vector_isend */
#define be_vector_isempty(vector)       (!(vector)->count)              /**< be_vector_isempty */
#define be_vector_end(vector)           ((vector)->end)                 /**< be_vector_end */
#define be_vector_count(vector)         ((vector)->count)               /**< be_vector_count */
#define be_vector_capacity(vector)      ((vector)->capacity)            /**< be_vector_capacity */
#define be_stack_init(vm, stack, size)  be_vector_init(vm, stack, size) /**< be_stack_init */
#define be_stack_delete(vm, stack)      be_vector_delete(vm, stack)     /**< be_stack_delete */
#define be_stack_clear(stack)           be_vector_clear(stack)          /**< be_stack_clear */
#define be_stack_push(vm, stack, data)  be_vector_push(vm, stack, data) /**< be_stack_push */
#define be_stack_pop(stack)             be_vector_remove_end(stack)     /**< be_stack_pop */
#define be_stack_top(stack)             be_vector_end(stack)            /**< be_stack_top */
#define be_stack_base(stack)            be_vector_first(stack)          /**< be_stack_base */
#define be_stack_count(stack)           be_vector_count(stack)          /**< be_stack_count */
#define be_stack_isempty(stack)         be_vector_isempty(stack)        /**< be_stack_isempty */
/** @} */

/**
 * @fn void be_vector_init(bvm*, bvector*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param vector (???)
 * @param size (???)
 */
void be_vector_init(bvm *vm, bvector *vector, int size);

/**
 * @fn void be_vector_delete(bvm*, bvector*)
 * @brief (???)
 *
 * @param vm (???)
 * @param vector (???)
 */
void be_vector_delete(bvm *vm, bvector *vector);

/**
 * @fn void be_vector_at*(bvector*, int)
 * @brief (???)
 *
 * @param vector (???)
 * @param index (???)
 */
void* be_vector_at(bvector *vector, int index);

/**
 * @fn void be_vector_push(bvm*, bvector*, void*)
 * @brief (???)
 *
 * @param vm (???)
 * @param vector (???)
 * @param data (???)
 */
void be_vector_push(bvm *vm, bvector *vector, void *data);

/**
 * @fn void be_vector_push_c(bvm*, bvector*, void*)
 * @brief (???)
 *
 * @param vm (???)
 * @param vector (???)
 * @param data (???)
 */
void be_vector_push_c(bvm *vm, bvector *vector, void *data);

/**
 * @fn void be_vector_remove_end(bvector*)
 * @brief (???)
 *
 * @param vector (???)
 */
void be_vector_remove_end(bvector *vector);

/**
 * @fn void be_vector_resize(bvm*, bvector*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param vector (???)
 * @param count (???)
 */
void be_vector_resize(bvm *vm, bvector *vector, int count);

/**
 * @fn void be_vector_clear(bvector*)
 * @brief (???)
 *
 * @param vector (???)
 */
void be_vector_clear(bvector *vector);

/**
 * @fn void be_vector_release*(bvm*, bvector*)
 * @brief (???)
 *
 * @param vm (???)
 * @param vector (???)
 */
void* be_vector_release(bvm *vm, bvector *vector);

/**
 * @fn int be_nextsize(int)
 * @brief (???)
 *
 * @param value (???)
 * @return (???)
 */
int be_nextsize(int value);
/** @} */

#endif
