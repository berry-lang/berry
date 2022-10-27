/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_string.h"
#include "be_vm.h"
#include "be_mem.h"
#include "be_constobj.h"
#include <string.h>

#define next(_s)    cast(void*, cast(bstring_t*, (_s)->next))
#define sstr(_s)    cast(char*, cast(bsstring_t*, _s) + 1)
#define lstr(_s)    cast(char*, cast(blstring_t*, _s) + 1)
#define cstr(_s)    (cast(bcstring_t*, _s)->s)

#define be_define_const_str(_name, _s, _hash, _extra, _len, _next) \
    BERRY_LOCAL const bcstring_t be_const_str_##_name = {            \
        .next = (bgcobject_t *)_next,                                \
        .type = BE_STRING,                                         \
        .marked = GC_CONST,                                        \
        .extra = _extra,                                           \
        .slen = _len,                                              \
        .hash = _hash,                                             \
        .s = _s                                                    \
    }

/* const string table */
struct bconststrtab {
    const bstring_t* const *table;
    int count; /* string count */
    int size;
};

#if BE_USE_PRECOMPILED_OBJECT
#include "../generate/be_const_strtab_def.h"
#endif

int be_eqstr(bstring_t *s1, bstring_t *s2)
{
    int slen;
    if (s1 == s2) { /* short string or the same string */
        return 1;
    }
    slen = s1->slen;
    /* discard different lengths */
    if (slen != s2->slen) {
        return 0;
    }
    /* long string */
    if (slen == 255) {  /* s2->slen is also 255 */
        blstring_t *ls1 = cast(blstring_t*, s1);
        blstring_t *ls2 = cast(blstring_t*, s2);
        return ls1->llen == ls2->llen && !strcmp(lstr(ls1), lstr(ls2));
    }
    /* const short strings */
    if (gc_isconst(s1) || gc_isconst(s2)) { /* one of the two string is short const */
        uint32_t hash1 = cast(bcstring_t*, s1)->hash;
        uint32_t hash2 = cast(bcstring_t*, s2)->hash;
        if (hash1 && hash2 && hash1 != hash2) {
            return 0; /* if hash differ, since we know both are non-null */
        }
        /* if hash are equals, there might be a chance that they are different */
        /* This can happen with solidified code that a same string is present more than once */
        /* so just considering that two strings with the same hash must be same pointer, this is no more true */
        return !strcmp(str(s1), str(s2));
    }

    /* if both strings are in-memory, they can't be equal without having the same pointer */
    return 0;
}

static void resize(bvm_t *vm, int size)
{
    int i;
    struct bstringtable *tab = &vm->strtab;
    if (size > tab->size) {
        tab->table = be_realloc(vm, tab->table,
            tab->size * sizeof(bstring_t*), size * sizeof(bstring_t*));
        for (i = tab->size; i < size; ++i) {
            tab->table[i] = NULL;
        }
    }
    for (i = 0; i < tab->size; ++i) { /* rehash */
        bstring_t *p = tab->table[i];
        tab->table[i] = NULL;
        while (p) { /* for each node in the list */
            bstring_t *hnext = next(p);
            uint32_t hash = be_strhash(p) & (size - 1);
            p->next = cast(void*, tab->table[hash]);
            tab->table[hash] = p;
            p = hnext;
        }
    }
    if (size < tab->size) {
        for (i = size; i < tab->size; ++i) {
            tab->table[i] = NULL;
        }
        tab->table = be_realloc(vm, tab->table,
            tab->size * sizeof(bstring_t*), size * sizeof(bstring_t*));
    }
    tab->size = size;
}

static void free_sstring(bvm_t *vm, bstring_t *str)
{
    be_free(vm, str, sizeof(bsstring_t) + str->slen + 1);
}

/* FNV-1a Hash */
static uint32_t str_hash(const char *str, size_t len)
{
    uint32_t hash = 2166136261u;
    be_assert(str || !len);
    while (len--) {
        hash = (hash ^ (unsigned char)*str++) * 16777619u;
    }
    return hash;
}

void be_string_init(bvm_t *vm)
{
    resize(vm, 8);
#if !BE_USE_PRECOMPILED_OBJECT
    /* the destructor name deinit needs to exist all the time, to ensure
     * that it does not need to be created when the heap is exhausted. */
    be_gc_fix(vm, cast(bgcobject_t*, str_literal(vm, "deinit")));
#endif
    /* be_const_str_deinit --> for precompiled */
}

void be_string_deleteall(bvm_t *vm)
{
    int i;
    struct bstringtable *tab = &vm->strtab;
    for (i = 0; i < tab->size; ++i) {
        bstring_t *node = tab->table[i];
        while (node) {
            bstring_t *next = next(node);
            free_sstring(vm, node);
            node = next;
        }
    }
    be_free(vm, tab->table, tab->size * sizeof(bstring_t*));
}

static bstring_t* createstrobj(bvm_t *vm, size_t len, int islong)
{
    size_t size = (islong ? sizeof(blstring_t)
                : sizeof(bsstring_t)) + len + 1;
    bgcobject_t *gco = be_gc_newstr(vm, size, islong);
    bstring_t *s = cast_str(gco);
    if (s) {
        s->slen = islong ? 255 : (bbyte_t)len;
        char *str = cast(char *, islong ? lstr(s) : sstr(s));
        str[len] = '\0';
    }
    return s;
}

#if BE_USE_PRECOMPILED_OBJECT
static bstring_t* find_conststr(const char *str, size_t len)
{
    const struct bconststrtab *tab = &m_const_string_table;
    uint32_t hash = str_hash(str, len);
    bcstring_t *s = (bcstring_t*)tab->table[hash % tab->size];
    for (; s != NULL; s = next(s)) {
        if (len == s->slen && !strncmp(str, s->s, len)) {
            return (bstring_t*)s;
        }
    }
    return NULL;
}
#endif

static bstring_t* newshortstr(bvm_t *vm, const char *str, size_t len)
{
    bstring_t *s;
    int size = vm->strtab.size;
    uint32_t hash = str_hash(str, len);
    bstring_t **list = vm->strtab.table + (hash & (size - 1));

    for (s = *list; s != NULL; s = next(s)) {
        if (len == s->slen && !strncmp(str, sstr(s), len)) {
            return s;
        }
    }
    s = createstrobj(vm, len, 0);
    if (s) {
        memcpy(cast(char *, sstr(s)), str, len);
        s->extra = 0;
        s->next = cast(void*, *list);
#if BE_USE_STR_HASH_CACHE
        cast(bsstring_t*, s)->hash = hash;
#endif
        *list = s;
        vm->strtab.count++;
        if (vm->strtab.count > size << 2) {
            resize(vm, size << 1);
        }
    }
    return s;
}

bstring_t* be_newlongstr(bvm_t *vm, const char *str, size_t len)
{
    bstring_t *s;
    blstring_t *ls;
    s = createstrobj(vm, len, 1);
    ls = cast(blstring_t*, s);
    s->extra = 0;
    ls->llen = cast_int(len);
    if (str) { /* if the argument 'str' is NULL, we just allocate space */
        memcpy(cast(char *, lstr(s)), str, len);
    }
    return s;
}

bstring_t* be_newstr(bvm_t *vm, const char *str)
{
    return be_newstrn(vm, str, strlen(str));
}

bstring_t *be_newstrn(bvm_t *vm, const char *str, size_t len)
{
    if (len <= SHORT_STR_MAX_LEN) {
#if BE_USE_PRECOMPILED_OBJECT
        bstring_t *s = find_conststr(str, len);
        return s ? s : newshortstr(vm, str, len);
#else
        return newshortstr(vm, str, len);
#endif
    }
    return be_newlongstr(vm, str, len); /* long string */
}

void be_gcstrtab(bvm_t *vm)
{
    struct bstringtable *tab = &vm->strtab;
    int size = tab->size, i;
    for (i = 0; i < size; ++i) {
        bstring_t **list = tab->table + i;
        bstring_t *prev = NULL, *node, *next;
        for (node = *list; node; node = next) {
            next = next(node);
            if (!gc_isfixed(node) && gc_iswhite(node)) {
                free_sstring(vm, node);
                tab->count--;
                if (prev) { /* link list */
                    prev->next = cast(void*, next);
                } else {
                    *list = next;
                }
            } else {
                prev = node;
                gc_setwhite(node);
            }
        }
    }
    if (tab->count < size >> 2 && size > 8) {
        resize(vm, size >> 1);
    }
}

uint32_t be_strhash(const bstring_t *s)
{
    if (gc_isconst(s)) {
        bcstring_t* cs = cast(bcstring_t*, s);
        if (cs->hash) {  /* if hash is null we need to compute it */
            return cs->hash;
        } else {
            return str_hash(cstr(s), str_len(s));
        }
    }
#if BE_USE_STR_HASH_CACHE
    if (s->slen != 255) {
        return cast(bsstring_t*, s)->hash;
    }
#endif
    return str_hash(str(s), str_len(s));
}

const char* be_str2cstr(const bstring_t *s)
{
    be_assert(cast_str(s) != NULL);
    if (gc_isconst(s)) {
        return cstr(s);
    }
    if (s->slen == 255) {
        return lstr(s);
    }
    return sstr(s);
}

void be_str_setextra(bstring_t *s, int extra)
{
    if (!gc_isconst(s)) {
        s->extra = cast(bbyte_t, extra);
    }
}
