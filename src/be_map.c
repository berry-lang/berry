/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_map.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_class.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_exec.h"
#include <string.h>

#define key(node)           (&(node)->key)
#define value(node)         (&(node)->value)
#define isnil(node)         var_isnil(key(node))
#define setnil(node)        var_setnil(key(node))
#define hash2slot(m, h)     ((m)->slots + (h) % (m)->size)
#define hashcode(_v)        _hashcode(vm, (_v)->type, (_v)->v)
#define keytype(key)        ((signed char)(key)->type)

#define next(node)          ((node)->key.next)
#define pos2slot(map, n)    ((n) != LASTNODE ? ((map)->slots + (n)) : NULL)
#define pos(map, node)      ((int)((node) - (map)->slots))
#define setkey(node, _v)    { (node)->key.type = (bbyte_t)(_v)->type; \
                              (node)->key.v = (_v)->v; }

#define datasize(size)      ((size) * sizeof(bmapnode_t))

#define LASTNODE            ((1 << 24) - 1)

static int map_nextsize(int size)
{
    be_assert(size < LASTNODE);
    if (size < LASTNODE) {
        return be_nextsize(size);
    }
    return LASTNODE + 1;
}

static uint32_t hashptr(void *p)
{
    uintptr_t i = (uintptr_t)p;
    return (uint32_t)((i ^ (i >> 16)) & 0xFFFFFFFF);
}

#if BE_USE_SINGLE_FLOAT
#define hashreal(v)     ((uint32_t)((v).i))
#else
static uint32_t hashreal(union bvaldata v)
{
    union { breal_t r; uint32_t i[2]; } u;
    u.r = v.r;
    return u.i[0] ^ u.i[1];
}
#endif

#if BE_USE_OVERLOAD_HASH
static uint32_t hashins(bvm_t *vm, binstance_t *obj)
{
    int type = be_instance_member(vm, obj, str_literal(vm, "hash"), vm->top);
    if (basetype(type) == BE_FUNCTION) {
        bvalue_t *top = vm->top;
        var_setinstance(top + 1, obj);
        vm->top += 2;
        be_dofunc(vm, top, 1); /* call method 'item' */
        vm->top -= 2;
        if (!var_isint(vm->top)) { /* check the return value */
            const char *name = str(be_instance_name(obj));
            be_raise(vm, "runtime_error", be_pushfstring(vm,
                "the value of `%s::hash()` is not a 'int'",
                strlen(name) ? name : "<anonymous>"));
        }
        return (uint32_t)var_toint(vm->top);
    }
    return hashptr(obj);
}
#endif

static uint32_t _hashcode(bvm_t *vm, int type, union bvaldata v)
{
    (void)vm;
    switch (type) {
    case BE_NIL: return 0;
    case BE_BOOL: return (uint32_t)v.b;
    case BE_INT: return (uint32_t)v.i;
    case BE_REAL: return hashreal(v);
    case BE_STRING: return be_strhash(v.s);
#if BE_USE_OVERLOAD_HASH
    case BE_INSTANCE: return hashins(vm, v.p);
#endif
    default: return hashptr(v.p);
    }
}

static int eqnode(bvm_t *vm, bmapnode_t *node, bvalue_t *key, uint32_t hash)
{
    (void)vm;
    if (!var_isnil(key)) {
        bmapkey_t *k = key(node);
#if BE_USE_OVERLOAD_HASH
        if (var_isinstance(key)) {
            bvalue_t kv;
            kv.type = k->type;
            kv.v = k->v;
            return be_vm_iseq(vm, key, &kv);
        }
#endif
        if(keytype(k) == key->type && hashcode(k) == hash) {
            switch (key->type) {
            case BE_INT: return var_toint(key) == var_toint(k);
            case BE_REAL: return var_toreal(key) == var_toreal(k);
            case BE_STRING: return be_eqstr(var_tostr(key), var_tostr(k));
            default: return var_toobj(key) == var_toobj(k);
            }
        }
    }
    return 0;
}

static bmapnode_t* findprev(bmap_t *map, bmapnode_t *list, bmapnode_t *slot)
{
    int n, pos = pos(map, slot);
    bmapnode_t *prev = list;
    for (;;) {
        n = next(prev);
        if (n == pos || n == LASTNODE) {
            break;
        }
        prev = map->slots + n;
    }
    return n == pos ? prev : NULL;
}

static bmapnode_t* nextfree(bmap_t *map)
{
    bmapnode_t *base = map->slots;
    while (map->lastfree >= base) {
        if (isnil(map->lastfree)) {
            return map->lastfree;
        }
        --map->lastfree;
    }
    return NULL;
}

static bmapnode_t* insert(bvm_t *vm, bmap_t *map, bvalue_t *key, uint32_t hash)
{
    bmapnode_t *slot = hash2slot(map, hash);
    if (isnil(slot)) { /* empty slot */
        setkey(slot, key);
        next(slot) = LASTNODE;
    } else {
        uint32_t h = hashcode(key(slot)); /* get the hashcode of the exist node */
        bmapnode_t *mainslot = hash2slot(map, h); /* get the main-slot */
        bmapnode_t *new = nextfree(map); /* get a free slot */
        if (mainslot == slot) { /* old is main slot */
            /* insert in first */
            setkey(new, key);
            next(new) = next(slot);
            next(slot) = pos(map, new);
            slot = new;
        } else {
            bmapnode_t *prev = findprev(map, mainslot, slot);
            next(prev) = pos(map, new); /* link the previous node */
            *new = *slot; /* copy to new slot */
            setkey(slot, key);
            next(slot) = LASTNODE;
        }
    }
    return slot;
}

static bmapnode_t* find(bvm_t *vm, bmap_t *map, bvalue_t *key, uint32_t hash)
{
    bmapnode_t *slot = hash2slot(map, hash);
    if (isnil(slot)) {
        return NULL;
    }
    while (!eqnode(vm, slot, key, hash)) {
        int n = next(slot);
        if (n == LASTNODE) {
            return NULL;
        }
        slot = map->slots + n;
    }
    return slot;
}

static void resize(bvm_t *vm, bmap_t *map, int size)
{
    int i, oldsize;
    bmapnode_t *slots, *oldslots;
    if (size < map->count) {
        return;
    }
    oldsize = map->size;
    oldslots = map->slots;
    slots = be_malloc(vm, datasize(size));
    for (i = 0; i < size; ++i) {
        setnil(slots + i);
        next(slots + i) = LASTNODE;
    }
    map->size = size;
    map->slots = slots;
    map->lastfree = slots + size - 1;
    /* rehash */
    for (i = 0; i < oldsize; ++i) {
        bmapnode_t *node = oldslots + i;
        if (!isnil(node)) {
            bvalue_t v;
            bmapnode_t *newslot;
            v.type = node->key.type;
            v.v = node->key.v;
            newslot = insert(vm, map, &v, hashcode(&v));
            newslot->value = node->value;
        }
    }
    be_free(vm, oldslots, datasize(oldsize));
}

bmap_t* be_map_new(bvm_t *vm)
{
    bgcobject_t *gco = be_gcnew(vm, BE_MAP, bmap_t);
    bmap_t *map = cast_map(gco);
    if (map) {
        map->size = 0;
        map->count = 0;
        map->slots = NULL;
        var_setmap(vm->top, map);
        be_incrtop(vm);
        resize(vm, map, 2);
        be_stackpop(vm, 1);
    }
    return map;
}

void be_map_delete(bvm_t *vm, bmap_t *map)
{
    be_free(vm, map->slots, datasize(map->size));
    be_free(vm, map, sizeof(bmap_t));
}

bvalue_t* be_map_find(bvm_t *vm, bmap_t *map, bvalue_t *key)
{
    bmapnode_t *entry = find(vm, map, key, hashcode(key));
    return entry ? value(entry) : NULL;
}

bvalue_t* be_map_insert(bvm_t *vm, bmap_t *map, bvalue_t *key, bvalue_t *value)
{
    uint32_t hash = hashcode(key);
    bmapnode_t *entry = find(vm, map, key, hash);
    if (!entry) { /* new entry */
        if (map->count >= map->size) {
            resize(vm, map, map_nextsize(map->size));
        }
        entry = insert(vm, map, key, hash);
        ++map->count;
    }
    if (value) {
        entry->value = *value;
    }
    return value(entry);
}

int be_map_remove(bvm_t *vm, bmap_t *map, bvalue_t *key)
{
    uint32_t hash = hashcode(key);
    bmapnode_t *slot = hash2slot(map, hash); /* main slot */

    if (eqnode(vm, slot, key, hash)) { /* first node */
        bmapnode_t *next = pos2slot(map, next(slot));
        if (next) { /* has next */
            *slot = *next; /* first: copy the second node to the slot */
            slot = next; /* second: set the second node to nil (empty) */
        }
    } else { /* the node will be remove is not first-node */
        bmapnode_t *prev = slot;
        for (;;) { /* find the previous node */
            int n = next(prev);
            slot = pos2slot(map, n);
            if (slot == NULL) { /* node not found */
                return bfalse;
            }
            if (eqnode(vm, slot, key, hash)) {
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

bvalue_t* be_map_findstr(bvm_t *vm, bmap_t *map, bstring_t *key)
{
    bvalue_t v;
    var_setstr(&v, key);
    return be_map_find(vm, map, &v);
}

bvalue_t* be_map_insertstr(bvm_t *vm, bmap_t *map, bstring_t *key, bvalue_t *value)
{
    bvalue_t v;
    var_setstr(&v, key);
    set_fixed(key);
    bvalue_t * r = be_map_insert(vm, map, &v, value);
    restore_fixed(key);
    return r;
}

void be_map_removestr(bvm_t *vm, bmap_t *map, bstring_t *key)
{
    bvalue_t v;
    var_setstr(&v, key);
    be_map_remove(vm, map, &v);
}

bmapnode_t* be_map_next(bmap_t *map, bmapiter_t *iter)
{
    bmapnode_t *end = map->slots + map->size;
    *iter = *iter ? *iter + 1 : map->slots;
    while (*iter < end && isnil(*iter)) {
        ++(*iter);
    }
    return *iter < end ? *iter : NULL;
}

bmapnode_t* be_map_val2node(bvalue_t *value)
{
    return (bmapnode_t *)((size_t)value - sizeof(bmapkey_t));
}

void be_map_compact(bvm_t *vm, bmap_t *map)
{
    (void)vm;
    if (!gc_isconst(map)) {
        resize(vm, map, map->count ? map->count : 1);
    }
}
