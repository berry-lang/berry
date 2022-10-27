/**
 * @file       be_list.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_LIST_H
#define BE_LIST_H

#include "be_object.h"

/**
 * @struct blist
 * @brief blist
 *
 */
struct blist {
    bcommon_header;      /**< bcommon_header */
    bgcobject_t *gray;   /**< for gc gray list */
    int count, capacity; /**< capacity, count */
    bvalue_t *data;      /**< data */
};

#define be_list_data(list)          ((list)->data)                 /**< be_list_data */
#define be_list_count(list)         ((list)->count)                /**< be_list_count */
#define be_list_at(list, index)     ((list)->data + index)         /**< be_list_at */
#define be_list_end(list)           ((list)->data + (list)->count) /**< be_list_end */

/**
 * @fn blist_t be_list_new*(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
blist_t* be_list_new(bvm_t *vm);

/**
 * @fn void be_list_delete(bvm_t*, blist_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 */
void be_list_delete(bvm_t *vm, blist_t *list);

/**
 * @fn blist_t be_list_copy*(bvm_t*, blist_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param original (???)
 * @return (???)
 */
blist_t* be_list_copy(bvm_t *vm, blist_t *original);

/**
 * @fn bvalue_t be_list_index*(blist_t*, int)
 * @brief (???)
 *
 * @param list (???)
 * @param index (???)
 * @return (???)
 */
bvalue_t* be_list_index(blist_t *list, int index);

/**
 * @fn bvalue_t be_list_push*(bvm_t*, blist_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param value (???)
 * @return (???)
 */
bvalue_t* be_list_push(bvm_t *vm, blist_t *list, bvalue_t *value);

/**
 * @fn bvalue_t be_list_insert*(bvm_t*, blist_t*, int, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param index (???)
 * @param value (???)
 * @return (???)
 */
bvalue_t* be_list_insert(bvm_t *vm, blist_t *list, int index, bvalue_t *value);

/**
 * @fn int be_list_remove(bvm_t*, blist_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param index (???)
 * @return (???)
 */
int be_list_remove(bvm_t *vm, blist_t *list, int index);

/**
 * @fn void be_list_resize(bvm_t*, blist_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param count (???)
 */
void be_list_resize(bvm_t *vm, blist_t *list, int count);

/**
 * @fn void be_list_merge(bvm_t*, blist_t*, const blist_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param other (???)
 */
void be_list_merge(bvm_t *vm, blist_t *list, const blist_t *other);

/**
 * @fn void be_list_reverse(blist_t*)
 * @brief (???)
 *
 * @param list (???)
 */
void be_list_reverse(blist_t *list);

/**
 * @fn void be_list_pool_init(bvm_t*, blist_t*)
 * @brief  (???)
 *
 * @param vm (???)
 * @param list (???)
 */
void be_list_pool_init(bvm_t *vm, blist_t *list);

/**
 * @fn int be_list_pool_alloc(bvm_t*, blist_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param src (???)
 * @return (???)
 */
int be_list_pool_alloc(bvm_t *vm, blist_t *list, bvalue_t *src);

/**
 * @fn void be_list_pool_free(blist_t*, int)
 * @brief (???)
 *
 * @param list (???)
 * @param id (???)
 */
void be_list_pool_free(blist_t *list, int id);

#endif
