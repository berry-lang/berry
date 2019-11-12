#include "be_module.h"
#include "be_string.h"
#include "be_strlib.h"
#include "be_list.h"
#include "be_exec.h"
#include "be_map.h"
#include "be_gc.h"
#include "be_mem.h"
#include "be_vm.h"
#include <string.h>

extern bntvmodule* const be_module_table[];

static bmodule* native_module(bvm *vm, bntvmodule *nm, bvalue *dst);

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
        bntvmodule_obj *node = nm->attrs + i;
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
            native_module(vm, node->u.m, v);
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

static bmodule* native_module(bvm *vm, bntvmodule *nm, bvalue *dst)
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

static char* fixpath(bvm *vm, bstring *path, size_t *size)
{
    char *buffer;
    const char *split, *base;
    bvalue *func = vm->cf->func;
    bclosure *cl = var_toobj(func);
    be_assert(var_isclosure(func));
    base = str(cl->proto->source); /* get the source file path */
    split = be_splitpath(base);
    *size = split - base + (size_t)str_len(path) + 1;
    buffer = be_malloc(vm, *size);
    strncpy(buffer, base, split - base);
    strcpy(buffer + (split - base), str(path));
    return buffer;
}

static char* conpath(bvm *vm, bstring *path1, bstring *path2, size_t *size)
{
    char *buffer;
    int len1 = str_len(path1);
    *size = (size_t)len1 + (size_t)str_len(path2) + 1;
    buffer = be_malloc(vm, *size);
    strcpy(buffer, str(path1));
    buffer[len1] = '/';
    strcpy(buffer + len1 + 1, str(path2));
    return buffer;
}

static int load_scriptfile(bvm *vm, char *path, size_t size)
{
    int res = be_fileparser(vm, path, 1);
    be_free(vm, path, size);
    if (res == BE_OK)
        be_call(vm, 0);
    return res;
}

static int load_path(bvm *vm, bstring *path, bstring *mod)
{
    size_t size;
    char *fullpath = conpath(vm, path, mod, &size);
    return load_scriptfile(vm, fullpath, size);
}

static int load_cwd(bvm *vm, bstring *path)
{
    size_t size;
    char *fullpath = fixpath(vm, path, &size);
    return load_scriptfile(vm, fullpath, size);
}

bbool load_script(bvm *vm, bstring *path)
{
    bbool res = load_cwd(vm, path) == BE_OK; /* load from current directory */
    if (!res && vm->module.path) {
        blist *list = vm->module.path;
        bvalue *v = be_list_end(list) - 1;
        bvalue *first = be_list_data(list);
        for (; !res && v >= first; v--) {
            if (var_isstr(v)) {
                res = load_path(vm, var_tostr(v), path) == BE_OK;
            }
        }
    }
    return res;
}

bbool load_native(bvm *vm, bstring *path)
{
    bntvmodule *nm = find_native(path);
    bmodule *mod = native_module(vm, nm, NULL);
    if (mod == NULL)
        return bfalse;
    /* the pointer vm->top may be changed */
    var_setmodule(vm->top, mod);
    be_incrtop(vm);
    return btrue;
}

static bvalue* load_cached(bvm *vm, bstring *path)
{
    bvalue *v = NULL;
    if (vm->module.loaded) {
        v = be_map_findstr(vm->module.loaded, path);
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
    if (vm->module.loaded == NULL) {
        vm->module.loaded = be_map_new(vm);
    }
    v = be_map_insertstr(vm, vm->module.loaded, name, NULL);
    *v = vm->top[-1];
}

/* load module to vm->top */
bbool be_module_load(bvm *vm, bstring *path)
{
    if (!load_cached(vm, path)) {
        do {
            if (load_native(vm, path))
                break;
            if (load_script(vm, path))
                break;
            return bfalse; /* load failed */
        } while (0);
        cache_module(vm, path);
    }
    return btrue;
}

bmodule* be_module_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_MODULE, bmodule);
    bmodule *obj = cast_module(gco);
    if (obj) {
        var_setmodule(vm->top, obj);
        be_incrtop(vm);
        obj->info.native = NULL;
        obj->table = NULL; /* gc protection */
        obj->table = be_map_new(vm);
        be_stackpop(vm, 1);
    }
    return obj;
}

void be_module_delete(bvm *vm, bmodule *module)
{
    be_free(vm, module, sizeof(bmodule));
}

bvalue* be_module_attr(bmodule *module, bstring *attr)
{
    return be_map_findstr(module->table, attr);
}

bvalue* be_module_bind(bvm *vm, bmodule *module, bstring *attr)
{
    bmap *attrs = module->table;
    if (!gc_isconst(attrs)) {
        bvalue *v = be_map_findstr(attrs, attr);
        if (v == NULL) {
            v = be_map_insertstr(vm, attrs, attr, NULL);
        }
        return v;
    }
    return NULL;
}

const char* be_module_name(bmodule *module)
{
    if (gc_isconst(module)) {
        return module->info.name;
    }
    if (module->info.native) {
        return module->info.native->name;
    }
    return NULL;
}

static blist* pathlist(bvm *vm)
{
    if (!vm->module.path) {
        vm->module.path = be_list_new(vm);
    }
    return vm->module.path;
}

/* push the path list to the top */
void be_module_path(bvm *vm)
{
    blist *list = pathlist(vm);
    bvalue *reg = be_incrtop(vm);
    var_setlist(reg, list);
}

void be_module_path_append(bvm *vm, const char *path)
{
    blist *list = pathlist(vm);
    bvalue *value = be_list_append(vm, list, NULL);
    var_setstr(value, be_newstr(vm, path))
}
