#ifndef __BE_OBJECT_H
#define __BE_OBJECT_H

#include "berry.h"

typedef enum {
    VT_NIL = 0,
    VT_INT,
    VT_REAL,
    VT_BOOL,
    VT_NOTMETHOD,
    VT_FUNCTION,
    VT_PROTO,
    VT_CLASS,
    VT_INSTANCE,
    VT_STRING,
    VT_LIST,
    VT_MAP
} bvaluetype;

#define VT_CLOSURE      ((0 << 5) | VT_FUNCTION)
#define VT_PRIMFUNC     ((1 << 5) | VT_FUNCTION)

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
typedef struct bprimfunc bprimfunc;
typedef struct bclass bclass;
typedef struct bobject bobject;
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
        bstring *s;
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
    bstring *name; /* upvalue name (for debug infomation) */
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
    bbyte nstack; /* stack usage */
    bbyte nupvals; /* upvalue count */
    char argc; /* argument count */
} bproto;

struct bclosure {
    bcommon_header;
    bbyte nupvals;
    bgcobject *gclist;
    bproto *proto;
    bupval *upvals[1];
};

/* C primordial function */
struct bprimfunc {
    bcfunction f;
    char argc; /* argument count */
};

#define value_type(v)               ((v)->type)
#define value_settype(v, t)         ((v)->type = t)
#define value_istype(v, t)          (value_type(v) == t)
#define value_setnil(v)             value_settype(v, VT_NIL)
#define value_isnil(v)              value_istype(v, VT_NIL)

#endif
