#ifndef __BE_OBJECT_H
#define __BE_OBJECT_H

#include "berry.h"

typedef enum {
    VT_NIL = 0,
    VT_INT,
    VT_REAL,
    VT_BOOL,
    VT_NOTMETHOD,
    /* gc objects */
    VT_FUNCTION,
    VT_PROTO,
    VT_CLASS,
    VT_INSTANCE,
    VT_STRING,
    VT_LIST,
    VT_MAP
} bvaluetype;

#define VT_CLOSURE      ((0 << 5) | VT_FUNCTION)
#define VT_NTVFUNC      ((1 << 5) | VT_FUNCTION)

#define bcommon_header        \
    struct bgcobject *next;   \
    bbyte type;               \
    bbyte marked

typedef struct bgcobject {
    bcommon_header;
} bgcobject;

typedef struct bgc bgc;
typedef struct bstringtable bstringtable;
typedef struct bclosure bclosure;
typedef struct bntvfunc bntvfunc;
typedef struct bclass bclass;
typedef struct bobject bobject;
typedef struct blist blist;
typedef struct bmap bmap;

typedef struct bstring {
    bcommon_header;
    bbyte extra;
    bbyte slen; /* short string length */
    uint32_t hash;
    union {
        int llen; /* long string length */
        struct bstring *next;
    } u;
    char s[1];
} bstring;

typedef struct bvalue {
    union {
        bbool b; /* boolean */
        breal r; /* real number */
        bint i; /* integer number */
        void *p; /* object pointer */
        bstring *s; /* string pointer */
        bgcobject *gc; /* GC object */
    } v;
    int type;
} bvalue;

typedef int (*bcfunction)(bvm *vm);

typedef struct {
  bstring *name;
  int startpc;  /* first point where variable is active */
  int endpc;    /* first point where variable is dead */
} blocalval;

typedef struct {
    bstring *name;
    int index;
} bvaldesc;

typedef struct {
    bbyte instack;
    bbyte idx;
} bupvaldesc;

typedef struct bupval {
    bvalue *value;
    union {
        bvalue value;
        struct bupval *next;
    } u;
    int refcnt;
} bupval;

typedef struct bproto {
    bcommon_header;
    bupvaldesc *upvals;
    bvalue *ktab; /* constants table */
    struct bproto **ptab; /* proto table */
    binstruction *code; /* instructions sequence */
    bstring *name; /* function name */
    int codesize; /* code size */
    int nconst; /* constants count */
    int nproto; /* proto count */
    bbyte nlocal; /* local variable count */
    bbyte nupvals; /* upvalue count */
    char argc; /* argument count */
} bproto;

struct bclosure {
    bcommon_header;
    bbyte nupvals;
    bproto *proto;
    bupval *upvals[1];
};

/* C native function or closure */
struct bntvfunc {
    bcommon_header;
    char argc; /* argument count */
    bbyte nupvals;
    bcfunction f;
};

#define var_type(_v)            ((_v)->type)
#define var_istype(_v, _t)      (var_type(_v) == _t)
#define var_settype(_v, _T)     ((_v)->type = (bbyte)_T)
#define var_setobj(_v, _T, _o)  { var_settype(_v, _T); (_v)->v.p = _o; }

#define var_isnil(_v)           var_istype(_v, VT_NIL)
#define var_isbool(_v)          var_istype(_v, VT_BOOL)
#define var_isint(_v)           var_istype(_v, VT_INT)
#define var_isreal(_v)          var_istype(_v, VT_REAL)
#define var_isclass(_v)         var_istype(_v, VT_CLASS)
#define var_isinstance(_v)      var_istype(_v, VT_INSTANCE)

#define var_setnil(_v)          var_settype(_v, VT_NIL)
#define var_setbool(_v, _b)     { var_settype(_v, VT_BOOL); (_v)->v.b = _b; }
#define var_setint(_v, _i)      { var_settype(_v, VT_INT); (_v)->v.i = _i; }
#define var_setreal(_v, _r)     { var_settype(_v, VT_REAL); (_v)->v.r = _r; }
#define var_setstr(_v, _s)      var_setobj(_v, VT_STRING, _s)
#define var_setinstance(_v, _o) var_setobj(_v, VT_INSTANCE, _o)
#define var_setclass(_v, _o)    var_setobj(_v, VT_CLASS, _o)
#define var_setclosure(_v, _o)  var_setobj(_v, VT_CLOSURE, _o)
#define var_setntvfunc(_v, _o)  var_setobj(_v, VT_NTVFUNC, _o)

#define var_getbool(_v)         ((_v)->v.b)
#define var_getint(_v)          ((_v)->v.i)
#define var_getreal(_v)         ((_v)->v.r)
#define var_getstr(_v)          ((_v)->v.s)
#define var_getgc(_v)           ((_v)->v.gc)
#define var_getobj(_v)          ((_v)->v.p)

#endif
