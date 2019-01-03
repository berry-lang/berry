#include "be_string.h"
#include "be_vm.h"
#include "be_mem.h"
#include "be_gc.h"
#include <string.h>

#define next(_s)    cast(void*, cast(bstring*, (_s)->next))

struct bstringtable {
    bstring **table;
    int count; /* string count */
    int size;
};

int be_eqstr(bstring *s1, bstring *s2)
{
    int slen;
    if (s1 == s2) { /* short string or the same string */
        return 1;
    }
    slen = s1->slen;
    /* long string */
    if (slen == 255 && slen == s2->slen) { 
        return cast(blstring*, s1)->llen == cast(blstring*, s2)->llen && !strcmp(s1->s, s2->s);
    }
    return 0;
}

static void resize(bvm *vm, int size)
{
    int i;
    bstringtable *tab = vm->strtab;
    if (size > tab->size) {
        tab->table = be_realloc(tab->table, size * sizeof(bstring*));
        for (i = tab->size; i < size; ++i) {
            tab->table[i] = NULL;
        }
    }
    for (i = 0; i < tab->size; ++i) { /* rehash */
        bstring *p = tab->table[i];
        tab->table[i] = NULL;
        while (p) { /* for each node in the list */
            bstring *hnext = next(p);
            uint32_t hash = str_hash(p) & (size - 1);
            p->next = cast(void*, tab->table[hash]);
            tab->table[hash] = p;
            p = hnext;
        }
    }
    if (size < tab->size) {
        tab->table = be_realloc(tab->table, size * sizeof(bstring*));
    }
    tab->size = size;
}

/* FNV-1a Hash */
uint32_t be_strhash(const char *str, int len)
{
    uint32_t hash = 2166136261u;
    while (len--) {
        hash = (hash ^ *str++) * 16777619u;
    }
    return hash;
}

void be_string_init(bvm *vm)
{
    vm->strtab = be_malloc(sizeof(bstringtable));
    vm->strtab->size = 0;
    vm->strtab->count = 0;
    vm->strtab->table = NULL;
    resize(vm, 8);
}

void be_string_deleteall(bvm *vm)
{
    int i;
    bstringtable *tab = vm->strtab;
    for (i = 0; i < tab->size; ++i) {
        bstring *node = tab->table[i];
        while (node) {
            bstring *next = next(node);
            be_free(node);
            node = next;
        }
    }
    be_free(tab->table);
    be_free(tab);
}

bstring* createstrobj(bvm *vm, int len, int islong, int isk)
{
    int size = (islong ? sizeof(blstring)
                : sizeof(bstring)) + (isk ? 0 : len + 1);
    bgcobject *gco = be_gc_newstr(vm, size, islong);
    bstring *s = cast_str(gco);
    if (s) {
        if (!isk) {
            char *str;
            if (islong) {
                str = cast(char*, (cast(blstring*, s) + 1));
            } else {
                str = cast(char*, (cast(bstring*, s) + 1));
            }
            str[len] = '\0';
            s->s = str;
        }
        s->slen = islong ? 255 : (bbyte)len;
    }
    return s;
}

static bstring* newshortstr(bvm *vm, const char *str, int len, int isk)
{
    bstring *s;
    int size = vm->strtab->size;
    uint32_t hash = be_strhash(str, len);
    bstring **list = vm->strtab->table + (hash & (size - 1));

    for (s = *list; s != NULL; s = next(s)) {
        if (len == s->slen && !strncmp(str, s->s, len)) {
            return s;
        }
    }
    s = createstrobj(vm, len, 0, isk);
    if (isk) {
        s->s = str;
    } else {
        strncpy((char*)s->s, str, len);
    }
    s->extra = 0;
    s->next = cast(void*, *list);
    *list = s;
    vm->strtab->count++;
    if (vm->strtab->count > size << 2) {
        resize(vm, size << 1);
    }
    return s;
}

static bstring* newlongstr(bvm *vm, const char *str, int len, int isk)
{
    bstring *s;
    blstring *ls;
    s = createstrobj(vm, len, 1, isk);
    ls = cast(blstring*, s);
    s->extra = 0;
    ls->llen = len;
    if (isk) {
        s->s = str;
    } else {
        strncpy((char*)s->s, str, len);
    }
    return s;
}

bstring* be_newstr(bvm *vm, const char *str)
{
    return be_newstrn(vm, str, (int)strlen(str));
}

bstring* be_newstrn(bvm *vm, const char *str, int len)
{
    if (len <= SHORT_STR_MAX_LEN) {
        return newshortstr(vm, str, len, 0);
    }
    return newlongstr(vm, str, len, 0); /* long string */
}

bstring* be_newconststr(bvm *vm, const char *str)
{
    int len = (int)strlen(str);
    if (len <= SHORT_STR_MAX_LEN) {
        return newshortstr(vm, str, len, 1);
    }
    return newlongstr(vm, str, len, 1); /* long string */
}

void be_gcstrtab(bvm *vm)
{
    bstringtable *strtab = vm->strtab;
    int size = strtab->size, i;
    for (i = 0; i < size; ++i) {
        bstring **list = strtab->table + i;
        bstring *prev = NULL, *node, *next;
        for (node = *list; node; node = next) {
            next = next(node);
            if (!gc_isfixed(node) && gc_iswhite(node)) {
                be_free(node);
                strtab->count--;
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
    if (strtab->count < size >> 2 && size > 8) {
        resize(vm, size >> 1);
    }
}
