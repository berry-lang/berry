/**
 * @file       be_module.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_MODULE_H
#define BE_MODULE_H

#include "be_object.h"

#define BE_MODULE_NAME    1

/**
 * @struct bmodule
 * @brief bmodule
 *
 */
typedef struct bmodule {
    bcommon_header;                                             /**< bcommon_header */
    bmap_t *table;                                              /**< table */
    union infodata {                                            /**< infodata */
        const bntvmodule_t *native;                             /**< native */
        const char *name;                                       /**< name */
        const bstring_t *sname;                                 /**< sname */
#ifdef __cplusplus
        BE_CONSTEXPR infodata(const char *name) : name(name) {} /**< infodata */
#endif
    } info;                                                     /**< info */
    bgcobject_t *gray;                                          /**< for gc gray list */
#ifdef __cplusplus
    BE_CONSTEXPR bmodule_t(bmap_t *tab, const char *name) :
        next(0), type(BE_MODULE), marked(GC_CONST),
        table(tab), info(infodata(name)), gray(0) {}            /**< bmodule_t */
#endif
} bmodule_t;                                                    /**< bmodule_t */

/**
 * @fn bmodule_t be_module_new*(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
bmodule_t* be_module_new(bvm_t *vm);

/**
 * @fn void be_module_delete(bvm_t*, bmodule_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param module (???)
 */
void be_module_delete(bvm_t *vm, bmodule_t *module);

/**
 * @fn int be_module_load(bvm_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param path (???)
 * @return (???)
 */
int be_module_load(bvm_t *vm, bstring_t *path);

/**
 * @fn void be_cache_module(bvm_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 */
void be_cache_module(bvm_t *vm, bstring_t *name);

/**
 * @fn int be_module_attr(bvm_t*, bmodule_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param module (???)
 * @param attr (???)
 * @param dst (???)
 * @return (???)
 */
int be_module_attr(bvm_t *vm, bmodule_t *module, bstring_t *attr, bvalue_t *dst);

/**
 * @fn bool be_module_setmember(bvm_t*, bmodule_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param module (???)
 * @param attr (???)
 * @param src (???)
 * @return (???)
 */
bbool be_module_setmember(bvm_t *vm, bmodule_t *module, bstring_t *attr, bvalue_t *src);

/**
 * @fn const char be_module_name*(bmodule_t*)
 * @brief (???)
 *
 * @param module (???)
 * @return (???)
 */
const char* be_module_name(bmodule_t *module);

/**
 * @fn bool be_module_setname(bmodule_t*, bstring_t*)
 * @brief (???)
 *
 * @param module (???)
 * @param name (???)
 * @return (???)
 */
bbool be_module_setname(bmodule_t *module, bstring_t *name);

#endif
