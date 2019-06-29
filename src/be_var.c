#include "be_var.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_string.h"
#include "be_map.h"
#include "be_gc.h"

#define global(vm)      ((vm)->gbldesc.global)
#define builtin(vm)     ((vm)->gbldesc.builtin)

void be_globalvar_init(bvm *vm)
{
    global(vm).vtab = be_map_new(vm);
    be_vector_init(&global(vm).vlist, sizeof(bvalue));
    be_gc_fix(vm, gc_object(global(vm).vtab));
#if !BE_USE_PRECOMPILED_OBJECT
    builtin(vm).vtab = be_map_new(vm);
    be_vector_init(&builtin(vm).vlist, sizeof(bvalue));
    be_gc_fix(vm, gc_object(builtin(vm).vtab));
#endif
}

void be_globalvar_deinit(bvm *vm)
{
    global(vm).vtab = NULL;
    be_vector_delete(&global(vm).vlist);
#if !BE_USE_PRECOMPILED_OBJECT
    builtin(vm).vtab = NULL;
    be_vector_delete(&builtin(vm).vlist);
#endif
}

static int global_find(bvm *vm, bstring *name)
{
    bvalue *res = be_map_findstr(global(vm).vtab, name);
    if (res) {
        return var_toidx(res) + be_builtin_count(vm);
    }
    return -1; /* not found */
}

int be_global_find(bvm *vm, bstring *name)
{
    int res = global_find(vm, name);
    return res != -1 ? res : be_builtin_find(vm, name);
}

int be_global_new(bvm *vm, bstring *name)
{
    int idx = global_find(vm, name);
    if (idx == -1) {
        bvalue *desc;
        idx = be_map_count(global(vm).vtab);
        desc = be_map_insertstr(vm, global(vm).vtab, name, NULL);
        var_setint(desc, idx);
        be_vector_resize(&global(vm).vlist, idx + 1);
        /* set the new variable to nil */
        var_setnil((bvalue*)(global(vm).vlist.end));
        return idx + be_builtin_count(vm);
    }
    return idx;
}

bvalue* be_global_var(bvm *vm, int index)
{
    int bcnt = be_builtin_count(vm);
    if (index < bcnt) {
        return be_vector_at(&builtin(vm).vlist, index);
    }
    index -= bcnt;
    return be_vector_at(&global(vm).vlist, index);
}

void be_global_release_space(bvm *vm)
{
    be_map_release(vm, global(vm).vtab);
    be_vector_release(&global(vm).vlist);
}

int be_builtin_find(bvm *vm, bstring *name)
{
    bvalue *res = be_map_findstr(builtin(vm).vtab, name);
    if (res) {
        return var_toidx(res);
    }
    return -1; /* not found */
}

bstring* be_builtin_name(bvm *vm, int index)
{
    bmap *map = builtin(vm).vtab;
    bmapnode *end, *node = map->slots;
    for (end = node + map->size; node < end; ++node) {
        if (var_isstr(&node->key) && node->value.v.i == index) {
            return node->key.v.s;
        }
    }
    return NULL;
}

#if !BE_USE_PRECOMPILED_OBJECT
int be_builtin_new(bvm *vm, bstring *name)
{
    int idx = be_builtin_find(vm, name);
    if (idx == -1) {
        bvalue *desc;
        idx = be_map_count(builtin(vm).vtab);
        desc = be_map_insertstr(builtin(vm).vtab, name, NULL);
        var_setint(desc, idx);
        be_vector_resize(&builtin(vm).vlist, idx + 1);
        /* set the new variable to nil */
        var_setnil((bvalue*)(builtin(vm).vlist.end));
    }
    return idx;
}

void be_bulitin_release_space(bvm *vm)
{
    be_map_release(vm, builtin(vm).vtab);
    be_vector_release(&builtin(vm).vlist);
}
#else
void be_const_builtin_set(bvm *vm, const bmap *map, const bvector *vec)
{
    builtin(vm).vtab = cast(bmap*, map);
    builtin(vm).vlist = *vec;
}
#endif
