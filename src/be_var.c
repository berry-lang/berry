/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_var.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_string.h"
#include "be_map.h"
#include "be_gc.h"
#include "be_class.h"
#include <string.h>

#define global(vm)      ((vm)->gbldesc.global)
#define builtin(vm)     ((vm)->gbldesc.builtin)

extern BERRY_LOCAL bclass_array_t be_class_table;

void be_globalvar_init(bvm_t *vm)
{
    global(vm).vtab = be_map_new(vm);
    be_gc_fix(vm, gc_object(global(vm).vtab));
    be_vector_init(vm, &global(vm).vlist, sizeof(bvalue_t));
#if !BE_USE_PRECOMPILED_OBJECT
    builtin(vm).vtab = be_map_new(vm);
    be_vector_init(vm, &builtin(vm).vlist, sizeof(bvalue_t));
    be_gc_fix(vm, gc_object(builtin(vm).vtab));
#endif
}

void be_globalvar_deinit(bvm_t *vm)
{
    global(vm).vtab = NULL;
    be_vector_delete(vm, &global(vm).vlist);
#if !BE_USE_PRECOMPILED_OBJECT
    builtin(vm).vtab = NULL;
    be_vector_delete(vm, &builtin(vm).vlist);
#endif
}

/* This function is called when the global was not found */
/* If the name looked for matches a static class, a global with the class name is created lazily */
/* Pre: no global exists with name `name`*/
/* Returns: idx of newly created global, or -1 if not found */
static int global_native_class_find(bvm_t *vm, bstring_t *name)
{
    const char* cl_name = str(name);
    bclass_ptr_t* class_p = &be_class_table[0];
    for (; *class_p; class_p++) {
        const bclass_t* cl = *class_p;

        if (!strcmp(str(cl->name), cl_name)) {
            /* class name matches */
            int idx = be_global_new(vm, name);
            bvalue_t *v = be_global_var(vm, idx);
            var_setclass(v, (void*) cl);
            return idx;
        }
    }
    return -1;
}

static int global_find(bvm_t *vm, bstring_t *name)
{
    bvalue_t *res = be_map_findstr(vm, global(vm).vtab, name);
    if (res) {
        return var_toidx(res) + be_builtin_count(vm);
    }
    return -1; /* not found */
}

int be_global_find(bvm_t *vm, bstring_t *name)
{
    int res = global_find(vm, name);
    if (res < 0) {
        res = be_builtin_find(vm, name);
    }
    if (res < 0) {
        res = global_native_class_find(vm, name);
    }
    return res;
}

static int global_new_anonymous(bvm_t *vm)
{
    int idx = be_global_count(vm);
    /* allocate space for new variables */
    be_vector_resize(vm, &global(vm).vlist, idx + 1);
    /* set the new variable to nil */
    var_setnil((bvalue_t *)global(vm).vlist.end);
    return idx;
}

int be_global_new(bvm_t *vm, bstring_t *name)
{
    int idx = global_find(vm, name);
    if (idx == -1) {
        bvalue_t *desc;
        idx = global_new_anonymous(vm);
        desc = be_map_insertstr(vm, global(vm).vtab, name, NULL);
        var_setint(desc, idx);
        idx += be_builtin_count(vm);
    }
    return idx;
}

bvalue_t* be_global_var(bvm_t *vm, int index)
{
    int bcnt = be_builtin_count(vm);
    if (index < bcnt) {
        return be_vector_at(&builtin(vm).vlist, index);
    }
    index -= bcnt;
    return be_vector_at(&global(vm).vlist, index);
}

void be_global_release_space(bvm_t *vm)
{
    be_map_compact(vm, global(vm).vtab);
    be_vector_release(vm, &global(vm).vlist);
}

int be_builtin_find(bvm_t *vm, bstring_t *name)
{
    bvalue_t *res = be_map_findstr(vm, builtin(vm).vtab, name);
    if (res) {
        return var_toidx(res);
    }
    return -1; /* not found */
}

bstring_t* be_builtin_name(bvm_t *vm, int index)
{
    bmap_t *map = builtin(vm).vtab;
    bmapnode_t *end, *node = map->slots;
    for (end = node + map->size; node < end; ++node) {
        if (var_isstr(&node->key) && node->value.v.i == index) {
            return node->key.v.s;
        }
    }
    return NULL;
}

#if !BE_USE_PRECOMPILED_OBJECT
int be_builtin_new(bvm_t *vm, bstring_t *name)
{
    int idx = be_builtin_find(vm, name);
    if (idx == -1) {
        bvalue_t *desc;
        idx = be_map_count(builtin(vm).vtab);
        desc = be_map_insertstr(vm, builtin(vm).vtab, name, NULL);
        var_setint(desc, idx);
        be_vector_resize(vm, &builtin(vm).vlist, idx + 1);
        /* set the new variable to nil */
        var_setnil((bvalue_t*)(builtin(vm).vlist.end));
    }
    return idx;
}

void be_bulitin_release_space(bvm_t *vm)
{
    be_map_compact(vm, builtin(vm).vtab);
    be_vector_release(vm, &builtin(vm).vlist);
}
#else
void be_const_builtin_set(bvm_t *vm, const bmap_t *map, const bvector_t *vec)
{
    builtin(vm).vtab = cast(bmap_t*, map);
    builtin(vm).vlist = *vec;
}
#endif
