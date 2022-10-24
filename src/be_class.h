/**
 * @file       be_class.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_CLASS_H
#define BE_CLASS_H

#include "be_object.h"

#define MT_VARIABLE                     BE_INDEX                /**< MT_VARIABLE */
#define MT_METHOD                       BE_CLOSURE              /**< MT_METHOD */
#define MT_PRIMMETHOD                   BE_NTVFUNC              /**< MT_PRIMMETHOD */

#define be_class_name(cl)               ((cl)->name)            /**< be_class_name */
#define be_class_members(cl)            ((cl)->members)         /**< be_class_members */
#define be_class_super(cl)              ((cl)->super)           /**< be_class_super */
#define be_class_setsuper(self, sup)    ((self)->super = (sup)) /**< be_class_setsuper */
#define be_class_setsub(self, sub)      ((self)->sub = (sub))   /**< be_class_setsub */
#define be_instance_name(obj)           ((obj)->_class->name)   /**< be_instance_name */
#define be_instance_class(obj)          ((obj)->_class)         /**< be_instance_class */
#define be_instance_members(obj)        ((obj)->members)        /**< be_instance_members */
#define be_instance_member_count(obj)   ((obj)->_class->nvar)   /**< be_instance_member_count */
#define be_instance_super(obj)          ((obj)->super)          /**< be_instance_super */
#define be_instance_sub(obj)            ((obj)->sub)            /**< be_instance_sub */

/**
 * @struct bclass
 * @brief bclass
 *
 */
struct bclass {
    bcommon_header;                                   /**< bcommon_header */
    uint16_t nvar;                                    /**< members variable data field count */
    struct bclass *super;                             /**< super */
    bmap *members;                                    /**< members */
    bstring *name;                                    /**< name */
    bgcobject *gray;                                  /**< for gc gray list */
#ifdef __cplusplus
    BE_CONSTEXPR bclass(uint16_t nv, bclass *sup, bmap *mem, bstring *s) :
        next(0), type(BE_CLASS), marked(GC_CONST), nvar(nv),
        super(sup), members(mem), name(s), gray(0) {} /**< bclass */
#endif
};

/**
 * @struct binstance
 * @brief binstance
 *
 */
struct binstance {
    bcommon_header;          /**< bcommon_header */
    struct binstance *super; /**< super */
    struct binstance *sub;   /**< sub */
    bclass *_class;          /**< _class */
    bgcobject *gray;         /**< for gc gray list */
    bvalue members[1];       /**< members variable data field */
};

/**
 * @fn bclass be_newclass*(bvm*, bstring*, bclass*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @param super (???)
 * @return (???)
 */
bclass* be_newclass(bvm *vm, bstring *name, bclass *super);

/**
 * @fn void be_class_compress(bvm*, bclass*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 */
void be_class_compress(bvm *vm, bclass *c);

/**
 * @fn int be_class_attribute(bvm*, bclass*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param attr (???)
 * @return (???)
 */
int be_class_attribute(bvm *vm, bclass *c, bstring *attr);

/**
 * @fn void be_class_member_bind(bvm*, bclass*, bstring*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param var (???)
 */
void be_class_member_bind(bvm *vm, bclass *c, bstring *name, bbool var);

/**
 * @fn void be_class_method_bind(bvm*, bclass*, bstring*, bproto*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param p (???)
 * @param is_static (???)
 */
void be_class_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p, bbool is_static);

/**
 * @fn void be_class_native_method_bind(bvm*, bclass*, bstring*, bntvfunc)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param f (???)
 */
void be_class_native_method_bind(bvm *vm, bclass *c, bstring *name, bntvfunc f);

/**
 * @fn void be_class_closure_method_bind(bvm*, bclass*, bstring*, bclosure*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param cl (???)
 */
void be_class_closure_method_bind(bvm *vm, bclass *c, bstring *name, bclosure *cl);

/**
 * @fn int be_class_member(bvm*, bclass*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param dst (???)
 * @return (???)
 */
int be_class_member(bvm *vm, bclass *obj, bstring *name, bvalue *dst);

/**
 * @fn bool be_class_setmember(bvm*, bclass*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param src (???)
 * @return (???)
 */
bbool be_class_setmember(bvm *vm, bclass *obj, bstring *name, bvalue *src);

/**
 * @fn int be_class_closure_count(bclass*)
 * @brief (???)
 *
 * @param c (???)
 * @return (???)
 */
int be_class_closure_count(bclass *c);

/**
 * @fn void be_class_upvalue_init(bvm*, bclass*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 */
void be_class_upvalue_init(bvm *vm, bclass *c);

/**
 * @fn bool be_class_newobj(bvm*, bclass*, int, int, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param pos (???)
 * @param argc (???)
 * @param mode (???)
 * @return (???)
 */
bbool be_class_newobj(bvm *vm, bclass *c, int pos, int argc, int mode);

/**
 * @fn int be_instance_member_simple(bvm*, binstance*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param dst (???)
 * @return (???)
 */
int be_instance_member_simple(bvm *vm, binstance *obj, bstring *name, bvalue *dst);

/**
 * @fn int be_instance_member(bvm*, binstance*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param dst (???)
 * @return (???)
 */
int be_instance_member(bvm *vm, binstance *obj, bstring *name, bvalue *dst);

/**
 * @fn bool be_instance_setmember(bvm*, binstance*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param src (???)
 * @return (???)
 */
bbool be_instance_setmember(bvm *vm, binstance *obj, bstring *name, bvalue *src);

#endif
