#include "be_module.h"
#include "be_string.h"
#include "be_map.h"
#include "be_gc.h"
#include "be_mem.h"
#include "be_vm.h"

be_extern_native_module(io);
be_extern_native_module(json);

static bmodule_native * module_tab[] = {
    &be_native_module(io),
    &be_native_module(json)
};

static bmodule_native* find_native(bstring *path)
{
    size_t i, size = array_count(module_tab);
    for (i = 0; i < size; ++i) {
        bmodule_native *node = module_tab[i];
        if (!strcmp(node->name, str(path))) {
            return node;
        }
    }
    return NULL;
}

static bmodule* find_existed(bvm *vm, bmodule_native *nm)
{
    bmodule *node  = vm->modulelist;
    while (node && node->native != nm) {
        node = node->mnext;
    }
    return node;
}

bmodule* new_module(bvm *vm, bmodule_native *nm, bvalue *dst)
{
    bgcobject *gco = be_gcnew(vm, BE_MODULE, bmodule);
    bmodule *obj = cast_module(gco);
    if (obj) {
        size_t i;
        var_setmodule(dst, obj);
        obj->native = nm;
        obj->table = NULL;
        obj->table = be_map_new(vm);
        for (i = 0; i < nm->size; ++i) {
            const bmemberinfo *node = nm->table + i;
            bstring *name = be_newconststr(vm, node->name);
            bvalue *v = be_map_insertstr(obj->table, name, NULL);
            var_setntvfunc(v, node->function);
        }
        vm->modulelist = obj->mnext;
        obj->mnext = vm->modulelist;
    }
    return obj;
}

bmodule* be_module_load(bvm *vm, bstring *path, bvalue *dst)
{
    bmodule_native *nm = find_native(path);
    if (nm) {
        bmodule *obj = find_existed(vm, nm);
        if (obj) { /* existed module */
            var_setmodule(dst, obj);
        } else { /* new module */
            obj = new_module(vm, nm, dst);
        }
        return obj;
    }
    return NULL;
}

void be_module_delete(bvm *vm, bmodule *module)
{
    bmodule *prev = vm->modulelist;
    /* find previous node */
    while (prev && prev->mnext != module) {
        prev = prev->mnext;
    }
    if (prev) { /* remove module node from module-list */
        prev->next = module->next;
    }
    be_free(module);
}

bvalue* be_module_attr(bmodule *module, bstring *attr)
{
    return be_map_findstr(module->table, attr);
}
