/**
 * @file       be_string.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_STRING_H
#define BE_STRING_H

#include "be_object.h"

#define SHORT_STR_MAX_LEN   64 /**< SHORT_STR_MAX_LEN */

/**
 * @typedef bsstring_t
 * @brief bsstring_t
 *
 */
typedef struct {
    bstring_header; /**< bstring_header */
#if BE_USE_STR_HASH_CACHE
    uint32_t hash;  /**< hash */
#endif
    /* char s[]; */
} bsstring_t;

/**
 * @typedef blstring_t
 * @brief blstring_t
 *
 */
typedef struct {
    bstring_t str; /**< str */
    int llen;      /**< llen */
    /* char s[]; */
} blstring_t;

/**
 * @typedef bcstring_t
 * @brief bcstring_t
 *
 */
typedef struct {
    bstring_header; /**< bstring_header */
    uint32_t hash;  /**< hash */
    const char *s;  /**< s */
} bcstring_t;

#define str_len(_s)             ((_s)->slen == 255 ? cast(blstring_t*, _s)->llen : (_s)->slen) /**< str_len */

#define str(_s)                 be_str2cstr(_s)                                                /**< str */
#define str_extra(_s)           ((_s)->extra)                                                  /**< str_extra */
#define str_literal(_vm, _s)    be_newstrn((_vm), (_s), sizeof(_s) - 1)                        /**< str_literal */

#if BE_USE_PRECOMPILED_OBJECT
#include "../generate/be_const_strtab.h"
#endif

/**
 * @fn void be_string_init(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_string_init(bvm_t *vm);

/**
 * @fn void be_string_deleteall(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_string_deleteall(bvm_t *vm);

/**
 * @fn int be_eqstr(bstring_t*, bstring_t*)
 * @brief (???)
 *
 * @param s1 (???)
 * @param s2 (???)
 * @return (???)
 */
int be_eqstr(bstring_t *s1, bstring_t *s2);

/**
 * @fn bstring_t be_newstr*(bvm_t*, const char*)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @return (???)
 */
bstring_t* be_newstr(bvm_t *vm, const char *str);

/**
 * @fn bstring_t be_newstrn*(bvm_t*, const char*, size_t)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @param len (???)
 * @return (???)
 */
bstring_t* be_newstrn(bvm_t *vm, const char *str, size_t len);

/**
 * @fn bstring_t_t be_newlongstr*(bvm_t*, const char*, size_t)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @param len (???)
 * @return (???)
 */
bstring_t* be_newlongstr(bvm_t *vm, const char *str, size_t len);

/**
 * @fn void be_gcstrtab(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_gcstrtab(bvm_t *vm);

/**
 * @fn uint32_t be_strhash(const bstring_t*)
 * @brief (???)
 *
 * @param s (???)
 * @return (???)
 */
uint32_t be_strhash(const bstring_t *s);

/**
 * @fn const char be_str2cstr*(const bstring_t*)
 * @brief (???)
 *
 * @param s (???)
 * @return (???)
 */
const char* be_str2cstr(const bstring_t *s);

/**
 * @fn void be_str_setextra(bstring_t*, int)
 * @brief (???)
 *
 * @param s (???)
 * @param extra (???)
 */
void be_str_setextra(bstring_t *s, int extra);

#endif
