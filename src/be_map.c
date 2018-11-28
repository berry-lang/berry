#include "be_map.h"
#include "be_string.h"
#include "be_mem.h"
#include "be_gc.h"

#define MAP_DEF_SIZE        4

#define key(entry)          (&(entry)->key)
#define value(entry)        (&(entry)->value)
#define isnil(entry)        var_isnil(key(entry))
#define setnil(entry)       var_setnil(key(entry))
#define hash2slot(m, h)     ((m)->slots + ((h) & ((m)->size - 1)))
#define is_mainslot(m, s)   (hash2slot(m, hashcode(&(s)->key)) == (s))
#define alloc_slot(m)       freelist_remove(m, m->freelist)
#define init_slot(s)        { (s)->prev = NULL; (s->next) = NULL; }

uint32_t hashptr(bvalue *v)
{
    uintptr_t i = (uintptr_t)v->v.p;
    return (uint32_t)((i ^ (i >> 16)) & 0xFFFFFFFF);
}

static uint32_t hashcode(bvalue *key)
{
    switch (key->type) {
    case BE_NIL:
        return 0;
    case BE_BOOL:
        return (uint32_t)key->v.b;
    case BE_INT:
        return (uint32_t)key->v.i;
    case BE_REAL:
        return (uint32_t)key->v.i; /* test */
    case BE_STRING:
        return str_hash(key->v.s);
    default:
        return hashptr(key);
    }
}

static int eq_entry(bmapentry *entry, bvalue *key, uint32_t hash)
{
    bvalue *ekey = &entry->key;
    if (hashcode(key(entry)) == hash && ekey->type == key->type) {
        switch (key->type) {
        case BE_NIL:
            return 0;
        case BE_BOOL:
            return key->v.b == ekey->v.b;
        case BE_INT:
            return key->v.b == ekey->v.b;
        case BE_REAL:
            return key->v.r == ekey->v.r;
        case BE_STRING:
            return be_eqstr(key->v.s, ekey->v.s);
        default:
            return key->v.p == ekey->v.p;
        }
    }
    return 0;
}

static bmapentry* insert(bmap *map, bvalue *key, uint32_t hash)
{
    bmapentry **slot = hash2slot(map, hash);
    bmapentry *node = be_malloc(sizeof(bmapentry));
    node->next = *slot; /* insert to head */
    *slot = node;
    node->key = *key;
    return node;
}

static bmapentry* find(bmap *map, bvalue *key, uint32_t hash)
{
    bmapentry *slot = *hash2slot(map, hash);
    while (slot && !eq_entry(slot, key, hash)) {
        slot = slot->next;
    }
    return slot;
}

static void resize(bmap *map, int size)
{
    bmapentry **slot, **end, **old_slots = map->slots;
    if (size < map->count) {
        return;
    }
    map->size = size;
    map->slots = be_malloc(sizeof(bmapentry*) * size);
    slot = map->slots;
    for (end = slot + size; slot < end; ++slot) {
        *slot = NULL;
    }
    /* rehash */
    slot = old_slots;
    for (end = slot + map->count; slot < end; ++slot) {
        bmapentry *node = *slot;
        while (node) {
            bmapentry **ns = hash2slot(map, hashcode(key(node)));
            bmapentry *next = node->next;
            node->next = *ns; /* insert to head */
            *ns = node;
            node = next;
        }
    }
    be_free(old_slots);
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
    int i, size = map->size;
    for (i = 0; i < size; ++i) {
        bmapentry *node, *next;
        for (node = map->slots[i]; node; node = next) {
            next = node->next;
            be_free(node);
        }
    }
    be_free(map->slots);
    be_free(map);
}

bvalue* be_map_find(bmap *map, bvalue *key)
{
    bmapentry *entry = find(map, key, hashcode(key));
    return entry ? value(entry) : NULL;
}

bvalue* be_map_insert(bmap *map, bvalue *key, bvalue *value)
{
    uint32_t hash = hashcode(key);
    bmapentry *entry = find(map, key, hash);
    if (!entry) { /* new entry */
        if (map->count > map->size << 1) {
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
    bmapentry *entry = find(map, key, hash);
    if (entry) {
        /* remove node from list */
        bmapentry **slot = hash2slot(map, hash), *node;
        node = *slot;
        if (node == entry) { /* remove first */
            *slot = entry->next;
        } else {
            while (node->next != entry) {
                node = node->next;
            }
            node->next = entry->next;
        }
        be_free(entry);
        --map->count;
        if (map->count < map->size && map->size > MAP_DEF_SIZE) {
            resize(map, map->size >> 1);
        }
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

bmapentry* be_map_next(bmap *map, bmapiter *iter)
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
