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
    bmap *table;                                                /**< table */
    union infodata {                                            /**< infodata */
        const bntvmodule_t *native;                             /**< native */
        const char *name;                                       /**< name */
        const bstring *sname;                                   /**< sname */
#ifdef __cplusplus
        BE_CONSTEXPR infodata(const char *name) : name(name) {} /**< infodata */
#endif
    } info;                                                     /**< info */
    bgcobject *gray;                                            /**< for gc gray list */
#ifdef __cplusplus
    BE_CONSTEXPR bmodule(bmap *tab, const char *name) :
        next(0), type(BE_MODULE), marked(GC_CONST),
        table(tab), info(infodata(name)), gray(0) {}            /**< bmodule */
#endif
} bmodule;                                                      /**< bmodule */

/**
 * @fn bmodule be_module_new*(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
bmodule* be_module_new(bvm *vm);

/**
 * @fn void be_module_delete(bvm*, bmodule*)
 * @brief (???)
 *
 * @param vm (???)
 * @param module (???)
 */
void be_module_delete(bvm *vm, bmodule *module);

/**
 * @fn int be_module_load(bvm*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param path (???)
 * @return (???)
 */
int be_module_load(bvm *vm, bstring *path);

/**
 * @fn void be_cache_module(bvm*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 */
void be_cache_module(bvm *vm, bstring *name);

/**
 * @fn int be_module_attr(bvm*, bmodule*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param module (???)
 * @param attr (???)
 * @param dst (???)
 * @return (???)
 */
int be_module_attr(bvm *vm, bmodule *module, bstring *attr, bvalue *dst);

/**
 * @fn bool be_module_setmember(bvm*, bmodule*, bstring*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param module (???)
 * @param attr (???)
 * @param src (???)
 * @return (???)
 */
bbool be_module_setmember(bvm *vm, bmodule *module, bstring *attr, bvalue *src);

/**
 * @fn const char be_module_name*(bmodule*)
 * @brief (???)
 *
 * @param module (???)
 * @return (???)
 */
const char* be_module_name(bmodule *module);

/**
 * @fn bool be_module_setname(bmodule*, bstring*)
 * @brief (???)
 *
 * @param module (???)
 * @param name (???)
 * @return (???)
 */
bbool be_module_setname(bmodule *module, bstring *name);

#endif
