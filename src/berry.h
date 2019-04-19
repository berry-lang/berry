#ifndef BERRY_H
#define BERRY_H

#include <stddef.h>
#include <stdint.h>

#include "berry_conf.h"

#define BERRY_VERSION   "0.0.6"

#if BE_STACK_TOTAL_MAX < BE_STACK_FREE_MIN * 2
#error "The value of the macro BE_STACK_TOTAL_MAX is too small."
#endif

#if BE_INTGER_TYPE == 0
  #define BE_INTEGER    int
  #define BE_INTFORMAT  "%d"
#elif BE_INTGER_TYPE == 1
  #define BE_INTEGER    long
  #define BE_INTFORMAT  "%ld"
#elif BE_INTGER_TYPE == 2
  #ifdef _WIN32
    #define BE_INTEGER    __int64
    #define BE_INTFORMAT  "%I64d"
  #else
    #define BE_INTEGER    long long
    #define BE_INTFORMAT  "%lld"
  #endif
#else
  #error "Unsupported integer type for `bint`."
#endif

typedef uint8_t         bbyte;
typedef BE_INTEGER      bint;

#if BE_SINGLE_FLOAT != 0
    typedef float       breal;
#else
    typedef double      breal;
#endif

typedef enum {
    bfalse = 0,
    btrue = 1
} bbool;

enum berrorcode {
    BE_OK = 0,
    BE_IO_ERROR,
    BE_SYNTAX_ERROR,
    BE_EXEC_ERROR,
    BE_MALLOC_FAIL,
    BE_EXIT
};

#define BE_CNIL                 0
#define BE_CINT                 1
#define BE_CREAL                2
#define BE_CBOOL                3
#define BE_CFUNCTION            4
#define BE_CSTRING              5
#define BE_CMODULE              6

typedef struct bvm bvm;
typedef int (*bntvfunc)(bvm*);

typedef struct {
    const char *name;
    bntvfunc function;
} bnfuncinfo;

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

typedef const struct bntvmodule {
    const char *name;
    bntvmodule_obj *table;
    size_t size;
} bntvmodule;

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
    static bntvmodule_obj name[] =

#define be_extern_native_module(name)               \
    extern bntvmodule be_native_module(name)

#define be_define_native_module(_name, _attrs)      \
bntvmodule be_native_module(_name) = {              \
    .name = #_name,                                 \
    .table = (_attrs),                              \
    .size = (sizeof(_attrs) / sizeof((_attrs)[0]))  \
}

#if !BE_DEBUG
  #if defined(be_assert)
    #undef be_assert
  #endif
  #define be_assert(expr)       ((void)0)
#endif

#define be_writestring(s)       be_fwrite(stdout, s, strlen(s))
#define be_writenewline()       be_fwrite(stdout, "\n", 1)

#define be_return(vm)           return be_returnvalue(vm)
#define be_return_nil(vm)       return be_returnnilvalue(vm)

#define be_loadstring(vm, str) \
    be_loadbuffer((vm), "string", (str), strlen(str))

#define be_dostring(vm, s) \
    (be_loadstring((vm), (s)) || be_pcall((vm), 0))

bint be_str2int(const char *str, const char **endstr);
breal be_str2real(const char *str, const char **endstr);
const char* be_str2num(bvm *vm, const char *str);

int be_top(bvm *vm);
const char* be_typename(bvm *vm, int index);
const char* be_classname(bvm *vm, int index);
int be_strlen(bvm *vm, int index);
void be_strconcat(bvm *vm, int index);
void be_pop(bvm *vm, int n);
void be_remove(bvm *vm, int index);
int be_absindex(bvm *vm, int index);

int be_isnil(bvm *vm, int index);
int be_isbool(bvm *vm, int index);
int be_isint(bvm *vm, int index);
int be_isreal(bvm *vm, int index);
int be_isnumber(bvm *vm, int index);
int be_isstring(bvm *vm, int index);
int be_isclosure(bvm *vm, int index);
int be_isntvclos(bvm *vm, int index);
int be_isfunction(bvm *vm, int index);
int be_isproto(bvm *vm, int index);
int be_isclass(bvm *vm, int index);
int be_isinstance(bvm *vm, int index);
int be_islist(bvm *vm, int index);
int be_ismap(bvm *vm, int index);
int be_iscomptr(bvm *vm, int index);

bint be_toint(bvm *vm, int index);
breal be_toreal(bvm *vm, int index);
int be_toindex(bvm *vm, int index);
int be_tobool(bvm *vm, int index);
const char* be_tostring(bvm *vm, int index);
void* be_tocomptr(bvm *vm, int index);
void be_moveto(bvm *vm, int from, int to);
void be_pushnil(bvm *vm);
void be_pushbool(bvm *vm, int b);
void be_pushint(bvm *vm, bint i);
void be_pushreal(bvm *vm, breal r);
void be_pushstring(bvm *vm, const char *str);
void be_pushnstring(bvm *vm, const char *str, size_t n);
const char* be_pushfstring(bvm *vm, const char *format, ...);
void be_pushvalue(bvm *vm, int index);
void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);
void be_pushntvfunction(bvm *vm, bntvfunc f);
void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);
void be_pushcomptr(bvm *vm, void *ptr);
int be_pushiter(bvm *vm, int index);
void be_pusherror(bvm *vm, const char *msg);

void be_newlist(bvm *vm);
void be_newmap(bvm *vm);
void be_getglobal(bvm *vm, const char *name);
void be_setmember(bvm *vm, int index, const char *k);
void be_getmember(bvm *vm, int index, const char *k);
void be_getindex(bvm *vm, int index);
void be_setindex(bvm *vm, int index);
void be_getupval(bvm *vm, int index, int pos);
void be_setupval(bvm *vm, int index, int pos);
void be_getsuper(bvm *vm, int index);
int be_data_size(bvm *vm, int index);
void be_data_append(bvm *vm, int index);
int be_data_insert(bvm *vm, int index);
int be_data_remove(bvm *vm, int index);
void be_data_resize(bvm *vm, int index);
int be_iter_next(bvm *vm, int index);
int be_iter_hasnext(bvm *vm, int index);
int be_refcontains(bvm *vm, int index);
void be_refpush(bvm *vm, int index);
void be_refpop(bvm *vm);
void be_stack_require(bvm *vm, int count);

int be_returnvalue(bvm *vm);
int be_returnnilvalue(bvm *vm);
void be_call(bvm *vm, int argc);
int be_pcall(bvm *vm, int argc);
void be_abort(void);
void be_exit(bvm *vm, int status);

void be_regfunc(bvm *vm, const char *name, bntvfunc f);
void be_regclass(bvm *vm, const char *name, const bnfuncinfo *lib);

bvm* be_vm_new(void);
void be_vm_delete(bvm *vm);

int be_loadbuffer(bvm *vm,
    const char *name, const char *buffer, size_t length);
int be_loadfile(bvm *vm, const char *name);
void be_loadlibs(bvm *vm);

#endif
