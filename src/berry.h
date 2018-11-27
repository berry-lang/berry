#ifndef __BERRY_H
#define __BERRY_H

#include <stddef.h>
#include <stdint.h>

#define BERRY_DEBUG     0

#define BERRY_VERSION   "0.0.1"

typedef uint8_t         bbyte;
typedef double          breal;
typedef int             bint;

typedef enum {
    bfalse = 0,
    btrue = 1
} bbool;

/* basic types, do not change value */
#define BE_NONE         (-1)
#define BE_NIL          0
#define BE_INT          1
#define BE_REAL         2
#define BE_BOOL         3
#define BE_FUNCTION     10
#define BE_STRING       11
#define BE_CLASS        12
#define BE_INSTANCE     13

typedef struct bvm bvm;
typedef int (*bcfunction)(bvm *vm);

typedef struct {
    const char *name;
    bcfunction function;
} bmemberinfo;

int be_top(bvm *vm);
int be_type(bvm *vm, int index);
const char* be_typename(bvm *vm, int index);
const char* be_classname(bvm *vm, int index);
void be_pop(bvm *vm, int n);
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

int be_toint(bvm *vm, int index);
breal be_toreal(bvm *vm, int index);
bbool be_tobool(bvm *vm, int index);
const char* be_tostring(bvm *vm, int index);
void be_moveto(bvm *vm, int index);
void be_pushnil(bvm *vm);
void be_pushbool(bvm *vm, int b);
void be_pushint(bvm *vm, bint i);
void be_pushreal(bvm *vm, breal r);
void be_pushstring(bvm *vm, const char *str);
void be_pushfstring(bvm *vm, const char *format, ...);
void be_pushvalue(bvm *vm, int index);
void be_pushntvclosure(bvm *vm, bcfunction f, int nupvals);
void be_pushntvfunction(bvm *vm, bcfunction f);
void be_pushclass(bvm *vm, const char *name, const bmemberinfo *lib);
void be_getsuper(bvm *vm, int index);

void be_newlist(bvm *vm);
void be_newmap(bvm *vm);
void be_setmember(bvm *vm, int index, const char *k);
void be_getmember(bvm *vm, int index, const char *k);
void be_getindex(bvm *vm, int index);
void be_setindex(bvm *vm, int index);
void be_getupval(bvm *vm, int index, int pos);
void be_setupval(bvm *vm, int index, int pos);
void be_getfunction(bvm *vm);
void be_getsize(bvm *vm, int index);
int be_size(bvm *vm, int index);
void be_append(bvm *vm, int index);
void be_insert(bvm *vm, int index);
void be_remove(bvm *vm, int index);
void be_resize(bvm *vm, int index);
int be_pushiter(bvm *vm, int index);
int be_next(bvm *vm, int index);
int be_hasnext(bvm *vm, int index);
int be_return(bvm *vm);
int be_returnnil(bvm *vm);

void be_call(bvm *vm, int argc);
int be_pcall(bvm *vm, int argc);
void be_printf(const char *format, ...);
void be_printvalue(bvm *vm, int quote, int index);
void be_abort(void);

void be_regcfunc(bvm *vm, const char *name, bcfunction f);
void be_regclass(bvm *vm, const char *name, const bmemberinfo *lib);

bvm* be_newvm(int nstack);

int be_loadstring(bvm *vm, const char *str);
void be_loadlibs(bvm *vm);

#endif
