/**
 * @file       be_func.h
 * @brief      Main berry entry API
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
    (((bupval**)((size_t)cc + sizeof(bntvclos)))[n])

/**
 * @fn void be_initupvals(bvm*, bclosure*)
 * @brief (???)
 *
 * @param vm (???)
 * @param cl (???)
 */
void be_initupvals(bvm *vm, bclosure *cl);

/**
 * @fn void be_upvals_close(bvm*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param level (???)
 */
void be_upvals_close(bvm *vm, bvalue *level);

/**
 * @fn void be_release_upvalues(bvm*, bclosure*)
 * @brief (???)
 *
 * @param vm (???)
 * @param cl (???)
 */
void be_release_upvalues(bvm *vm, bclosure *cl);

/**
 * @fn bproto be_newproto*(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
bproto* be_newproto(bvm *vm);

/**
 * @fn bclosure be_newclosure*(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param nupval (???)
 * @return (???)
 */
bclosure* be_newclosure(bvm *vm, int nupval);

/**
 * @fn bntvclos be_newntvclosure*(bvm*, bntvfunc, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param cf (???)
 * @param nupvals (???)
 * @return (???)
 */
bntvclos* be_newntvclosure(bvm *vm, bntvfunc cf, int nupvals);

/**
 * @fn bstring be_func_varname*(bproto*, int, int)
 * @brief (???)
 *
 * @param proto (???)
 * @param index (???)
 * @param pc (???)
 * @return (???)
 */
bstring* be_func_varname(bproto *proto, int index, int pc);

#endif
