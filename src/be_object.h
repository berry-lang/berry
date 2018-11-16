#ifndef __BE_OBJECT_H
#define __BE_OBJECT_H

#include "berry.h"

#define BE_CLOSURE      ((0 << 5) | BE_FUNCTION)
#define BE_NTVFUNC      ((1 << 5) | BE_FUNCTION)

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
typedef struct binstance binstance;
typedef struct blist blist;
typedef struct bmap bmap;

typedef uint32_t binstruction;

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

#define cast(_T, _v)            ((_T)_v)

#define var_type(_v)            ((_v)->type)
#define var_basetype(_v)        ((_v)->type & 0x1F)
#define var_istype(_v, _t)      (var_type(_v) == _t)
#define var_settype(_v, _t)     ((_v)->type = (bbyte)_t)
#define var_setobj(_v, _t, _o)  { (_v)->v.p = _o; var_settype(_v, _t); }

#define var_isnil(_v)           var_istype(_v, BE_NIL)
#define var_isbool(_v)          var_istype(_v, BE_BOOL)
#define var_isint(_v)           var_istype(_v, BE_INT)
#define var_isreal(_v)          var_istype(_v, BE_REAL)
#define var_isstring(_v)        var_istype(_v, BE_STRING)
#define var_isclosure(_v)       var_istype(_v, BE_CLOSURE)
#define var_isntvfunc(_v)       var_istype(_v, BE_NTVFUNC)
#define var_isfunction(_v)      (var_basetype(_v) == BE_FUNCTION)
#define var_isproto(_v)         var_istype(_v, BE_PROTO)
#define var_isclass(_v)         var_istype(_v, BE_CLASS)
#define var_isinstance(_v)      var_istype(_v, BE_INSTANCE)
#define var_islist(_v)          var_istype(_v, BE_LIST)
#define var_ismap(_v)           var_istype(_v, BE_MAP)
#define var_isnumber(_v)        (var_isint(_v) || var_isreal(_v))

#define var_setnil(_v)          var_settype(_v, BE_NIL)
#define var_setval(_v, _s)      (*(_v) = *(_s))
#define var_setbool(_v, _b)     { var_settype(_v, BE_BOOL); (_v)->v.b = _b; }
#define var_setint(_v, _i)      { var_settype(_v, BE_INT); (_v)->v.i = _i; }
#define var_setreal(_v, _r)     { var_settype(_v, BE_REAL); (_v)->v.r = _r; }
#define var_setstr(_v, _s)      var_setobj(_v, BE_STRING, _s)
#define var_setinstance(_v, _o) var_setobj(_v, BE_INSTANCE, _o)
#define var_setclass(_v, _o)    var_setobj(_v, BE_CLASS, _o)
#define var_setclosure(_v, _o)  var_setobj(_v, BE_CLOSURE, _o)
#define var_setntvfunc(_v, _o)  var_setobj(_v, BE_NTVFUNC, _o)

#define var_tobool(_v)          ((_v)->v.b)
#define var_toint(_v)           ((_v)->v.i)
#define var_toreal(_v)          ((_v)->v.r)
#define var_tostr(_v)           ((_v)->v.s)
#define var_togc(_v)            ((_v)->v.gc)
#define var_toobj(_v)           ((_v)->v.p)

#endif
