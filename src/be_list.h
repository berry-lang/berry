/**
 * @file       be_list.h
 * @brief      Main berry entry API
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
    bgcobject *gray;     /**< for gc gray list */
    int count, capacity; /**< capacity, count */
    bvalue *data;        /**< data */
};

#define be_list_data(list)          ((list)->data)                 /**< be_list_data */
#define be_list_count(list)         ((list)->count)                /**< be_list_count */
#define be_list_at(list, index)     ((list)->data + index)         /**< be_list_at */
#define be_list_end(list)           ((list)->data + (list)->count) /**< be_list_end */

/**
 * @fn blist be_list_new*(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
blist* be_list_new(bvm *vm);

/**
 * @fn void be_list_delete(bvm*, blist*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 */
void be_list_delete(bvm *vm, blist *list);

/**
 * @fn blist be_list_copy*(bvm*, blist*)
 * @brief (???)
 *
 * @param vm (???)
 * @param original (???)
 * @return (???)
 */
blist* be_list_copy(bvm *vm, blist *original);

/**
 * @fn bvalue be_list_index*(blist*, int)
 * @brief (???)
 *
 * @param list (???)
 * @param index (???)
 * @return (???)
 */
bvalue* be_list_index(blist *list, int index);

/**
 * @fn bvalue be_list_push*(bvm*, blist*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param value (???)
 * @return (???)
 */
bvalue* be_list_push(bvm *vm, blist *list, bvalue *value);

/**
 * @fn bvalue be_list_insert*(bvm*, blist*, int, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param index (???)
 * @param value (???)
 * @return (???)
 */
bvalue* be_list_insert(bvm *vm, blist *list, int index, bvalue *value);

/**
 * @fn int be_list_remove(bvm*, blist*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param index (???)
 * @return (???)
 */
int be_list_remove(bvm *vm, blist *list, int index);

/**
 * @fn void be_list_resize(bvm*, blist*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param count (???)
 */
void be_list_resize(bvm *vm, blist *list, int count);

/**
 * @fn void be_list_merge(bvm*, blist*, const blist*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param other (???)
 */
void be_list_merge(bvm *vm, blist *list, const blist *other);

/**
 * @fn void be_list_reverse(blist*)
 * @brief (???)
 *
 * @param list (???)
 */
void be_list_reverse(blist *list);

/**
 * @fn void be_list_pool_init(bvm*, blist*)
 * @brief  (???)
 *
 * @param vm (???)
 * @param list (???)
 */
void be_list_pool_init(bvm *vm, blist *list);

/**
 * @fn int be_list_pool_alloc(bvm*, blist*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param list (???)
 * @param src (???)
 * @return (???)
 */
int be_list_pool_alloc(bvm *vm, blist *list, bvalue *src);

/**
 * @fn void be_list_pool_free(blist*, int)
 * @brief (???)
 *
 * @param list (???)
 * @param id (???)
 */
void be_list_pool_free(blist *list, int id);

#endif
