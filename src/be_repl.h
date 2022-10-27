/**
 * @file       be_repl.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_REPL_H
#define BE_REPL_H

#include "berry.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn char* (*breadline)(const char *prompt)
 * @brief (???)
 *
 * @param prompt (???)
 * @return (???)
 */
typedef char* (*breadline)(const char *prompt);

/**
 * @fn void (*bfreeline)(char *ptr)
 * @brief (???)
 *
 * @param ptr (???)
 */
typedef void (*bfreeline)(char *ptr);

/**
 * @fn int be_repl(bvm_t*, breadline, bfreeline)
 * @brief (???)
 *
 * @param vm (???)
 * @param getline (???)
 * @param freeline (???)
 * @return (???)
 */
BERRY_API int be_repl(bvm_t *vm, breadline getline, bfreeline freeline);

#ifdef __cplusplus
}
#endif

#endif
