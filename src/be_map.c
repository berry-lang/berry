#include "be_map.h"
#include "be_string.h"
#include "be_mem.h"
#include "be_gc.h"

#define MAP_DEF_SIZE        4

#define key(entry)          (&(entry)->key)
#define value(entry)        (&(entry)->value)
#define isnil(entry)        value_isnil(key(entry))
#define setnil(entry)       value_setnil(key(entry))
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
    case VT_NIL:
        return 0;
    case VT_BOOL:
        return (uint32_t)key->v.b;
    case VT_INT:
        return (uint32_t)key->v.i;
    case VT_REAL:
        return (uint32_t)key->v.i; /* test */
    case VT_STRING:
        return key->v.s->hash;
    default:
        return hashptr(key);
    }
}

static int eq_entry(bmapentry *entry, bvalue *key)
{
    bvalue *ekey = &entry->key;
    if (ekey->type == key->type) {
        switch (key->type) {
        case VT_NIL:
            return 0;
        case VT_BOOL:
            return key->v.b == ekey->v.b;
        case VT_INT:
            return key->v.b == ekey->v.b;
        case VT_REAL:
            return key->v.r == ekey->v.r;
        case VT_STRING:
            return be_eqstr(key->v.s, ekey->v.s);
        default:
            return key->v.p == ekey->v.p;
        }
    }
    return 0;
}

static void freelist_insert(bmap *map, bmapentry *slot)
{
    setnil(slot);
    /* insert to head */
    if (map->freelist) {
        map->freelist->prev = slot;
    }
    slot->prev = NULL;
    slot->next = map->freelist;
    map->freelist = slot;
}

static bmapentry* freelist_remove(bmap *map, bmapentry *slot)
{
    if (slot == map->freelist) { /* remove head */
        map->freelist = slot->next;
        if (map->freelist) {
            map->freelist->prev = NULL;
        }
    } else {
        slot->prev->next = slot->next;
        if (slot->next) {
            slot->next->prev = slot->prev;
        }
    }
    return slot;
}

static void moveto_newslot(bmap *map, bmapentry *slot)
{
    bmapentry *newslot = alloc_slot(map);
    newslot->prev = slot->prev;
    newslot->next = slot->next;
    if (slot->prev) {
        slot->prev->next = newslot;
    }
    if (slot->next) {
        slot->next->prev = newslot;
    }
}

static bmapentry* linkto_newslot(bmap *map, bmapentry *slot)
{
    bmapentry *newslot = alloc_slot(map);
    newslot->prev = slot;
    newslot->next = slot->next;
    if (slot->next) {
        slot->next->prev = newslot;
    }
    slot->next = newslot;
    return newslot;
}

static bmapentry* insert(bmap *map, bvalue *key, uint32_t hash)
{
    bmapentry *slot = hash2slot(map, hash);
    if (isnil(slot)) { /* insert to nil slot */
        freelist_remove(map, slot);
        init_slot(slot);
    } else {
        if (is_mainslot(map, slot)) {
            slot = linkto_newslot(map, slot);
        } else {
            moveto_newslot(map, slot);
            init_slot(slot);
        }
    }
    slot->key = *key;
    return slot;
}

static bmapentry* find(bmap *map, bvalue *key, uint32_t hash)
{
    bmapentry *slot = hash2slot(map, hash);
    if (!isnil(slot) && is_mainslot(map, slot)) {
        while (slot && (hashcode(key(slot)) != hash || !eq_entry(slot, key))) {
            slot = slot->next;
        }
        return slot;
    }
    return NULL;
}

static void resize(bmap *map, int size)
{
    bmapentry *slot, *end, *old_slots = map->slots;
    if (size < map->count) {
        return;
    }
    map->size = size;
    map->slots = be_malloc(sizeof(bmapentry) * size);
    map->freelist = NULL; /* reset free-list */
    slot = map->slots;
    for (end = slot + size; slot < end; ++slot) {
        freelist_insert(map, slot);
    }
    /* re-hash */
    slot = old_slots;
    for (end = slot + map->count; slot < end; ++slot) {
        if (!isnil(slot)) {
            bmapentry *e = insert(map, &slot->key, hashcode(key(slot)));
            e->value = slot->value;
        }
    }
    be_free(old_slots);
}

bmap* be_map_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, VT_MAP, bmap);
    bmap *map = cast_map(gco);
    if (map) {
        map->freelist = NULL;
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
    bmapentry *entry = find(map, key, hashcode(key));
    return entry ? &entry->value : NULL;
}

bvalue* be_map_insert(bmap *map, bvalue *key, bvalue *value)
{
    uint32_t hash = hashcode(key);
    bmapentry *entry = find(map, key, hash);
    if (!entry) { /* new entry */
        if (!map->freelist) {
            resize(map, map->size << 1);
        }
        entry = insert(map, key, hash);
        ++map->count;
    }
    if (value) {
        entry->value = *value;
    }
    return &entry->value;
}

void be_map_remove(bmap *map, bvalue *key)
{
    bmapentry *entry = find(map, key, hashcode(key));
    if (entry) {
        /* remove node from list */
        if (entry->prev) {
            entry->prev->next = entry->next;
        }
        if (entry->next) {
            entry->next->prev = entry->prev;
        }
        freelist_insert(map, entry); /* link node to free-list */
        --map->count;
    }
}

bvalue* be_map_findstr(bmap *map, bstring *key)
{
    bvalue v = {
        .v.s = key,
        .type = VT_STRING
    };
    return be_map_find(map, &v);
}

bvalue* be_map_insertstr(bmap *map, bstring *key, bvalue *value)
{
    bvalue v = {
        .v.s = key,
        .type = VT_STRING
    };
    return be_map_insert(map, &v, value);
}

void be_map_removestr(bmap *map, bstring *key)
{
    bvalue v = {
        .v.s = key,
        .type = VT_STRING
    };
    be_map_remove(map, &v);
}
