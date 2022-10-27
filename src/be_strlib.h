/**
 * @file       be_strlib.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_STRLIB_H
#define BE_STRLIB_H

#include "be_object.h"
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn bstring_t be_strcat*(bvm_t*, bstring_t*, bstring_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param s1 (???)
 * @param s2 (???)
 * @return (???)
 */
bstring_t* be_strcat(bvm_t *vm, bstring_t *s1, bstring_t *s2);

/**
 * @fn int be_strcmp(bstring_t*, bstring_t*)
 * @brief (???)
 *
 * @param s1 (???)
 * @param s2 (???)
 * @return (???)
 */
int be_strcmp(bstring_t *s1, bstring_t *s2);

/**
 * @fn bstring_t be_num2str*(bvm_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param v (???)
 * @return (???)
 */
bstring_t* be_num2str(bvm_t *vm, bvalue_t *v);

/**
 * @fn void be_val2str(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param index (???)
 */
void be_val2str(bvm_t *vm, int index);

/**
 * @fn int be_char2hex(int)
 * @brief (???)
 *
 * @param c (???)
 * @return (???)
 */
int be_char2hex(int c);

/**
 * @fn size_t be_strlcpy(char*, const char*, size_t)
 * @brief (???)
 *
 * @param dst (???)
 * @param src (???)
 * @param size (???)
 * @return (???)
 */
size_t be_strlcpy(char *dst, const char *src, size_t size);

/**
 * @fn const char be_splitpath*(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
const char* be_splitpath(const char *path);

/**
 * @fn const char be_splitname*(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
const char* be_splitname(const char *path);

/**
 * @fn const char be_pushvfstr*(bvm_t*, const char*, va_list)
 * @brief (???)
 *
 * @param vm (???)
 * @param format (???)
 * @param arg (???)
 * @return (???)
 */
const char* be_pushvfstr(bvm_t *vm, const char *format, va_list arg);

/**
 * @fn bstring be_strindex*(bvm_t*, bstring_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @param idx (???)
 * @return (???)
 */
bstring_t* be_strindex(bvm_t *vm, bstring_t *str, bvalue_t *idx);

#ifdef __cplusplus
}
#endif

#endif
