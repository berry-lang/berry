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

typedef enum {
    BE_NIL = 0,
    BE_INT,
    BE_REAL,
    BE_BOOL,
    BE_NOTMETHOD,
    /* gc objects */
    BE_FUNCTION,
    BE_PROTO,
    BE_CLASS,
    BE_INSTANCE,
    BE_STRING,
    BE_LIST,
    BE_MAP
} bvaluetype;

typedef struct bvm bvm;
typedef int (*bcfunction)(bvm *vm);

typedef struct {
    const char *name;
    bcfunction function;
    bbyte argc;
} bfieldinfo;

int be_top(bvm *vm);
int be_type(bvm *vm, int index);
void be_pop(bvm *vm, int n);

int be_isnil(bvm *vm, int index);
int be_toint(bvm *vm, int index);
breal be_toreal(bvm *vm, int index);
bbool be_tobool(bvm *vm, int index);
const char* be_tostring(bvm *vm, int index);

void be_pushnil(bvm *vm);
void be_pushbool(bvm *vm, int b);
void be_pushint(bvm *vm, bint i);
void be_pushreal(bvm *vm, breal r);
void be_pushstring(bvm *vm, const char *str);
void be_pushvalue(bvm *vm, int index);
void be_pushntvclosure(bvm *vm, bcfunction f, int argc, int nupvals);

void be_getsuper(bvm *vm, int index);
void be_getobjtype(bvm *vm, int index);

void be_newlist(bvm *vm);
void be_setfield(bvm *vm, int index, const char *k);
void be_getfield(bvm *vm, int index, const char *k);
void be_getindex(bvm *vm, int index);
void be_setindex(bvm *vm, int index);
void be_getupval(bvm *vm, int index, int pos);
void be_setupval(bvm *vm, int index, int pos);
void be_getfunction(bvm *vm);
void be_getsize(bvm *vm, int index);
int be_size(bvm *vm, int index);
void be_append(bvm *vm, int index);
void be_resize(bvm *vm, int index);

int be_returnvalue(bvm *vm);

void be_call(bvm *vm, int argc);
void be_printf(const char *format, ...);
void be_printvalue(bvm *vm, int quote, int index);
void be_abort(void);

void be_regcfunc(bvm *vm, const char *name, bcfunction f, int argc);
void be_regclass(bvm *vm, const char *name, const bfieldinfo *lib);

#endif
