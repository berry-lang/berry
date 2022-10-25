/**
 * @file       be_var.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_VAR_H
#define BE_VAR_H

#include "be_object.h"

#define be_global_count(vm)   be_vector_count(&(vm)->gbldesc.global.vlist)  /**< be_global_count */
#define be_builtin_count(vm)  be_vector_count(&(vm)->gbldesc.builtin.vlist) /**< be_builtin_count */

/**
 * @fn void be_globalvar_init(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_globalvar_init(bvm *vm);

/**
 * @fn void be_globalvar_deinit(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_globalvar_deinit(bvm *vm);

/**
 * @fn int be_global_find(bvm*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_global_find(bvm *vm, bstring *name);

/**
 * @fn int be_global_new(bvm*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_global_new(bvm *vm, bstring *name);

/**
 * @fn bvalue be_global_var*(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param index (???)
 * @return (???)
 */
bvalue* be_global_var(bvm *vm, int index);

/**
 * @fn void be_global_release_space(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_global_release_space(bvm *vm);

/**
 * @fn int be_builtin_find(bvm*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_builtin_find(bvm *vm, bstring *name);

/**
 * @fn bstring be_builtin_name*(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param index (???)
 * @return (???)
 */
bstring* be_builtin_name(bvm *vm, int index);

/**
 * @fn int be_builtin_new(bvm*, bstring*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_builtin_new(bvm *vm, bstring *name);

/**
 * @fn void be_bulitin_release_space(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_bulitin_release_space(bvm *vm);

/**
 * @fn void be_const_builtin_set(bvm*, const bmap*, const bvector*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param vec (???)
 */
void be_const_builtin_set(bvm *vm, const bmap *map, const bvector *vec);

#endif
