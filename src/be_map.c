#include "be_map.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_mem.h"
#include "be_gc.h"

#define key(node)           (&(node)->key)
#define value(node)         (&(node)->value)
#define isnil(node)         var_isnil(key(node))
#define setnil(node)        var_setnil(key(node))
#define hash2slot(m, h)     ((m)->slots + (h) % (m)->size)
#define hashcode(_v)        _hashcode_((_v)->type, (_v)->v)

#define next(node)          ((node)->key.next)
#define pos2slot(map, n)    ((n) != LASTNODE ? ((map)->slots + (n)) : NULL)
#define pos(map, node)      ((uint16_t)((node) - (map)->slots))
#define setkey(node, _v)    { (node)->key.type = (bbyte)(_v)->type; \
                              (node)->key.v = (_v)->v; }

#define LASTNODE            ((1 << 24) - 1)

static int map_nextsize(int size)
{
    be_assert(size < LASTNODE);
    if (size < LASTNODE) {
        return be_nextsize(size);
    }
    return LASTNODE + 1;
}

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
        return be_strhash(v.s);
    default:
        return hashptr(v.p);
    }
}

static int eqnode(bmapnode *node, bvalue *key, uint32_t hash)
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

static bmapnode* findprev(bmap *map, bmapnode *list, bmapnode *slot)
{
    int n, pos = pos(map, slot);
    bmapnode *prev = list;
    for (;;) {
        n = next(prev);
        if (n == pos || n == LASTNODE) {
            break;
        }
        prev = map->slots + n;
    }
    return n == pos ? prev : NULL;
}

bmapnode* nextfree(bmap *map)
{
    bmapnode *base = map->slots;
    while (map->lastfree >= base) {
        if (isnil(map->lastfree)) {
            return map->lastfree;
        }
        --map->lastfree;
    }
    return NULL;
}

static bmapnode* insert(bmap *map, bvalue *key, uint32_t hash)
{
    bmapnode *slot = hash2slot(map, hash);
    if (isnil(slot)) { /* empty slot */
        setkey(slot, key);
        next(slot) = LASTNODE;
    } else {
        uint32_t h = hashcode(key(slot)); /* get the hashcode of the exist node */
        bmapnode *mainslot = hash2slot(map, h); /* get the main-slot */
        bmapnode *new = nextfree(map); /* get a free slot */
        if (mainslot == slot) { /* old is main slot */
            /* insert in first */
            setkey(new, key);
            next(new) = next(slot);
            next(slot) = pos(map, new);;
            slot = new;
        } else {
            bmapnode *prev = findprev(map, mainslot, slot);
            next(prev) = pos(map, new); /* link the previous node */
            *new = *slot; /* copy to new slot */
            setkey(slot, key);
            next(slot) = LASTNODE;
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
    while (!eqnode(slot, key, hash)) {
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
    int i, oldsize;
    bmapnode *slots, *oldslots;
    if (size < map->count) {
        return;
    }
    oldsize = map->size;
    oldslots = map->slots;
    slots = be_malloc(sizeof(bmapnode) * size);
    for (i = 0; i < size; ++i) {
        setnil(slots + i);
    }
    map->size = size;
    map->slots = slots;
    map->lastfree = slots + size - 1;
    /* rehash */
    for (i = 0; i < oldsize; ++i) {
        bmapnode *node = oldslots + i;
        if (!isnil(node)) {
            bvalue v;
            bmapnode *newslot;
            v.type = node->key.type;
            v.v = node->key.v;
            newslot = insert(map, &v, hashcode(&v));
            newslot->value = node->value;
        }
    }
    be_free(oldslots);
}

bmap* be_map_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_MAP, bmap);
    bmap *map = cast_map(gco);
    if (map) {
        map->size = 0;
        map->count = 0;
        map->slots = NULL;
        resize(map, 2);
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
        if (map->count >= map->size) {
            resize(map, map_nextsize(map->size));
        }
        entry = insert(map, key, hash);
        ++map->count;
    }
    if (value) {
        entry->value = *value;
    }
    return value(entry);
}

int be_map_remove(bmap *map, bvalue *key)
{
    uint32_t hash = hashcode(key);
    bmapnode *slot = hash2slot(map, hash); /* main slot */

    if (eqnode(slot, key, hash)) { /* first node */
        bmapnode *next = pos2slot(map, next(slot));
        if (next) { /* has next */
            *slot = *next; /* first: copy the second node to the slot */
            slot = next; /* second: set the second node to nil (empty) */
        }
    } else { /* the node will be remove is not first-node */
        bmapnode *prev = slot;
        for (;;) { /* find the previous node */
            int n = next(prev);
            slot = pos2slot(map, n);
            if (slot == NULL) { /* node not found */
                return bfalse;
            }
            if (eqnode(slot, key, hash)) {
                break;
            }
            prev = slot;
        }
        /* link the list */
        next(prev) = next(slot);
    }
    /* set to nil */
    setnil(slot);
    /* add to lastfree */
    if (map->lastfree < slot) {
        map->lastfree = slot;
    }
    --map->count;
    return btrue;
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

bmapnode* be_map_next(bmap *map, bmapiter *iter)
{
    bmapnode *end = map->slots + map->size;
    *iter = *iter ? *iter + 1 : map->slots;
    while (*iter < end && isnil(*iter)) {
        ++(*iter);
    }
    return *iter < end ? *iter : NULL;
}

bvalue be_map_key2value(bmapnode *node)
{
    bvalue v;
    v.type = node->key.type;
    v.v = node->key.v;
    return v;
}

void be_map_release(bvm *vm, bmap *map)
{
    (void)vm;
    resize(map, map->count ? map->count : 1);
}
