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
 * @fn void be_globalvar_init(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_globalvar_init(bvm_t *vm);

/**
 * @fn void be_globalvar_deinit(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_globalvar_deinit(bvm_t *vm);

/**
 * @fn int be_global_find(bvm_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_global_find(bvm_t *vm, bstring_t *name);

/**
 * @fn int be_global_new(bvm_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_global_new(bvm_t *vm, bstring_t *name);

/**
 * @fn bvalue_t be_global_var*(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param index (???)
 * @return (???)
 */
bvalue_t* be_global_var(bvm_t *vm, int index);

/**
 * @fn void be_global_release_space(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_global_release_space(bvm_t *vm);

/**
 * @fn int be_builtin_find(bvm_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_builtin_find(bvm_t *vm, bstring_t *name);

/**
 * @fn bstring_t be_builtin_name*(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param index (???)
 * @return (???)
 */
bstring_t* be_builtin_name(bvm_t *vm, int index);

/**
 * @fn int be_builtin_new(bvm_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param name (???)
 * @return (???)
 */
int be_builtin_new(bvm_t *vm, bstring_t *name);

/**
 * @fn void be_bulitin_release_space(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_bulitin_release_space(bvm_t *vm);

/**
 * @fn void be_const_builtin_set(bvm_t*, const bmap_t*, const bvector_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param map (???)
 * @param vec (???)
 */
void be_const_builtin_set(bvm_t *vm, const bmap_t *map, const bvector_t *vec);

#endif
