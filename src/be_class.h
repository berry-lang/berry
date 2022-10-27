/**
 * @file       be_class.h
 * @brief      Berry file
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
    bmap_t *members;                                  /**< members */
    bstring_t *name;                                  /**< name */
    bgcobject_t *gray;                                /**< for gc gray list */
#ifdef __cplusplus
    BE_CONSTEXPR bclass_t(uint16_t nv, bclass_t *sup, bmap_t *mem, bstring_t *s) :
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
    bclass_t *_class;        /**< _class */
    bgcobject_t *gray;       /**< for gc gray list */
    bvalue_t members[1];     /**< members variable data field */
};

/**
 * @fn bclass_t be_newclass*(bvm_t*, bstring_t*, bclass_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @param super (???)
 * @return (???)
 */
bclass_t* be_newclass(bvm_t *vm, bstring_t *name, bclass_t *super);

/**
 * @fn void be_class_compress(bvm_t*, bclass_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 */
void be_class_compress(bvm_t *vm, bclass_t *c);

/**
 * @fn int be_class_attribute(bvm_t*, bclass_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param attr (???)
 * @return (???)
 */
int be_class_attribute(bvm_t *vm, bclass_t *c, bstring_t *attr);

/**
 * @fn void be_class_member_bind(bvm_t*, bclass_t*, bstring_t*, bbool)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param var (???)
 */
void be_class_member_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bbool var);

/**
 * @fn void be_class_method_bind(bvm_t*, bclass_t*, bstring_t*, bproto_t*, bbool)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param p (???)
 * @param is_static (???)
 */
void be_class_method_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bproto_t *p, bbool is_static);

/**
 * @fn void be_class_native_method_bind(bvm_t*, bclass_t*, bstring_t*, bntvfunc)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param f (???)
 */
void be_class_native_method_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bntvfunc f);

/**
 * @fn void be_class_closure_method_bind(bvm_t*, bclass_t*, bstring_t*, bclosure_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param name (???)
 * @param cl (???)
 */
void be_class_closure_method_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bclosure_t *cl);

/**
 * @fn int be_class_member(bvm_t*, bclass_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param dst (???)
 * @return (???)
 */
int be_class_member(bvm_t *vm, bclass_t *obj, bstring_t *name, bvalue_t *dst);

/**
 * @fn bbool be_class_setmember(bvm_t*, bclass_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param src (???)
 * @return (???)
 */
bbool be_class_setmember(bvm_t *vm, bclass_t *obj, bstring_t *name, bvalue_t *src);

/**
 * @fn int be_class_closure_count(bclass_t*)
 * @brief (???)
 *
 * @param c (???)
 * @return (???)
 */
int be_class_closure_count(bclass_t *c);

/**
 * @fn void be_class_upvalue_init(bvm_t*, bclass_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 */
void be_class_upvalue_init(bvm_t *vm, bclass_t *c);

/**
 * @fn bbool be_class_newobj(bvm_t*, bclass_t*, int, int, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param c (???)
 * @param pos (???)
 * @param argc (???)
 * @param mode (???)
 * @return (???)
 */
bbool be_class_newobj(bvm_t *vm, bclass_t *c, int pos, int argc, int mode);

/**
 * @fn int be_instance_member_simple(bvm_t*, binstance_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param dst (???)
 * @return (???)
 */
int be_instance_member_simple(bvm_t *vm, binstance_t *obj, bstring_t *name, bvalue_t *dst);

/**
 * @fn int be_instance_member(bvm_t*, binstance_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param dst (???)
 * @return (???)
 */
int be_instance_member(bvm_t *vm, binstance_t *obj, bstring_t *name, bvalue_t *dst);

/**
 * @fn bbool be_instance_setmember(bvm_t*, binstance_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param obj (???)
 * @param name (???)
 * @param src (???)
 * @return (???)
 */
bbool be_instance_setmember(bvm_t *vm, binstance_t *obj, bstring_t *name, bvalue_t *src);

#endif
