/**
 * @file       be_object.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_OBJECT_H
#define BE_OBJECT_H

#include "berry.h"

/**
 * @note basic types, do not change value
 * @{
 */
#define BE_NONE        -256                                   /**< unknown type */
#define BE_NIL          0                                     /**< BE_NIL */
#define BE_INT          1                                     /**< BE_INT */
#define BE_REAL         2                                     /**< BE_REAL */
#define BE_BOOL         3                                     /**< BE_BOOL */
#define BE_COMPTR       4                                     /**< common pointer */
#define BE_INDEX        5                                     /**< index for instance variable, previously BE_INT */
#define BE_FUNCTION     6                                     /**< BE_FUNCTION */

#define BE_GCOBJECT     16                                    /**< from this type can be gced */
#define BE_GCOBJECT_MAX (3<<5)                                /**< from this type can't be gced */

#define BE_STRING       16                                    /**< BE_STRING */
#define BE_CLASS        17                                    /**< BE_CLASS */
#define BE_INSTANCE     18                                    /**< BE_INSTANCE */
#define BE_PROTO        19                                    /**< BE_PROTO */
#define BE_LIST         20                                    /**< BE_LIST */
#define BE_MAP          21                                    /**< BE_MAP */
#define BE_MODULE       22                                    /**< BE_MODULE */
#define BE_COMOBJ       23                                    /**< common object */

#define BE_NTVFUNC      ((0 << 5) | BE_FUNCTION)              /**< BE_NTVFUNC */
#define BE_CLOSURE      ((1 << 5) | BE_FUNCTION)              /**< BE_CLOSURE */
#define BE_NTVCLOS      ((2 << 5) | BE_FUNCTION)              /**< BE_NTVCLOS */
#define BE_CTYPE_FUNC   ((3 << 5) | BE_FUNCTION)              /**< BE_CTYPE_FUNC */
#define BE_STATIC       (1 << 7)                              /**< BE_STATIC */

#define func_isstatic(o)       (((o)->type & BE_STATIC) != 0) /**< func_isstatic */
#define func_setstatic(o)      ((o)->type |= BE_STATIC)       /**< func_setstatic */
#define func_clearstatic(o)    ((o)->type &= ~BE_STATIC)      /**< func_clearstatic */
/** @} */

#define BE_VA_VARARG    (1 << 0)                      /**< value for bproto.varg: function has variable number of arguments */
#define BE_VA_METHOD    (1 << 1)                      /**< value for bproto.varg: function is a method (this is only a hint) */
#define array_count(a)   (sizeof(a) / sizeof((a)[0])) /**< value for bproto.varg: array_count */

/**
 * @def bcommon_header
 * @brief bcommon_header
 *
 */
#define bcommon_header          \
    struct bgcobject *next;     \
    bbyte type;                 \
    bbyte marked

/**
 * @def bstring_header
 * @brief bstring_header
 *
 */
#define bstring_header          \
    bcommon_header;             \
    bbyte extra;                \
    bbyte slen

/**
 * @struct bgcobject
 * @brief bgcobject
 *
 */
typedef struct bgcobject {
    bcommon_header; /**< bcommon_header */
} bgcobject;        /**< bgcobject */

typedef struct bclosure bclosure;   /**< bclosure */
typedef struct bntvclos bntvclos;   /**< bntvclos */
typedef struct bclass bclass;       /**< bclass */
typedef struct binstance binstance; /**< binstance */
typedef struct blist blist;         /**< blist */
typedef struct bmap bmap;           /**< bmap */
typedef struct bupval bupval;       /**< bupval */

typedef uint32_t binstruction;      /**< binstruction */

/**
 * @struct bstring
 * @brief bstring
 *
 */
typedef struct bstring {
    bstring_header; /**< bstring_header */
} bstring;          /**< bstring */

/* the definition of the vector and stack data structures.
 * in fact, the stack is implemented by vector. */
typedef struct bvector {
    int capacity;   /**< the number of elements that the vector can store */
    int size;       /**< the size of each element (bytes) */
    int count;      /**< number of elements of the vector */
    void *data;     /**< the data block pointer, if vector is empty,
                       it will point to the first element */
    void *end;      /**< pointer to the last element, if the vector is empty,
                       the end pointer will be smaller than the data pointer */
} bvector, bstack;  /**< bvector */  /**< bstack */

/**
 * @note berry value data union, a berry value is always described
 * by the data structure contained in the bvaldata union.
 * @{
 */
union bvaldata {
    bbool b;        /**< boolean */
    breal r;        /**< real number */
    bint i;         /**< integer number */
    void *p;        /**< object pointer */
    const void *c;  /**< const object pointer */
    bstring *s;     /**< string pointer */
    bgcobject *gc;  /**< GC object */
    bntvfunc nf;    /**< native C function */
#ifdef __cplusplus
    BE_CONSTEXPR bvaldata(bbool v) : b(v) {}       /**< bvaldata(bbool v) */
    BE_CONSTEXPR bvaldata(breal v) : r(v) {}       /**< bvaldata(breal v) */
    BE_CONSTEXPR bvaldata(bint v) : i(v) {}        /**< bvaldata(bint v) */
    BE_CONSTEXPR bvaldata(void *v) : p(v) {}       /**< bvaldata(void *v) */
    BE_CONSTEXPR bvaldata(const void *v) : c(v) {} /**< bvaldata(const void *v) */
    BE_CONSTEXPR bvaldata(bntvfunc v) : nf(v) {}   /**< bvaldata(bntvfunc v)  */
#endif
};
/** @} */

/**
 * @struct bvalue
 * @brief bvalue
 *
 * berry value. for simple types, the value of the data is stored,
 * while the complex type stores a reference to the data
 *
 */
typedef struct bvalue {
    union bvaldata v; /**< the value data */
    int type;         /**< the value type */
} bvalue; /**< bvalue */

/**
 * @typedef bupvaldesc
 * @brief bupvaldesc
 *
 */
typedef struct {
#if BE_DEBUG_VAR_INFO
    bstring *name; /**< name */
#endif
    bbyte instack; /**< instack */
    bbyte idx;     /**< idx */
} bupvaldesc;

/**
 * @typedef blineinfo
 * @brief blineinfo
 *
 */
typedef struct {
#if BE_DEBUG_RUNTIME_INFO > 1
    uint16_t linenumber; /**< linenumber */
    uint16_t endpc;      /**< endpc */
#else
    int linenumber;      /**< linenumber */
    int endpc;           /**< endpc */
#endif
} blineinfo;

/**
 * @typedef bvarinfo
 * @brief bvarinfo
 *
 */
typedef struct {
    bstring *name; /**<  */
#if BE_DEBUG_RUNTIME_INFO > 1
    uint16_t beginpc; /**< beginpc */
    uint16_t endpc;   /**< endpc */
#else
    int beginpc;      /**< beginpc */
    int endpc;        /**< endpc */
#endif
} bvarinfo;

/**
 * @struct bproto
 * @brief bproto
 *
 */
typedef struct bproto {
    bcommon_header;       /**< bcommon_header */
    bbyte nstack;         /**< number of stack size by this function */
    bbyte nupvals;        /**< upvalue count */
    bbyte argc;           /**< argument count */
    bbyte varg;           /**< variable argument position + 1 */
    bgcobject *gray;      /**< for gc gray list */
    bupvaldesc *upvals;   /**< upvals */
    bvalue *ktab;         /**< constants table */
    struct bproto **ptab; /**< proto table */
    binstruction *code;   /**< instructions sequence */
    bstring *name;        /**< function name */
    int codesize;         /**< code size */
    int nconst;           /**< constants count */
    int nproto;           /**< proto count */
    bstring *source;      /**< source file name */
#if BE_DEBUG_RUNTIME_INFO /* debug information */
    blineinfo *lineinfo;  /**< lineinfo */
    int nlineinfo;        /**< nlineinfo */
#endif
#if BE_DEBUG_VAR_INFO
    bvarinfo *varinfo;    /**< varinfo */
    int nvarinfo;         /**< nvarinfo */
#endif
} bproto;                 /**< bproto */

/**
 * @struct bclosure
 * @brief berry closure
 *
 */
struct bclosure {
    bcommon_header;    /**< bcommon_header */
    bbyte nupvals;     /**< nupvals */
    bgcobject *gray;   /**< for gc gray list */
    bproto *proto;     /**< proto */
    bupval *upvals[1]; /**< upvals */
};

/**
 * @struct bntvclos
 * @brief C native function or closure
 *
 */
struct bntvclos {
    bcommon_header;  /**< bcommon_header */
    bbyte nupvals;   /**< nupvals */
    bgcobject *gray; /**< for gc gray list */
    bntvfunc f;      /**< f */
};

/**
 * @typedef bcommomobj
 * @brief common object
 *
 */
typedef struct {
    bcommon_header;   /**< bcommon_header */
    void *data;       /**< data */
    bntvfunc destroy; /**< destroy */
} bcommomobj;

/**
 * @fn const char* (*breader)(void*, size_t*)
 * @brief (???)
 *
 * @param  (???)
 * @param  (???)
 * @return (???)
 */
typedef const char* (*breader)(void*, size_t*);

#define cast(_T, _v)            ((_T)(_v))       /**< cast */
#define cast_int(_v)            cast(int, _v)    /**< cast_int */
#define cast_bool(_v)           cast(bbool, _v)  /**< cast_bool */
#define basetype(_t)            ((_t) & 0x1F)    /**< basetype */

#define var_type(_v)            ((_v)->type)                               /**< var_type */
#define var_basetype(_v)        basetype((_v)->type)                       /**< var_basetype */
#define var_primetype(_v)       (var_type(_v) & ~BE_STATIC)                /**< var_primetype */
#define var_isstatic(_v)        ((var_type(_v) & BE_STATIC) == BE_STATIC)  /**< var_isstatic */
#define var_istype(_v, _t)      (var_primetype(_v) == _t)                  /**< var_istype */
#define var_settype(_v, _t)     ((_v)->type = _t)                          /**< var_settype */
#define var_markstatic(_v)      var_settype(_v, var_type(_v) | BE_STATIC)  /**< var_markstatic */
#define var_clearstatic(_v)     var_settype(_v, var_type(_v) & ~BE_STATIC) /**< var_clearstatic */
#define var_setobj(_v, _t, _o)  { (_v)->v.p = _o; var_settype(_v, _t); }   /**< var_setobj */

#define var_isnil(_v)           var_istype(_v, BE_NIL)            /**< var_isnil */
#define var_isbool(_v)          var_istype(_v, BE_BOOL)           /**< var_isbool */
#define var_isint(_v)           var_istype(_v, BE_INT)            /**< var_isint */
#define var_isreal(_v)          var_istype(_v, BE_REAL)           /**< var_isreal */
#define var_isstr(_v)           var_istype(_v, BE_STRING)         /**< var_isstr */
#define var_isclosure(_v)       var_istype(_v, BE_CLOSURE)        /**< var_isclosure */
#define var_isntvclos(_v)       var_istype(_v, BE_NTVCLOS)        /**< var_isntvclos */
#define var_isntvfunc(_v)       var_istype(_v, BE_NTVFUNC)        /**< var_isntvfunc */
#define var_isfunction(_v)      (var_basetype(_v) == BE_FUNCTION) /**< var_isfunction */
#define var_isproto(_v)         var_istype(_v, BE_PROTO)          /**< var_isproto */
#define var_isclass(_v)         var_istype(_v, BE_CLASS)          /**< var_isclass */
#define var_isinstance(_v)      var_istype(_v, BE_INSTANCE)       /**< var_isinstance */
#define var_islist(_v)          var_istype(_v, BE_LIST)           /**< var_islist */
#define var_ismap(_v)           var_istype(_v, BE_MAP)            /**< var_ismap */
#define var_ismodule(_v)        var_istype(_v, BE_MODULE)         /**< var_ismodule */
#define var_isindex(_v)         var_istype(_v, BE_INDEX)          /**< var_isindex */
#define var_iscomptr(_v)        var_istype(_v, BE_COMPTR)         /**< var_iscomptr */
#define var_isnumber(_v)        (var_isint(_v) || var_isreal(_v)) /**< var_isnumber */

#define var_setnil(_v)          var_settype(_v, BE_NIL)                                /**< var_setnil */
#define var_setval(_v, _s)      (*(_v) = *(_s))                                        /**< var_setval */
#define var_setbool(_v, _b)     { var_settype(_v, BE_BOOL); (_v)->v.b = (bbool)(_b); } /**< var_setbool */
#define var_setint(_v, _i)      { var_settype(_v, BE_INT); (_v)->v.i = (_i); }         /**< var_setint */
#define var_setreal(_v, _r)     { var_settype(_v, BE_REAL); (_v)->v.r = (_r); }        /**< var_setreal */
#define var_setstr(_v, _s)      var_setobj(_v, BE_STRING, _s)                          /**< var_setstr */
#define var_setinstance(_v, _o) var_setobj(_v, BE_INSTANCE, _o)                        /**< var_setinstance */
#define var_setclass(_v, _o)    var_setobj(_v, BE_CLASS, _o)                           /**< var_setclass */
#define var_setclosure(_v, _o)  var_setobj(_v, BE_CLOSURE, _o)                         /**< var_setclosure */
#define var_setntvclos(_v, _o)  var_setobj(_v, BE_NTVCLOS, _o)                         /**< var_setntvclos */
#define var_setntvfunc(_v, _o)  { (_v)->v.nf = (_o); var_settype(_v, BE_NTVFUNC); }    /**< var_setntvfunc */
#define var_setlist(_v, _o)     var_setobj(_v, BE_LIST, _o)                            /**< var_setlist */
#define var_setmap(_v, _o)      var_setobj(_v, BE_MAP, _o)                             /**< var_setmap */
#define var_setmodule(_v, _o)   var_setobj(_v, BE_MODULE, _o)                          /**< var_setmodule */
#define var_setindex(_v, _i)    { var_settype(_v, BE_INDEX); (_v)->v.i = (_i); }       /**< var_setindex */
#define var_setproto(_v, _o)    var_setobj(_v, BE_PROTO, _o)                           /**< var_setproto */

#define var_tobool(_v)          ((_v)->v.b)             /**< var_tobool */
#define var_toint(_v)           ((_v)->v.i)             /**< var_toint */
#define var_toreal(_v)          ((_v)->v.r)             /**< var_toreal */
#define var_tostr(_v)           ((_v)->v.s)             /**< var_tostr */
#define var_togc(_v)            ((_v)->v.gc)            /**< var_togc */
#define var_toobj(_v)           ((_v)->v.p)             /**< var_toobj */
#define var_tontvfunc(_v)       ((_v)->v.nf)            /**< var_tontvfunc */
#define var_toidx(_v)           cast_int(var_toint(_v)) /**< var_toidx */

/**
 * @fn const char be_vtype2str*(bvalue*)
 * @brief (???)
 *
 * @param v (???)
 * @return (???)
 */
const char* be_vtype2str(bvalue *v);

/**
 * @fn bvalue be_indexof*(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param idx (???)
 * @return (???)
 */
bvalue* be_indexof(bvm *vm, int idx);

/**
 * @fn void be_commonobj_delete(bvm*, bgcobject*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 */
void be_commonobj_delete(bvm *vm, bgcobject *obj);

/**
 * @fn int be_commonobj_destroy_generic(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
int be_commonobj_destroy_generic(bvm* vm);

#endif
