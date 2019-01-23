#include "be_object.h"
#include "be_mem.h"
#include <stdio.h>
#include <stdlib.h>

#if BE_USE_OS_MODULE

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

#define FNAME_BUF_SIZE  600

static int m_getcwd(bvm *vm)
{
    char *buf = be_malloc(FNAME_BUF_SIZE);
    if (getcwd(buf, FNAME_BUF_SIZE)) {
        be_pushstring(vm, buf);
    } else {
        be_pushstring(vm, "");
    }
    be_return(vm);
}

static int m_chdir(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        int res = chdir(be_tostring(vm, 1));
        be_pushbool(vm, res == 0);
    }
    be_return(vm);
}

static int m_mkdir(bvm *vm)
{
    int res = 1;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
#ifdef _WIN32
        res = mkdir(be_tostring(vm, 1));
#else
        res = mkdir(be_tostring(vm, 1),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }
    be_pushbool(vm, res == 0);
    be_return(vm);
}

static int m_rmdir(bvm *vm)
{
    int res = 1;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        res = rmdir(be_tostring(vm, 1));
    }
    be_pushbool(vm, res == 0);
    be_return(vm);
}

static int m_listdir(bvm *vm)
{
    DIR *dp;
    struct dirent *ep;

    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        dp = opendir(be_tostring(vm, 1));
    } else {
        dp = opendir(".");
    }
    be_getglobal(vm, "list");
    if (dp != NULL) {
        be_newlist(vm);
        while ((ep = readdir (dp)) != NULL) {
            const char *fn = ep->d_name;
            if (strcmp(fn, ".") && strcmp(fn, "..")) {
                be_pushstring(vm, fn);
                be_append(vm, -2);
                be_pop(vm, 1);
            }
        }
        closedir (dp);
        be_call(vm, 1);
        be_pop(vm, 1);
    } else {
        be_call(vm, 0);
    }
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
    struct stat path_stat;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        stat(be_tostring(vm, 1), &path_stat);
    } else {
        stat(".", &path_stat);
    }
    be_pushbool(vm, S_ISDIR(path_stat.st_mode));
    be_return(vm);
}

static int m_isfile(bvm *vm)
{
    int res;
    struct stat path_stat;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        res = stat(be_tostring(vm, 1), &path_stat);
    } else {
        res = stat(".", &path_stat);
    }
    be_pushbool(vm, res == 0 && !S_ISDIR(path_stat.st_mode));
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
        be_getglobal(vm, "list");
        be_pushnstring(vm, str, dot - str);
        be_pushstring(vm, dot);
        be_call(vm, 2);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int m_exists(bvm *vm)
{
    int res;
    struct stat path_stat;
    if (be_top(vm) >= 1 && be_isstring(vm, 1)) {
        res = stat(be_tostring(vm, 1), &path_stat);
    } else {
        res = stat(".", &path_stat);
    }
    be_pushbool(vm, res == 0);
    be_return(vm);
}

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
    be_native_module_function("rmdir", m_rmdir),
    be_native_module_function("listdir", m_listdir),
    be_native_module_function("system", m_system),
    be_native_module_module("path", be_native_module(path))
};

be_define_native_module(os, os_attr);

#endif
