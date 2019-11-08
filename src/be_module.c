#include "be_module.h"
#include "be_string.h"
#include "be_exec.h"
#include "be_map.h"
#include "be_gc.h"
#include "be_mem.h"
#include "be_vm.h"
#include <string.h>

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

static void insert_attrs(bvm *vm, bmap *table, bntvmodule *nm)
{
    size_t i;
    for (i = 0; i < nm->size; ++i) {
        bntvmodule_obj *node = nm->table + i;
        bstring *name = be_newstr(vm, node->name);
        bvalue *v = be_map_insertstr(vm, table, name, NULL);
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
            var_setstr(v, be_newstr(vm, node->u.s));
            break;
        case BE_CMODULE:
            load_module(vm, node->u.m, v);
            break;
        default: /* error */
            break;
        }
    }
}

static bmodule* new_module(bvm *vm, bntvmodule *nm)
{
    bgcobject *gco = be_gcnew(vm, BE_MODULE, bmodule);
    bmodule *obj = cast_module(gco);
    if (obj) {
        var_setmodule(vm->top, obj);
        be_incrtop(vm);
        obj->info.native = nm;
        obj->table = NULL; /* gc protection */
        obj->table = be_map_new(vm);
        insert_attrs(vm, obj->table, nm);
        be_map_release(vm, obj->table); /* clear space */
        be_stackpop(vm, 1);
    }
    return obj;
}

static bmodule* load_module(bvm *vm, bntvmodule *nm, bvalue *dst)
{
    if (nm) {
        bmodule *obj;
        if (nm->module) {
            obj = (bmodule *)nm->module;
        } else { /* new module */
            obj = new_module(vm, nm);
        }
        if (obj && dst) {
            var_setmodule(dst, obj);
        }
        return obj;
    }
    return NULL;
}

static int _load_script_module(bvm *vm, bstring *path)
{
    int res = be_fileparser(vm, str(path), 1);
    if (res == BE_OK) {
        be_call(vm, 0);
    } else {
        be_pop(vm, 1);
    }
    return res;
}

static bvalue* load_cached(bvm *vm, bstring *path)
{
    bvalue *v = NULL;
    if (vm->loaded) {
        v = be_map_findstr(vm->loaded, path);
        if (v) {
            *vm->top = *v;
            be_incrtop(vm);
        }
    }
    return v;
}

static void cache_module(bvm *vm, bstring *name)
{
    bvalue *v;
    if (vm->loaded == NULL) {
        vm->loaded = be_map_new(vm);
        be_gc_fix(vm, gc_object(vm->loaded));
    }
    v = be_map_insertstr(vm, vm->loaded, name, NULL);
    *v = vm->top[-1];
}

/* load module to vm->top */
bbool be_module_load(bvm *vm, bstring *path)
{
    if (load_cached(vm, path)) {
        return btrue;
    }
    if (_load_script_module(vm, path) != BE_OK) {
        bntvmodule *nm = find_native(path);
        bmodule *mod = load_module(vm, nm, NULL);
        if (mod == NULL) {
            return bfalse;
        }
        /* the pointer vm->top may be changed */
        var_setmodule(vm->top, mod);
        be_incrtop(vm);
    }
    cache_module(vm, path);
    return btrue;
}

void be_module_delete(bvm *vm, bmodule *module)
{
    be_free(vm, module, sizeof(bmodule));
}

bvalue* be_module_attr(bmodule *module, bstring *attr)
{
    return be_map_findstr(module->table, attr);
}

const char *be_module_name(bmodule *module)
{
    if (gc_isconst(module)) {
        return module->info.name;
    }
    return module->info.native->name;
}
