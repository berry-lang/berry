#include "be_map.h"
#include "be_string.h"
#include "be_mem.h"
#include "be_gc.h"

#define MAP_DEF_SIZE        4

#define key(node)           (&(node)->key)
#define value(node)         (&(node)->value)
#define isnil(node)         var_isnil(key(node))
#define setnil(node)        var_setnil(key(node))
#define hash2slot(m, h)     ((m)->slots + (h) % (m)->size)
#define hashcode(_v)        _hashcode_((_v)->type, (_v)->v)

#define next(node)          ((node)->key.next)
#define pos(map, node)      ((node) - (map)->slots)
#define setkey(node, _v)    { (node)->key.type = (_v)->type; \
                              (node)->key.v = (_v)->v; }

#define LASTNODE            65535

uint32_t hashptr(void *p)
{
    uintptr_t i = (uintptr_t)p;
    return (uint32_t)((i ^ (i >> 16)) & 0xFFFFFFFF);
}

static uint32_t _hashcode_(int type, union bvaldata v)
{
    switch (type) {
    case BE_NIL:
        return 0;
    case BE_BOOL:
        return (uint32_t)v.b;
    case BE_INT:
        return (uint32_t)v.i;
    case BE_REAL:
        return (uint32_t)v.i; /* test */
    case BE_STRING:
        return str_hash(v.s);
    default:
        return hashptr(v.p);
    }
}

static int eq_entry(bmapnode *node, bvalue *key, uint32_t hash)
{
    bmapkey *k = key(node);
    if (hashcode(key(node)) == hash && k->type == key->type) {
        switch (key->type) {
        case BE_NIL:
            return 0;
        case BE_BOOL:
            return key->v.b == k->v.b;
        case BE_INT:
            return key->v.b == k->v.b;
        case BE_REAL:
            return key->v.r == k->v.r;
        case BE_STRING:
            return be_eqstr(key->v.s, k->v.s);
        default:
            return key->v.p == k->v.p;
        }
    }
    return 0;
}

static bmapnode* insert(bmap *map, bvalue *key, uint32_t hash)
{
    bmapnode *slot = hash2slot(map, hash);
    if (isnil(slot)) { /* empty slot */
        setslot:
        setkey(slot, key);
        next(slot) = LASTNODE;
    } else {
        uint32_t hash = hashcode(key(slot));
        bmapnode *new = map->freelist; /* get a next slot */
        int n = next(new);
        map->freelist = n != LASTNODE ? (map->slots + n) : NULL;
        if (hash2slot(map, hash) == slot) { /* old is main slot */
            /* insert in first */
            setkey(new, key);
            next(new) = next(slot);
            next(slot) = new - map->slots;
            slot = new;
        } else {
            *new = *slot; /* copy to new slot */
            goto setslot;
        }
    }
    return slot;
}

static bmapnode* find(bmap *map, bvalue *key, uint32_t hash)
{
    bmapnode *slot = hash2slot(map, hash);
    if (isnil(slot)) {
        return NULL;
    }
    while (!eq_entry(slot, key, hash)) {
        int n = next(slot);
        if (n == LASTNODE) {
            return NULL;
        }
        slot = map->slots + n;
    }
    return slot;
}

static void resize(bmap *map, int size)
{
    int i;
    bmapnode *slots;
    if (size < map->count) {
        return;
    }
    slots = be_malloc(sizeof(bmapnode) * size);
    for (i = 0; i < size; ++i) {
        next(slots + i) = i + 1;
    }
    next(slots + size - 1) = LASTNODE;
    i = map->size;
    map->size = size;
    map->freelist = slots;
    slots = map->slots;
    map->slots = map->freelist;
    size = i;
    /* rehash */
    for (i = 0; i < size; ++i) {
        bmapnode *node = map->slots + i;
        if (!isnil(node)) {
            bvalue v;
            v.type = node->key.type;
            v.v = node->key.v;
            insert(map, &v, hashcode(&v));
        }
    }
    be_free(slots);
}

bmap* be_map_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_MAP, bmap);
    bmap *map = cast_map(gco);
    if (map) {
        map->size = 0;
        map->count = 0;
        map->slots = NULL;
        resize(map, MAP_DEF_SIZE);
    }
    return map;
}

void be_map_delete(bmap *map)
{
    be_free(map->slots);
    be_free(map);
}

bvalue* be_map_find(bmap *map, bvalue *key)
{
    bmapnode *entry = find(map, key, hashcode(key));
    return entry ? value(entry) : NULL;
}

bvalue* be_map_insert(bmap *map, bvalue *key, bvalue *value)
{
    uint32_t hash = hashcode(key);
    bmapnode *entry = find(map, key, hash);
    if (!entry) { /* new entry */
        if (map->count > map->size) {
            resize(map, map->size << 1);
        }
        entry = insert(map, key, hash);
        ++map->count;
    }
    if (value) {
        entry->value = *value;
    }
    return value(entry);
}

void be_map_remove(bmap *map, bvalue *key)
{
    uint32_t hash = hashcode(key);
    bmapnode *slot = hash2slot(map, hash); /* main slot */

    if (eq_entry(slot, key, hash)) { /* first node */
        int n = next(slot);
        if (n != LASTNODE) { /* has next ? */
            bmapnode *next = map->slots + n;
            *slot = map->slots[n];
        }
        /* add to free list */
        next(slot) = map->freelist ? pos(map, map->freelist) : LASTNODE;
        map->freelist = slot;
    }
    --map->count;
    if (map->count >> 1 < map->size && map->size > MAP_DEF_SIZE) {
        resize(map, map->size >> 1);
    }
}

bvalue* be_map_findstr(bmap *map, bstring *key)
{
    bvalue v;
    var_setstr(&v, key);
    return be_map_find(map, &v);
}

bvalue* be_map_insertstr(bmap *map, bstring *key, bvalue *value)
{
    bvalue v;
    var_setstr(&v, key);
    return be_map_insert(map, &v, value);
}

void be_map_removestr(bmap *map, bstring *key)
{
    bvalue v;
    var_setstr(&v, key);
    be_map_remove(map, &v);
}

bmapiter be_map_iter(void)
{
    bmapiter iter;
    iter.slotidx = -1;
    iter.node = NULL;
    return iter;
}

bmapnode* be_map_next(bmap *map, bmapiter *iter)
{
    if (iter->node) {
        iter->node = iter->node->next;
        if (iter->node == NULL) {
            goto nextslot;
        }
    } else {
        nextslot:
        do {
            if (++iter->slotidx >= map->size) {
                return NULL;
            }
            iter->node = map->slots[iter->slotidx];
        } while (iter->node == NULL);
    }
    return iter->node;
}
