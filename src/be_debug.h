/**
 * @file       be_debug.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_DEBUG_H
#define BE_DEBUG_H

#include "be_object.h"

/**
 * @struct bhookblock
 * @brief bhookblock
 *
 */
struct bhookblock {
    void *data;    /**< data */
    bntvhook hook; /**< hook */
};

/**
 * @fn void be_dumpclosure(bclosure*)
 * @brief
 *
 * @param cl
 */
void be_dumpclosure(bclosure *cl);

/**
 * @fn void be_tracestack(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_tracestack(bvm *vm);

/**
 * @fn void be_callhook(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param mask (???)
 */
void be_callhook(bvm *vm, int mask);

/**
 * @fn bool be_debug_varname(bvm*, int, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param level (???)
 * @param index (???)
 * @return (???)
 */
bbool be_debug_varname(bvm *vm, int level, int index);

/**
 * @fn bool be_debug_upvname(bvm*, int, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param level (???)
 * @param index (???)
 * @return (???)
 */
bbool be_debug_upvname(bvm *vm, int level, int index);

#if BE_USE_DEBUG_MODULE

/**
 * @fn void be_print_inst(binstruction, int, void*)
 * @brief (???)
 *
 * @param ins (???)
 * @param pc (???)
 * @param fout (???)
 */
void be_print_inst(binstruction ins, int pc, void* fout);
#endif

#endif
