/**
 * @file       be_debug.h
 * @brief      Berry file
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
 * @fn void be_dumpclosure(bclosure_t*)
 * @brief
 *
 * @param cl
 */
void be_dumpclosure(bclosure_t *cl);

/**
 * @fn void be_tracestack(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_tracestack(bvm_t *vm);

/**
 * @fn void be_callhook(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param mask (???)
 */
void be_callhook(bvm_t *vm, int mask);

/**
 * @fn bbool be_debug_varname(bvm_t*, int, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param level (???)
 * @param index (???)
 * @return (???)
 */
bbool be_debug_varname(bvm_t *vm, int level, int index);

/**
 * @fn bbool be_debug_upvname(bvm_t*, int, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param level (???)
 * @param index (???)
 * @return (???)
 */
bbool be_debug_upvname(bvm_t *vm, int level, int index);

#if BE_USE_DEBUG_MODULE

/**
 * @fn void be_print_inst(binstruction_t, int, void*)
 * @brief (???)
 *
 * @param ins (???)
 * @param pc (???)
 * @param fout (???)
 */
void be_print_inst(binstruction_t ins, int pc, void* fout);
#endif

#endif
