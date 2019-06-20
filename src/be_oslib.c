#include "be_object.h"
#include "be_mem.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define FNAME_BUF_SIZE 512

#if BE_USE_OS_MODULE

static int m_getcwd(bvm *vm)
{
    char *buf = be_malloc(FNAME_BUF_SIZE);
    if (be_getcwd(buf, FNAME_BUF_SIZE)) {
        be_pushstring(vm, buf);
    } else {
        be_pushstring(vm, "");
    }
    be_free(buf);
    be_return(vm);
}

static int m_chdir(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        int res = be_chdir(be_tostring(vm, 1));
        be_pushbool(vm, res == 0);
    }
    be_return(vm);
}

static int m_mkdir(bvm *vm)
{
    int res = 1;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        res = be_mkdir(be_tostring(vm, 1));
    }
    be_pushbool(vm, res == 0);
    be_return(vm);
}

static int m_remove(bvm *vm)
{
    int res = 1;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        res = be_unlink(be_tostring(vm, 1));
    }
    be_pushbool(vm, res == 0);
    be_return(vm);
}

static int m_listdir(bvm *vm)
{
    int res;
    bdirinfo info;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        res = be_dirfirst(&info, be_tostring(vm, 1));
    } else {
        res = be_dirfirst(&info, ".");
    }
    be_getbuiltin(vm, "list");
    be_newlist(vm);
    while (res == 0) {
        const char *fn = info.name;
        if (strcmp(fn, ".") && strcmp(fn, "..")) {
            be_pushstring(vm, fn);
            be_data_append(vm, -2);
            be_pop(vm, 1);
        }
        res = be_dirnext(&info);
    }
    be_call(vm, 1);
    be_pop(vm, 1);
    be_return(vm);
}

static int m_system(bvm *vm)
{
    int res = -1, i, argc = be_top(vm);
    if (argc > 0) {
        be_tostring(vm, 1);
        be_pushstring(vm, " ");
        for (i = 2; i <= argc; ++i) {
            be_strconcat(vm, 1); /* add " " */
            be_tostring(vm, i);
            be_pushvalue(vm, i);
            be_strconcat(vm, 1); /* concat arg */
            be_pop(vm, 1);
        }
        be_pop(vm, argc);
        res = system(be_tostring(vm, 1));
    }
    be_pushint(vm, res);
    be_return(vm);
}

static int m_isdir(bvm *vm)
{
    const char *path = NULL;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        path = be_tostring(vm, 1);
    }
    be_pushbool(vm, be_isdir(path));
    be_return(vm);
}

static int m_isfile(bvm *vm)
{
    const char *path = NULL;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        path = be_tostring(vm, 1);
    }
    be_pushbool(vm, be_isfile(path));
    be_return(vm);
}

static int m_splitext(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        const char *ptr, *dot, *str = be_tostring(vm, 1);
        for (ptr = str; *ptr != '\0' && *ptr == '.'; ++ptr);
        for (dot = str; *ptr != '\0'; ++ptr) {
            if (*ptr == '.') {
                dot = ptr;
            }
        }
        dot = dot == str ? ptr : dot;
        be_getbuiltin(vm, "list");
        be_pushnstring(vm, str, dot - str);
        be_pushstring(vm, dot);
        be_call(vm, 2);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int m_exists(bvm *vm)
{
    const char *path = NULL;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        path = be_tostring(vm, 1);
    }
    be_pushbool(vm, be_isexist(path));
    be_return(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(path_attr) {
    be_native_module_function("isdir", m_isdir),
    be_native_module_function("isfile", m_isfile),
    be_native_module_function("exists", m_exists),
    be_native_module_function("splitext", m_splitext)
};

static be_define_native_module(path, path_attr);

be_native_module_attr_table(os_attr) {
    be_native_module_function("getcwd", m_getcwd),
    be_native_module_function("chdir", m_chdir),
    be_native_module_function("mkdir", m_mkdir),
    be_native_module_function("remove", m_remove),
    be_native_module_function("listdir", m_listdir),
    be_native_module_function("system", m_system),
    be_native_module_module("path", be_native_module(path))
};

be_define_native_module(os, os_attr);
#else
/* @const_object_info_begin
module path (scope: local) {
    isdir, func(m_isdir)
    isfile, func(m_isfile)
    exists, func(m_exists)
    splitext, func(m_splitext)
}
@const_object_info_end */
#include "../generate/be_fixed_path.h"

/* @const_object_info_begin
module os (scope: global) {
    getcwd, func(m_getcwd)
    chdir, func(m_chdir)
    mkdir, func(m_mkdir)
    remove, func(m_remove)
    listdir, func(m_listdir)
    system, func(m_system)
    path, module(m_libpath)
}
@const_object_info_end */
#include "../generate/be_fixed_os.h"
#endif

#endif /* BE_USE_OS_MODULE */
