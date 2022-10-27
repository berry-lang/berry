/**
 * @file       be_bytecode.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef __BE_BYTECODE_H
#define __BE_BYTECODE_H

#include "be_object.h"

/**
 * @fn void be_bytecode_save(bvm_t*, const char*, bproto_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param filename (???)
 * @param proto (???)
 */
void be_bytecode_save(bvm_t *vm, const char *filename, bproto_t *proto);

/**
 * @fn bclosure_t be_bytecode_load*(bvm_t*, const char*)
 * @brief (???)
 *
 * @param vm (???)
 * @param filename (???)
 * @return (???)
 */
bclosure_t* be_bytecode_load(bvm_t *vm, const char *filename);

/**
 * @fn bbool be_bytecode_check(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
bbool be_bytecode_check(const char *path);

#endif
