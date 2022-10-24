/**
 * @file       be_map.h
 * @brief      Main berry entry API
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
} bmapkey;            /**< bmapkey */

/**
 * @struct bmapnode
 * @brief bmapnode
 *
 */
typedef struct bmapnode {
    bmapkey key;  /**< key */
    bvalue value; /**< value */
} bmapnode;       /**< bmapnode */

/**
 * @struct bmap
 * @brief bmap
 *
 */
struct bmap {
    bcommon_header;     /**< bcommon_header */
    bgcobject *gray;    /**< for gc gray list */
    bmapnode *slots;    /**< slots */
    bmapnode *lastfree; /**< lastfree */
    int size;           /**< size */
    int count;          /**< count */
#ifdef __cplusplus
    BE_CONSTEXPR bmap(bmapnode *s, int n) :
        next(0), type(BE_MAP), marked(GC_CONST), gray(0),
        slots(s), lastfree(0), size(n), count(n) {} /**<  */
#endif
};

typedef bmapnode *bmapiter; /**< bmapiter */

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
 * @fn bmap be_map_new*(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
bmap* be_map_new(bvm *vm);

/**
 * @fn void be_map_delete(bvm*, bmap*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 */
void be_map_delete(bvm *vm, bmap *map);

/**
 * @fn bvalue be_map_find*(bvm*, bmap*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @return (???)
 */
bvalue* be_map_find(bvm *vm, bmap *map, bvalue *key);

/**
 * @fn bvalue be_map_insert*(bvm*, bmap*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @param value (???)
 * @return (???)
 */
bvalue* be_map_insert(bvm *vm, bmap *map, bvalue *key, bvalue *value);

/**
 * @fn int be_map_remove(bvm*, bmap*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @return (???)
 */
int be_map_remove(bvm *vm, bmap *map, bvalue *key);

/**
 * @fn bvalue be_map_findstr*(bvm*, bmap*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @return (???)
 */
bvalue* be_map_findstr(bvm *vm, bmap *map, bstring *key);

/**
 * @fn bvalue be_map_insertstr*(bvm*, bmap*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 * @param value (???)
 * @return (???)
 */
bvalue* be_map_insertstr(bvm *vm, bmap *map, bstring *key, bvalue *value);

/**
 * @fn void be_map_removestr(bvm*, bmap*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param key (???)
 */
void be_map_removestr(bvm *vm, bmap *map, bstring *key);

/**
 * @fn bmapnode be_map_next*(bmap*, bmapiter*)
 * @brief (???)
 *
 * @param map (???)
 * @param iter (???)
 * @return (???)
 */
bmapnode* be_map_next(bmap *map, bmapiter *iter);

/**
 * @fn bmapnode be_map_val2node*(bvalue*)
 * @brief (???)
 *
 * @param value (???)
 * @return (???)
 */
bmapnode* be_map_val2node(bvalue *value);

/**
 * @fn void be_map_compact(bvm*, bmap*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 */
void be_map_compact(bvm *vm, bmap *map);

#endif
