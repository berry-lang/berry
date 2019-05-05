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
    builtin(vm).vtab = be_map_new(vm);
    be_vector_init(&builtin(vm).vlist, sizeof(bvalue));
    be_gc_fix(vm, gc_object(builtin(vm).vtab));
}

void be_globalvar_deinit(bvm *vm)
{
    global(vm).vtab = NULL;
    be_vector_delete(&global(vm).vlist);
    builtin(vm).vtab = NULL;
    be_vector_delete(&builtin(vm).vlist);
}

int be_builtin_find(bvm *vm, bstring *name)
{
    bvalue *res = be_map_findstr(builtin(vm).vtab, name);
    if (res) {
        return var_toidx(res);
    }
    return -1; /* not found */
}

int be_global_find(bvm *vm, bstring *name)
{
    bvalue *res = be_map_findstr(global(vm).vtab, name);
    if (res) {
        return var_toidx(res) + be_builtin_count(vm);
    }
    return be_builtin_find(vm, name);
}

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

int be_global_new(bvm *vm, bstring *name)
{
    int idx = be_global_find(vm, name);
    if (idx == -1) {
        bvalue *desc;
        idx = be_map_count(global(vm).vtab);
        desc = be_map_insertstr(global(vm).vtab, name, NULL);
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

void be_bulitin_release_space(bvm *vm)
{
    be_map_release(vm, builtin(vm).vtab);
    be_vector_release(&builtin(vm).vlist);
}

void be_global_release_space(bvm *vm)
{
    be_map_release(vm, global(vm).vtab);
    be_vector_release(&global(vm).vlist);
}
