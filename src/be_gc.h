/**
 * @file       be_gc.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_GC_H
#define BE_GC_H

#include "be_object.h"

#define gc_object(o)        cast(bgcobject*, o)                        /**< gc_object */
#define gc_cast(o, t, T)    ((o) && (o)->type == (t) ? (T*)(o) : NULL) /**< gc_cast */
#define cast_proto(o)       gc_cast(o, BE_PROTO, bproto)               /**< cast_proto */
#define cast_closure(o)     gc_cast(o, BE_CLOSURE, bclosure)           /**< cast_closure */
#define cast_ntvclos(o)     gc_cast(o, BE_NTVCLOS, bntvclos)           /**< cast_ntvclos */
#define cast_str(o)         gc_cast(o, BE_STRING, bstring)             /**< cast_str */
#define cast_class(o)       gc_cast(o, BE_CLASS, bclass)               /**< cast_class */
#define cast_instance(o)    gc_cast(o, BE_INSTANCE, binstance)         /**< cast_instance */
#define cast_map(o)         gc_cast(o, BE_MAP, bmap)                   /**< cast_map */
#define cast_list(o)        gc_cast(o, BE_LIST, blist)                 /**< cast_list */
#define cast_module(o)      gc_cast(o, BE_MODULE, bmodule)             /**< cast_module */

#define gc_ismark(o, m)     (((o)->marked & 0x03) == m)                /**< gc_ismark */
#define gc_iswhite(o)       gc_ismark((o), GC_WHITE)                   /**< gc_iswhite */
#define gc_isgray(o)        gc_ismark((o), GC_GRAY)                    /**< gc_isgray */
#define gc_isdark(o)        gc_ismark((o), GC_DARK)                    /**< gc_isdark */

/**
 * @def gc_setmark
 * @brief gc_setmark
 *
 */
#define gc_setmark(o, m) \
if (!gc_isconst(o)) { \
    (o)->marked &= ~0x03; \
    (o)->marked |= (m) & 0x03; \
}

#define gc_setwhite(o)      gc_setmark((o), GC_WHITE)                                               /**< gc_setwhite */
#define gc_setgray(o)       gc_setmark((o), GC_GRAY)                                                /**< gc_setgray */
#if BE_USE_PERF_COUNTERS
    #define gc_setdark(o)       { vm->counter_gc_kept++; gc_setmark((o), GC_DARK); }                /**< gc_setdark */
#else
    #define gc_setdark(o)       gc_setmark((o), GC_DARK)                                            /**< gc_setdark */
#endif
#define gc_isfixed(o)       (((o)->marked & GC_FIXED) != 0)                                         /**< gc_isfixed */
#define gc_setfixed(o)      ((o)->marked |= GC_FIXED)                                               /**< gc_setfixed */
#define gc_clearfixed(o)    ((o)->marked &= ~GC_FIXED)                                              /**< gc_clearfixed */
#define gc_isconst(o)       (((o)->marked & GC_CONST) != 0)                                         /**< gc_isconst */
#define gc_exmark(o)        (((o)->marked >> 4) & 0x0F)                                             /**< gc_exmark */
#define gc_setexmark(o, k)  ((o)->marked |= (k) << 4)                                               /**< gc_setexmark */

#define be_isgcobj(o)       (var_primetype(o) >= BE_GCOBJECT && var_primetype(o) < BE_GCOBJECT_MAX) /**< be_isgcobj */
#define be_gcnew(v, t, s)   be_newgcobj((v), (t), sizeof(s))                                        /**< be_gcnew */

#define set_fixed(s)        bbool _was_fixed = be_gc_fix_set(vm, cast(bgcobject*, (s)), 1)          /**< set_fixed */
#define restore_fixed(s)    be_gc_fix_set(vm, cast(bgcobject*, (s)), _was_fixed);                   /**< restore_fixed */

/**
 * @enum bgcmark
 * @brief GC mark
 *
 * the GC mark uses bit4:0 of the `object->marked` field,
 * so other bits can be used for special flags (ex-mark).
 *
 */
typedef enum {
    GC_WHITE = 0x00, /**< unreachable object */
     GC_GRAY = 0x01, /**< unscanned object */
     GC_DARK = 0x02, /**< scanned object */
    GC_FIXED = 0x04, /**< disable collection mark */
    GC_CONST = 0x08  /**< constant object mark */
} bgcmark;

/**
 * @fn void be_gc_init(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_gc_init(bvm *vm);

/**
 * @fn void be_gc_deleteall(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_gc_deleteall(bvm *vm);

/**
 * @fn void be_gc_setsteprate(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param rate (???)
 */
void be_gc_setsteprate(bvm *vm, int rate);

/**
 * @fn void be_gc_setpause(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param pause (???)
 */
void be_gc_setpause(bvm *vm, int pause);

/**
 * @fn size_t be_gc_memcount(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
size_t be_gc_memcount(bvm *vm);

/**
 * @fn bgcobject be_newgcobj*(bvm*, int, size_t)
 * @brief (???)
 *
 * @param vm (???)
 * @param type (???)
 * @param size (???)
 * @return (???)
 */
bgcobject *be_newgcobj(bvm *vm, int type, size_t size);

/**
 * @fn bgcobject be_gc_newstr*(bvm*, size_t, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param size (???)
 * @param islong (???)
 * @return (???)
 */
bgcobject* be_gc_newstr(bvm *vm, size_t size, int islong);

/**
 * @fn void be_gc_fix(bvm*, bgcobject*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 */
void be_gc_fix(bvm *vm, bgcobject *obj);

/**
 * @fn void be_gc_unfix(bvm*, bgcobject*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 */
void be_gc_unfix(bvm *vm, bgcobject *obj);

/**
 * @fn bool be_gc_fix_set(bvm*, bgcobject*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param fix (???)
 * @return (???)
 */
bbool be_gc_fix_set(bvm *vm, bgcobject *obj, bbool fix);

/**
 * @fn void be_gc_collect(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_gc_collect(bvm *vm);

/**
 * @fn void be_gc_auto(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_gc_auto(bvm *vm);

#endif
