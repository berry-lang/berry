#include "be_module.h"
#include "be_string.h"
#include "be_map.h"
#include "be_gc.h"
#include "be_mem.h"
#include "be_vm.h"

extern bntvmodule* const be_module_table[];

static bmodule* load_module(bvm *vm, bntvmodule *nm, bvalue *dst);

static bntvmodule* find_native(bstring *path)
{
    bntvmodule *module;
    bntvmodule * const *node = be_module_table;
    for (; (module = *node) != NULL; ++node) {
        if (!strcmp(module->name, str(path))) {
            return module;
        }
    }
    return NULL;
}

static bmodule* find_existed(bvm *vm, bntvmodule *nm)
{
    bmodule *node  = vm->modulelist;
    while (node && node->native != nm) {
        node = node->mnext;
    }
    return node;
}

static void insert_attrs(bvm *vm, bmap *table, bntvmodule *nm)
{
    size_t i;
    for (i = 0; i < nm->size; ++i) {
        bntvmodule_obj *node = nm->table + i;
        bstring *name = be_newconststr(vm, node->name);
        bvalue *v = be_map_insertstr(table, name, NULL);
        be_assert(node->type <= BE_CMODULE);
        switch (node->type) {
        case BE_CNIL:
            var_setnil(v);
            break;
        case BE_CINT:
            var_setint(v, node->u.i);
            break;
        case BE_CREAL:
            var_setreal(v, node->u.r);
            break;
        case BE_CBOOL:
            var_setbool(v, node->u.b);
            break;
        case BE_CFUNCTION:
            var_setntvfunc(v, node->u.f);
            break;
        case BE_CSTRING:
            var_setstr(v, be_newconststr(vm, node->u.s));
            break;
        case BE_CMODULE:
            load_module(vm, node->u.m, v);
            break;
        default: /* error */
            break;
        }
    }
}

static bmodule* new_module(bvm *vm, bntvmodule *nm, bvalue *dst)
{
    bgcobject *gco = be_gcnew(vm, BE_MODULE, bmodule);
    bmodule *obj = cast_module(gco);
    if (obj) {
        var_setmodule(dst, obj);
        obj->native = nm;
        obj->table = NULL; /* gc protection */
        obj->table = be_map_new(vm);
        insert_attrs(vm, obj->table, nm);
        obj->mnext = vm->modulelist;
        vm->modulelist = obj;
    }
    return obj;
}

static bmodule* load_module(bvm *vm, bntvmodule *nm, bvalue *dst)
{
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

bmodule* be_module_load(bvm *vm, bstring *path, bvalue *dst)
{
    bntvmodule *nm = find_native(path);
    return load_module(vm, nm, dst);
}

void be_module_delete(bvm *vm, bmodule *module)
{
    if (module == vm->modulelist) {
        /* remove module node from module-list */
        vm->modulelist = module->mnext;
    } else {
        bmodule *prev = vm->modulelist;
        /* find previous node */
        while (prev && prev->mnext != module) {
            prev = prev->mnext;
        }
        /* remove module node from module-list */
        if (prev) {
            prev->mnext = module->mnext;
        }
    }
    be_free(module);
}

bvalue* be_module_attr(bmodule *module, bstring *attr)
{
    return be_map_findstr(module->table, attr);
}
