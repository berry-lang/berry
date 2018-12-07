#include "be_map.h"
#include "be_string.h"
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
#define pos(map, node)      ((node) - (map)->slots)
#define setkey(node, _v)    { (node)->key.type = (_v)->type; \
                              (node)->key.v = (_v)->v; }

#define LASTNODE            65535

static const uint16_t hashtab_size[] = {
    0, 2, 4, 6, 8, 10, 12, /* + 2 */
    16, 20, 25, 32, 39, 48, 61, 76, /* * 1.25 */
    96, 128, 170, 226, 300, 400, 531, 707, 940, 1250, /* * 1.33 */
    1875, 2812, 4218, 6328, 9492, 14238, 21357, 32036, 48054 /* * 1.5 */
};

static int get_nextsize(int size)
{
    size_t i;
    for (i = 0; i < array_count(hashtab_size); ++i) {
        if (hashtab_size[i] > size) {
            return hashtab_size[i];
        }
    }
    return LASTNODE;
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
        uint32_t hash = hashcode(key(slot)); /* get the hashcode of the exist node */
        bmapnode *mainslot = hash2slot(map, hash); /* get the main-slot */
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
            resize(map, get_nextsize(map->size));
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
            bmapnode *next = pos2slot(map, n);
            *slot = *next;
        }
    } else {
        for (;;) {
            int n = next(slot);
            bmapnode *next = pos2slot(map, n);
            if (next == NULL) {
                return;
            }
            if (eq_entry(slot, key, hash)) {
                break;
            }
            slot = next;
        }
    }
    /* set to nil */
    setnil(slot);
    /* add to lastfree */
    if (map->lastfree < slot) {
        map->lastfree = slot;
    }
    --map->count;
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
    bmapnode *end = map->slots + map->size;

    iter->node = iter->node ? iter->node + 1 : map->slots;
    while (isnil(iter->node)) {
        ++iter->node;
        if (iter->node >= end) {
            return NULL;
        }
    }
    return iter->node;
}

bvalue be_map_key2value(bmapnode *node)
{
    bvalue v;
    v.type = node->key.type;
    v.v = node->key.v;
    return v;
}
