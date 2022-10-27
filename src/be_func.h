/**
 * @file       be_func.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_FUNC_H
#define BE_FUNC_H

#include "be_object.h"

/**
 * @def be_newntvclos
 * @brief be_newntvclos
 *
 */
#define be_newntvclos(vm, cf) \
    be_newntvclosure(vm, cf, 0)

/**
 * @def be_ntvclos_upval
 * @brief be_ntvclos_upval
 *
 */
#define be_ntvclos_upval(cc, n) \
    (((bupval_t**)((size_t)cc + sizeof(bntvclos_t)))[n])

/**
 * @fn void be_initupvals(bvm_t*, bclosure_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param cl (???)
 */
void be_initupvals(bvm_t *vm, bclosure_t *cl);

/**
 * @fn void be_upvals_close(bvm_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param level (???)
 */
void be_upvals_close(bvm_t *vm, bvalue_t *level);

/**
 * @fn void be_release_upvalues(bvm_t*, bclosure_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param cl (???)
 */
void be_release_upvalues(bvm_t *vm, bclosure_t *cl);

/**
 * @fn bproto_t be_newproto*(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
bproto_t* be_newproto(bvm_t *vm);

/**
 * @fn bclosure_t be_newclosure*(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param nupval (???)
 * @return (???)
 */
bclosure_t* be_newclosure(bvm_t *vm, int nupval);

/**
 * @fn bntvclos_t be_newntvclosure*(bvm_t*, bntvfunc, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param cf (???)
 * @param nupvals (???)
 * @return (???)
 */
bntvclos_t* be_newntvclosure(bvm_t *vm, bntvfunc cf, int nupvals);

/**
 * @fn bstring_t be_func_varname*(bproto_t*, int, int)
 * @brief (???)
 *
 * @param proto (???)
 * @param index (???)
 * @param pc (???)
 * @return (???)
 */
bstring_t* be_func_varname(bproto_t *proto, int index, int pc);

#endif
