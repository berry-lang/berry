#include "be_string.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_mem.h"
#include "be_gc.h"
#include <string.h>

#define SHORT_STR_MAX_LEN   64

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
        return s1->hash == s2->hash && !strcmp(s1->s, s2->s);
    }
    return 0;
}

static void strtab_resize(bvm *vm, int size)
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
            bstring *hnext = p->u.next;
            uint32_t hash = p->hash & (size - 1);
            p->u.next = tab->table[hash];
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
static uint32_t string_hash(const char *str, int len)
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
    strtab_resize(vm, 16);
}

bstring* createstrobj(bvm *vm, int len, uint32_t hash)
{
    int size = sizeof(bstring) + len;
    bgcobject *gco = be_newgcobj(vm, VT_STRING, size);
    bstring *s = cast_str(gco);
    if (s) {
        s->hash = hash;
        s->s[len] = '\0';
        s->slen = len <= SHORT_STR_MAX_LEN ? (bbyte)len : 255;
    }
    return s;
}

static bstring* newshortstr(bvm *vm, const char *str, int len)
{
    bstring *s;
    int size = vm->strtab->size;
    uint32_t hash = string_hash(str, len);
    bstring **list = vm->strtab->table + (hash & (size - 1));

    for (s = *list; s != NULL; s = s->u.next) {
        if (hash == s->hash && !strncmp(str, s->s, len)) {
            return s;
        }
    }
    s = createstrobj(vm, len, hash);
    strncpy(s->s, str, len);
    s->extra = 0;
    s->u.next = *list;
    *list = s;
    vm->strtab->count++;
    if (vm->strtab->count >= size) {
        strtab_resize(vm, size << 1);
    }
    return s;
}

bstring* be_newstr(bvm *vm, const char *str)
{
    return be_newstrn(vm, str, (int)strlen(str));
}

bstring* be_newstrn(bvm *vm, const char *str, int len)
{
    bstring *s;
    if (len <= SHORT_STR_MAX_LEN) {
        return newshortstr(vm, str, len);
    }
    /* long string */
    s = createstrobj(vm, len, len);
    strncpy(s->s, str, len);
    s->extra = 0;
    s->u.llen = len;
    return s;
}

void be_deletestrgc(bvm *vm, bstring *str)
{
    if (str->slen < 255) { /* remove short string */
        bstringtable *strtab = vm->strtab;
        int size = strtab->size;
        bstring **list = strtab->table + (str->hash & (size - 1));
        strtab->count--;
        if (*list == str) {
            *list = str->u.next;
        } else {
            bstring *prev = *list;
            while (prev && prev->u.next != str) {
                prev = prev->u.next;
            }
            if (prev) { /* find */
                prev->u.next = str->u.next;
            }
        }
        if ((strtab->count << 1) < size && size > 16) {
            strtab_resize(vm, size >> 1);
        }
    }
    be_free(str);
}
