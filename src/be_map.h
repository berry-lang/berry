/**
 * @file       be_map.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_MAP_H
#define BE_MAP_H

#include "be_object.h"

/**
 * @struct bmapkey
 * @brief bmapkey
 *
 */
typedef struct bmapkey {
    union bvaldata v; /**< v */
    uint32_t type:8;  /**< type */
    uint32_t next:24; /**< next */
} bmapkey_t;          /**< bmapkey_t */

/**
 * @struct bmapnode
 * @brief bmapnode
 *
 */
typedef struct bmapnode {
    bmapkey_t key;   /**< key */
    bvalue_t value;  /**< value */
} bmapnode_t;        /**< bmapnode_t */

/**
 * @struct bmap
 * @brief bmap
 *
 */
struct bmap {
    bcommon_header;       /**< bcommon_header */
    bgcobject_t *gray;    /**< for gc gray list */
    bmapnode_t *slots;    /**< slots */
    bmapnode_t *lastfree; /**< lastfree */
    int size;             /**< size */
    int count;            /**< count */
#ifdef __cplusplus
    BE_CONSTEXPR bmap_t(bmapnode_t *s, int n) :
        next(0), type(BE_MAP), marked(GC_CONST), gray(0),
        slots(s), lastfree(0), size(n), count(n) {} /**<  */
#endif
};

typedef bmapnode_t *bmapiter_t; /**< bmapiter_t */

#define be_map_iter()       NULL           /**< be_map_iter */
#define be_map_count(map)   ((map)->count) /**< be_map_count */
#define be_map_size(map)    (map->size)    /**< be_map_size */

/**
 * @def be_map_key2value
 * @brief be_map_key2value
 *
 */
#define be_map_key2value(dst, node) do { \
    (dst)->type = (node)->key.type;      \
    (dst)->v = (node)->key.v;            \
} while (0);

/**
 * @fn bmap_t be_map_new*(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
bmap_t* be_map_new(bvm_t *vm);

/**
 * @fn void be_map_delete(bvm_t*, bmap_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 */
void be_map_delete(bvm_t *vm, bmap_t *map);

/**
 * @fn bvalue_t be_map_find*(bvm_t*, bmap_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @return (???)
 */
bvalue_t* be_map_find(bvm_t *vm, bmap_t *map, bvalue_t *key);

/**
 * @fn bvalue_t be_map_insert*(bvm_t*, bmap_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @param value (???)
 * @return (???)
 */
bvalue_t* be_map_insert(bvm_t *vm, bmap_t *map, bvalue_t *key, bvalue_t *value);

/**
 * @fn int be_map_remove(bvm_t*, bmap_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @return (???)
 */
int be_map_remove(bvm_t *vm, bmap_t *map, bvalue_t *key);

/**
 * @fn bvalue_t be_map_findstr*(bvm_t*, bmap_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @return (???)
 */
bvalue_t* be_map_findstr(bvm_t *vm, bmap_t *map, bstring_t *key);

/**
 * @fn bvalue_t be_map_insertstr*(bvm_t*, bmap_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @param value (???)
 * @return (???)
 */
bvalue_t* be_map_insertstr(bvm_t *vm, bmap_t *map, bstring_t *key, bvalue_t *value);

/**
 * @fn void be_map_removestr(bvm_t*, bmap_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 */
void be_map_removestr(bvm_t *vm, bmap_t *map, bstring_t *key);

/**
 * @fn bmapnode_t be_map_next*(bmap_t*, bmapiter_t*)
 * @brief (???)
 *
 * @param map (???)
 * @param iter (???)
 * @return (???)
 */
bmapnode_t* be_map_next(bmap_t *map, bmapiter_t *iter);

/**
 * @fn bmapnode_t be_map_val2node*(bvalue_t*)
 * @brief (???)
 *
 * @param value (???)
 * @return (???)
 */
bmapnode_t* be_map_val2node(bvalue_t *value);

/**
 * @fn void be_map_compact(bvm_t*, bmap_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 */
void be_map_compact(bvm_t *vm, bmap_t *map);

#endif
