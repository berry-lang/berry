#ifndef BERRY_H
#define BERRY_H

#include <stddef.h>
#include <stdint.h>

#include "berry_conf.h"

/* do not modify the version number! */
#define BERRY_VERSION   "0.1.6"

#if BE_STACK_TOTAL_MAX < BE_STACK_FREE_MIN * 2
#error "The value of the macro BE_STACK_TOTAL_MAX is too small."
#endif

/* basic type definition */
#if BE_INTGER_TYPE == 0
  #define BE_INTEGER        int
  #define BE_INT_FMTLEN     ""
#elif BE_INTGER_TYPE == 1
  #define BE_INTEGER        long
  #define BE_INT_FMTLEN     "l"
#elif BE_INTGER_TYPE == 2
  #ifdef _WIN32
    #define BE_INTEGER      __int64
    #define BE_INT_FMTLEN   "I64"
  #else
    #define BE_INTEGER      long long
    #define BE_INT_FMTLEN   "ll"
  #endif
#else
  #error "Unsupported integer type for `bint`."
#endif
#define BE_INT_FORMAT       "%"BE_INT_FMTLEN"d"

typedef int                 bbool;
typedef uint8_t             bbyte;
typedef BE_INTEGER          bint;

#if BE_SINGLE_FLOAT != 0
    typedef float           breal;
#else
    typedef double          breal;
#endif

/* boolean values definition */
enum {
    bfalse = 0,
    btrue = 1
};

/* error code definition */
enum berrorcode {
    BE_OK = 0,
    BE_IO_ERROR,
    BE_SYNTAX_ERROR,
    BE_EXEC_ERROR,
    BE_EXCEPTION,
    BE_MALLOC_FAIL,
    BE_EXIT
};

/* native-module member type specifier */
#define BE_CNIL                 0
#define BE_CINT                 1
#define BE_CREAL                2
#define BE_CBOOL                3
#define BE_CFUNCTION            4
#define BE_CSTRING              5
#define BE_CMODULE              6

/* API function mark */
#ifdef BERRT_EXPORT_DLL
  #if defined(BERRY_LIB)
    #define BERRY_API           __declspec(dllexport)
  #else
    #define BERRY_API           __declspec(dllimport)
  #endif
#else
  #define BERRY_API             extern
#endif

/* only linux */
#define BERRY_LOCAL             __attribute__ ((visibility ("hidden")))

typedef struct bvm bvm;        /* virtual machine structure */
typedef int (*bntvfunc)(bvm*); /* native function pointer */

/* native function information */
typedef struct {
    const char *name;
    bntvfunc function;
} bnfuncinfo;

/* native module object node */
typedef const struct {
    const char *name;
    int type;
    union {
        bint i;
        breal r;
        bbool b;
        bntvfunc f;
        const char *s;
        const struct bntvmodule *m;
    } u;
} bntvmodule_obj;

/* native module object */
typedef const struct bntvmodule {
    const char *name; /* native module name */
    bntvmodule_obj *attrs; /* native module attributes */
    size_t size; /* native module attribute count */
    const struct bmodule *module; /* const module object */
    bntvfunc init; /* initialization function */
} bntvmodule;

/* native module node definition macro */
#define be_native_module_nil(_name)                 \
    { .name = (_name), .type = BE_CNIL, .u.i = 0 }

#define be_native_module_int(_name, _v)             \
    { .name = (_name), .type = BE_CINT, .u.i = (bint)(_v) }

#define be_native_module_real(_name, _v)            \
    { .name = (_name), .type = BE_CREAL, .u.r = (breal)(_v) }

#define be_native_module_bool(_name, _b)            \
    { .name = (_name), .type = BE_CBOOL, .u.b = (bbool)(_b) }

#define be_native_module_function(_name, _f)        \
    { .name = (_name), .type = BE_CFUNCTION, .u.f = (_f) }

#define be_native_module_str(_name, _s)             \
    { .name = (_name), .type = BE_CSTRING, .u.s = (_s) }

#define be_native_module_module(_name, _m)          \
    { .name = (_name), .type = BE_CMODULE, .u.m = &(_m) }

#define be_native_module(name)  be_native_module_##name

#define be_native_module_attr_table(name)           \
    static bntvmodule_obj name##_attrs[] =

/* native module declaration macro */
#define be_extern_native_module(name)               \
    extern bntvmodule be_native_module(name)

/* native module definition macro */
#define be_define_native_module(_name, _init)       \
    bntvmodule be_native_module(_name) = {          \
        .name = #_name,                             \
        .attrs = _name##_attrs,                     \
        .size = (sizeof(_name##_attrs)              \
               / sizeof(_name##_attrs[0])),         \
        .module = NULL,                             \
        .init = _init                               \
    }

#if !BE_DEBUG
  #if defined(be_assert)
    #undef be_assert
  #endif
  #define be_assert(expr)       ((void)0)
#endif

/* FFI functions */
#define be_writestring(s)       be_writebuffer((s), strlen(s))
#define be_writenewline()       be_writebuffer("\n", 1)

#define be_return(vm)           return be_returnvalue(vm)
#define be_return_nil(vm)       return be_returnnilvalue(vm)

#define be_loadstring(vm, str) \
    be_loadbuffer((vm), "string", (str), strlen(str))

#define be_dostring(vm, s) \
    (be_loadstring((vm), (s)) || be_pcall((vm), 0))

BERRY_API bint be_str2int(const char *str, const char **endstr);
BERRY_API breal be_str2real(const char *str, const char **endstr);
BERRY_API const char *be_str2num(bvm *vm, const char *str);

BERRY_API int be_top(bvm *vm);
BERRY_API const char *be_typename(bvm *vm, int index);
BERRY_API const char *be_classname(bvm *vm, int index);
BERRY_API bbool be_classof(bvm *vm, int index);
BERRY_API int be_strlen(bvm *vm, int index);
BERRY_API void be_strconcat(bvm *vm, int index);
BERRY_API void be_pop(bvm *vm, int n);
BERRY_API void be_remove(bvm *vm, int index);
BERRY_API int be_absindex(bvm *vm, int index);

BERRY_API bbool be_isnil(bvm *vm, int index);
BERRY_API bbool be_isbool(bvm *vm, int index);
BERRY_API bbool be_isint(bvm *vm, int index);
BERRY_API bbool be_isreal(bvm *vm, int index);
BERRY_API bbool be_isnumber(bvm *vm, int index);
BERRY_API bbool be_isstring(bvm *vm, int index);
BERRY_API bbool be_isclosure(bvm *vm, int index);
BERRY_API bbool be_isntvclos(bvm *vm, int index);
BERRY_API bbool be_isfunction(bvm *vm, int index);
BERRY_API bbool be_isproto(bvm *vm, int index);
BERRY_API bbool be_isclass(bvm *vm, int index);
BERRY_API bbool be_isinstance(bvm *vm, int index);
BERRY_API bbool be_islist(bvm *vm, int index);
BERRY_API bbool be_ismap(bvm *vm, int index);
BERRY_API bbool be_iscomptr(bvm *vm, int index);

BERRY_API bint be_toint(bvm *vm, int index);
BERRY_API breal be_toreal(bvm *vm, int index);
BERRY_API int be_toindex(bvm *vm, int index);
BERRY_API bbool be_tobool(bvm *vm, int index);
BERRY_API const char* be_tostring(bvm *vm, int index);
BERRY_API void* be_tocomptr(bvm *vm, int index);
BERRY_API void be_moveto(bvm *vm, int from, int to);
BERRY_API void be_pushnil(bvm *vm);
BERRY_API void be_pushbool(bvm *vm, int b);
BERRY_API void be_pushint(bvm *vm, bint i);
BERRY_API void be_pushreal(bvm *vm, breal r);
BERRY_API void be_pushstring(bvm *vm, const char *str);
BERRY_API void be_pushnstring(bvm *vm, const char *str, size_t n);
BERRY_API const char* be_pushfstring(bvm *vm, const char *format, ...);
BERRY_API void be_pushvalue(bvm *vm, int index);
BERRY_API void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);
BERRY_API void be_pushntvfunction(bvm *vm, bntvfunc f);
BERRY_API void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);
BERRY_API void be_pushcomptr(bvm *vm, void *ptr);
BERRY_API bbool be_pushiter(bvm *vm, int index);
BERRY_API void be_pusherror(bvm *vm, const char *msg);

BERRY_API void be_newlist(bvm *vm);
BERRY_API void be_newmap(bvm *vm);
BERRY_API void be_newmodule(bvm *vm);
BERRY_API void be_getglobal(bvm *vm, const char *name);
BERRY_API void be_getbuiltin(bvm *vm, const char *name);
BERRY_API bbool be_setmember(bvm *vm, int index, const char *k);
BERRY_API bbool be_getmember(bvm *vm, int index, const char *k);
BERRY_API bbool be_getmethod(bvm *vm, int index, const char *k);
BERRY_API bbool be_getindex(bvm *vm, int index);
BERRY_API bbool be_setindex(bvm *vm, int index);
BERRY_API void be_getupval(bvm *vm, int index, int pos);
BERRY_API void be_setupval(bvm *vm, int index, int pos);
BERRY_API void be_getsuper(bvm *vm, int index);
BERRY_API int be_data_size(bvm *vm, int index);
BERRY_API void be_data_append(bvm *vm, int index);
BERRY_API bbool be_data_insert(bvm *vm, int index);
BERRY_API bbool be_data_remove(bvm *vm, int index);
BERRY_API void be_data_resize(bvm *vm, int index);
BERRY_API int be_iter_next(bvm *vm, int index);
BERRY_API bbool be_iter_hasnext(bvm *vm, int index);
BERRY_API bbool be_refcontains(bvm *vm, int index);
BERRY_API void be_refpush(bvm *vm, int index);
BERRY_API void be_refpop(bvm *vm);
BERRY_API void be_stack_require(bvm *vm, int count);

BERRY_API int be_returnvalue(bvm *vm);
BERRY_API int be_returnnilvalue(bvm *vm);
BERRY_API void be_call(bvm *vm, int argc);
BERRY_API int be_pcall(bvm *vm, int argc);
BERRY_API void be_raise(bvm *vm);
BERRY_API void be_exit(bvm *vm, int status);

/* raise exception APIs */
BERRY_API void be_stop_iteration(bvm *vm);

BERRY_API void be_regfunc(bvm *vm, const char *name, bntvfunc f);
BERRY_API void be_regclass(bvm *vm, const char *name, const bnfuncinfo *lib);

BERRY_API bvm* be_vm_new(void);
BERRY_API void be_vm_delete(bvm *vm);

BERRY_API int be_loadbuffer(bvm *vm,
    const char *name, const char *buffer, size_t length);
BERRY_API int be_loadfile(bvm *vm, const char *name);
BERRY_API int be_loadlib(bvm *vm, const char *path);
BERRY_API int be_loadexec(bvm *vm, const char *name);
BERRY_API int be_saveexec(bvm *vm, const char *name);
BERRY_API void be_codedump(bvm *vm, int index);

/* module path list APIs */
BERRY_API void be_module_path(bvm *vm);
BERRY_API void be_module_path_set(bvm *vm, const char *path);

/* basic character IO APIs */
BERRY_API void be_writebuffer(const char *buffer, size_t length);
BERRY_API char* be_readstring(char *buffer, size_t size);

#endif
