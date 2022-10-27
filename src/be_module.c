/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
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

/* the maximum suffix length */
#define SUFFIX_LEN      5 /* length of (.be .bec .so .dll) + 1 */

/* check POSIX platforms */
#if defined(__linux__) || \
    defined(__APPLE__) || defined(__unix__) || defined(__CYGWIN__)
  #define __POSIX_OS__
#endif

#if BE_USE_SHARED_LIB
  #if defined(__POSIX_OS__)
    #define DLL_SUFFIX ".so"
  #elif defined(_WIN32)
    #define DLL_SUFFIX ".dll"
  #else
    #define DLL_SUFFIX ""
    #warning module: unsuppord OS
  #endif
#endif

extern BERRY_LOCAL const bntvmodule_t* const be_module_table[];

static bmodule_t* native_module(bvm_t *vm, const bntvmodule_t *nm, bvalue_t *dst);

static const bntvmodule_t* find_native(bstring_t *path)
{
    const bntvmodule_t *module;
    const bntvmodule_t* const *node = be_module_table;
    for (; (module = *node) != NULL; ++node) {
        if (!strcmp(module->name, str(path))) {
            return module;
        }
    }
    return NULL;
}

static void insert_attrs(bvm_t *vm, bmap_t *table, const bntvmodule_t *nm)
{
    size_t i;
    for (i = 0; i < nm->size; ++i) {
        const bntvmodobj_t *node = nm->attrs + i;
        bstring_t *name = be_newstr(vm, node->name);
        bvalue_t *v = be_map_insertstr(vm, table, name, NULL);
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
            native_module(vm, node->u.o, v);
            break;
        default: /* error */
            break;
        }
    }
}

static bmodule_t* new_module(bvm_t *vm, const bntvmodule_t *nm)
{
    bgcobject_t *gco = be_gcnew(vm, BE_MODULE, bmodule_t);
    bmodule_t *obj = cast_module(gco);
    if (obj) {
        var_setmodule(vm->top, obj);
        be_incrtop(vm);
        obj->info.native = nm;
        obj->table = NULL; /* gc protection */
        obj->table = be_map_new(vm);
        insert_attrs(vm, obj->table, nm);
        be_map_compact(vm, obj->table); /* clear space */
        be_stackpop(vm, 1);
    }
    return obj;
}

static bmodule_t* native_module(bvm_t *vm, const bntvmodule_t *nm, bvalue_t *dst)
{
    if (nm) {
        bmodule_t *obj;
        if (nm->module) {
            obj = (bmodule_t *)nm->module;
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

static char* fixpath(bvm_t *vm, bstring_t *path, size_t *size)
{
    char *buffer;
    const char *split, *base;
    bvalue_t *func = vm->cf->func;
    bclosure_t *cl = var_toobj(func);
    if (var_isclosure(func)) {
        base = str(cl->proto->source); /* get the source file path */
    } else {
        base = "/";
    }
    split = be_splitpath(base);
    *size = split - base + (size_t)str_len(path) + SUFFIX_LEN;
    buffer = be_malloc(vm, *size);
    strncpy(buffer, base, split - base);
    strcpy(buffer + (split - base), str(path));
    return buffer;
}

static char* conpath(bvm_t *vm, bstring_t *path1, bstring_t *path2, size_t *size)
{
    char *buffer;
    int len1 = str_len(path1);
    *size = (size_t)len1 + (size_t)str_len(path2) + 1 + SUFFIX_LEN;
    buffer = be_malloc(vm, *size);
    strcpy(buffer, str(path1));
    buffer[len1] = '/';
    strcpy(buffer + len1 + 1, str(path2));
    return buffer;
}

static int open_script(bvm_t *vm, char *path)
{
    int res = be_loadmodule(vm, path);
    if (res == BE_OK)
        be_call(vm, 0);
    return res;
}

#if BE_USE_SHARED_LIB
static int open_dllib(bvm_t *vm, char *path)
{
    int res = be_loadlib(vm, path);
    if (res == BE_OK)
        be_call(vm, 0);
    return res;
}
#endif

static int open_libfile(bvm_t *vm, char *path, size_t size)
{
    int res, idx = 0;
    const char *sfxs[] = { "", ".bec", ".be" };
    do {
        strcpy(path + size - SUFFIX_LEN, sfxs[idx]);
        res = open_script(vm, path);
    } while (idx++ < 2 && res == BE_IO_ERROR);
    if (res == BE_IO_ERROR) {
#if BE_USE_SHARED_LIB
        strcpy(path + size - SUFFIX_LEN, DLL_SUFFIX);
        res = open_dllib(vm, path);
#endif
    }
    be_free(vm, path, size);
    return res;
}

static int load_path(bvm_t *vm, bstring_t *path, bstring_t *mod)
{
    size_t size;
    char *fullpath = conpath(vm, path, mod, &size);
    return open_libfile(vm, fullpath, size);
}

static int load_cwd(bvm_t *vm, bstring_t *path)
{
    size_t size;
    char *fullpath = fixpath(vm, path, &size);
    return open_libfile(vm, fullpath, size);
}

static int load_package(bvm_t *vm, bstring_t *path)
{
    int res = load_cwd(vm, path); /* load from current directory */
    if (res == BE_IO_ERROR && vm->module.path) {
        blist_t *list = vm->module.path;
        bvalue_t *v = be_list_end(list) - 1;
        bvalue_t *first = be_list_data(list);
        for (; res == BE_IO_ERROR && v >= first; v--) {
            if (var_isstr(v)) {
                res = load_path(vm, var_tostr(v), path);
            }
        }
    }
    return res;
}

static int load_native(bvm_t *vm, bstring_t *path)
{
    const bntvmodule_t *nm = find_native(path);
    bmodule_t *mod = native_module(vm, nm, NULL);
    if (mod != NULL) {
        /* the pointer vm->top may be changed */
        var_setmodule(vm->top, mod);
        be_incrtop(vm);
        return BE_OK;
    }
    return BE_IO_ERROR;
}

static bvalue_t* load_cached(bvm_t *vm, bstring_t *path)
{
    bvalue_t *v = NULL;
    if (vm->module.loaded) {
        v = be_map_findstr(vm, vm->module.loaded, path);
        if (v) {
            *vm->top = *v;
            be_incrtop(vm);
        }
    }
    return v;
}

void be_cache_module(bvm_t *vm, bstring_t *name)
{
    bvalue_t *v;
    if (vm->module.loaded == NULL) {
        vm->module.loaded = be_map_new(vm);
    }
    v = be_map_insertstr(vm, vm->module.loaded, name, NULL);
    *v = vm->top[-1];
}

/* Try to run 'init(m)' function of module. Module is already loaded. */
static void module_init(bvm_t *vm) {
    if (be_ismodule(vm, -1)) {
        if (be_getmember(vm, -1, "init")) {
            /* found, call it with current module as parameter */
            be_pushvalue(vm, -2);
            be_call(vm, 1);
            /* the result of init() is cached and returned */
            be_pop(vm, 1);
            be_remove(vm, -2);  /* remove initial module */
        } else {
            be_pop(vm, 1);
        }
    }
}

/* load module to vm->top */
int be_module_load(bvm_t *vm, bstring_t *path)
{
    int res = BE_OK;
    if (!load_cached(vm, path)) {
        res = load_native(vm, path);
        if (res == BE_IO_ERROR)
            res = load_package(vm, path);
        if (res == BE_OK) {
            /* on first load of the module, try running the '()' function */
            module_init(vm);
            be_cache_module(vm, path);
        }
    }
    return res;
}

BERRY_API bbool be_getmodule(bvm_t *vm, const char *k)
{
    int res = be_module_load(vm, be_newstr(vm, k));
    return res == BE_OK;
}

bmodule_t* be_module_new(bvm_t *vm)
{
    bgcobject_t *gco = be_gcnew(vm, BE_MODULE, bmodule_t);
    bmodule_t *obj = cast_module(gco);
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

void be_module_delete(bvm_t *vm, bmodule_t *module)
{
    be_free(vm, module, sizeof(bmodule_t));
}

int be_module_attr(bvm_t *vm, bmodule_t *module, bstring_t *attr, bvalue_t *dst)
{
    bvalue_t *member = be_map_findstr(vm, module->table, attr);
    if (!member) {  /* try the 'member' function */
        /* if 'init' does not exist, don't call member() */
        if (strcmp(str(attr), "init") == 0) {
            var_setntvfunc(dst, be_default_init_native_function);
            return var_primetype(dst);
        }
        member = be_map_findstr(vm, module->table, str_literal(vm, "member"));
        if (member && var_basetype(member) == BE_FUNCTION) {
            bvalue_t *top = vm->top;
            top[0] = *member;
            var_setstr(&top[1], attr);
            vm->top += 2;   /* prevent collection results */
            be_dofunc(vm, top, 1); /* call method 'method' */
            vm->top -= 2;
            *dst = *vm->top;   /* copy result to R(A) */
            
            int type = var_type(dst);
            if (type == BE_MODULE) {
                /* check if the module is named `undefined` */
                bmodule_t *mod = var_toobj(dst);
                if (strcmp(be_module_name(mod), "undefined") == 0) {
                    return BE_NONE;     /* if the return value is module `undefined`, consider it is an error */
            }
            }
            return type;
        }
        return BE_NONE;
    }
    *dst = *member;
    return var_type(dst);
}

bbool be_module_setmember(bvm_t *vm, bmodule_t *module, bstring_t *attr, bvalue_t *src)
{
    assert(src);
    bmap_t *attrs = module->table;
    if (!gc_isconst(attrs)) {
        bvalue_t *v = be_map_findstr(vm, attrs, attr);
        if (v == NULL) {
            v = be_map_insertstr(vm, attrs, attr, NULL);
        }
        if (v) {
            *v = *src;
            return btrue;
        }
    } else {
        /* if not writable, try 'setmember' */
        int type = be_module_attr(vm, module, str_literal(vm, "setmember"), vm->top);
        if (basetype(type) == BE_FUNCTION) {
            bvalue_t *top = vm->top;
            // top[0] already has 'member'
            var_setstr(&top[1], attr);  /* attribute name */
            top[2] = *src;  /* new value */
            vm->top += 3;   /* prevent collection results */
            be_dofunc(vm, top, 2); /* call method 'setmember' */
            vm->top -= 3;
            int type = var_type(vm->top);
            if (type == BE_BOOL) {
                bbool ret = var_tobool(vm->top);
                if (!ret) {
                    return bfalse;
                }
            } else if (type == BE_MODULE) {
                /* check if the module is named `undefined` */
                bmodule_t *mod = var_toobj(vm->top);
                if (strcmp(be_module_name(mod), "undefined") == 0) {
                    return bfalse;     /* if the return value is module `undefined`, consider it is an error */
                }
            }
            return btrue;
        }
    }
    return bfalse;
}

const char* be_module_name(bmodule_t *module)
{
    if (gc_isconst(module)) {
        return module->info.name;
    }
    if (gc_exmark(module) & BE_MODULE_NAME) {
        return str(module->info.sname);
    }
    if (module->info.native) {
        return module->info.native->name;
    }
    return NULL;
}

bbool be_module_setname(bmodule_t *module, bstring_t *name)
{
    if (!gc_isconst(module)) {
        module->info.sname = name;
        gc_setexmark(module, BE_MODULE_NAME);
        return btrue;
    }
    return bfalse;
}

static blist_t* pathlist(bvm_t *vm)
{
    if (!vm->module.path) {
        vm->module.path = be_list_new(vm);
    }
    return vm->module.path;
}

/* push the path list to the top */
BERRY_API void be_module_path(bvm_t *vm)
{
    blist_t *list = pathlist(vm);
    bvalue_t *reg = be_incrtop(vm);
    var_setlist(reg, list);
}

BERRY_API void be_module_path_set(bvm_t *vm, const char *path)
{
    blist_t *list = pathlist(vm);
    bvalue_t *value = be_list_push(vm, list, NULL);
    var_setnil(value);
    var_setstr(value, be_newstr(vm, path))
}

/* shared library support */
#if BE_USE_SHARED_LIB

#if defined(__POSIX_OS__)
#include <dlfcn.h>

#if defined(__GNUC__)
  #define cast_func(f) (__extension__(bntvfunc)(f))
#else
  #define cast_func(f) ((bntvfunc)(f))
#endif

/* load shared library */
BERRY_API int be_loadlib(bvm_t *vm, const char *path)
{
    void *handle = dlopen(path, RTLD_LAZY);
    bntvfunc func = cast_func(dlsym(handle, "berry_export"));
    if (func == NULL) {
        return BE_IO_ERROR;
    }
    be_pushntvfunction(vm, func);
    return BE_OK;
}
#elif defined(_WIN32)
#include<wtypes.h>
#include <winbase.h>

BERRY_API int be_loadlib(bvm_t *vm, const char *path)
{
    HINSTANCE handle = LoadLibrary(path);
    if (handle) {
        union {
            FARPROC proc;
            bntvfunc func;
        } u;
        u.proc = GetProcAddress(handle, "berry_export");
        if (u.func != NULL) {
            be_pushntvfunction(vm, u.func);
            return BE_OK;
        }
    }
    return BE_IO_ERROR;
}
#else
BERRY_API int be_loadlib(bvm_t *vm, const char *path)
{
    (void)vm, (void)path;
    return BE_IO_ERROR;
}
#endif

#endif /* BE_USE_SHARED_LIB */
