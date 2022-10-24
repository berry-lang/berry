/**
 * @file       be_string.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_STRING_H
#define BE_STRING_H

#include "be_object.h"

#define SHORT_STR_MAX_LEN   64 /**< SHORT_STR_MAX_LEN */

/**
 * @typedef bsstring
 * @brief bsstring
 *
 */
typedef struct {
    bstring_header; /**< bstring_header */
#if BE_USE_STR_HASH_CACHE
    uint32_t hash;  /**< hash */
#endif
    /* char s[]; */
} bsstring;

/**
 * @typedef blstring
 * @brief blstring
 *
 */
typedef struct {
    bstring str; /**<  str*/
    int llen;    /**< llen */
    /* char s[]; */
} blstring;

typedef struct {
    bstring_header; /**< bstring_header */
    uint32_t hash;  /**< hash */
    const char *s;  /**< s */
} bcstring;

#define str_len(_s)             ((_s)->slen == 255 ? cast(blstring*, _s)->llen : (_s)->slen)  /**< str_len */

#define str(_s)                 be_str2cstr(_s)                                               /**< str */
#define str_extra(_s)           ((_s)->extra)                                                 /**< str_extra */
#define str_literal(_vm, _s)    be_newstrn((_vm), (_s), sizeof(_s) - 1)                       /**< str_literal */

#if BE_USE_PRECOMPILED_OBJECT
#include "../generate/be_const_strtab.h"
#endif

/**
 * @fn void be_string_init(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_string_init(bvm *vm);

/**
 * @fn void be_string_deleteall(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_string_deleteall(bvm *vm);

/**
 * @fn int be_eqstr(bstring*, bstring*)
 * @brief (???)
 *
 * @param s1 (???)
 * @param s2 (???)
 * @return (???)
 */
int be_eqstr(bstring *s1, bstring *s2);

/**
 * @fn bstring be_newstr*(bvm*, const char*)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @return (???)
 */
bstring* be_newstr(bvm *vm, const char *str);

/**
 * @fn bstring be_newstrn*(bvm*, const char*, size_t)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @param len (???)
 * @return (???)
 */
bstring* be_newstrn(bvm *vm, const char *str, size_t len);

/**
 * @fn bstring be_newlongstr*(bvm*, const char*, size_t)
 * @brief (???)
 *
 * @param vm (???)
 * @param str (???)
 * @param len (???)
 * @return (???)
 */
bstring* be_newlongstr(bvm *vm, const char *str, size_t len);

/**
 * @fn void be_gcstrtab(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_gcstrtab(bvm *vm);

/**
 * @fn uint32_t be_strhash(const bstring*)
 * @brief (???)
 *
 * @param s (???)
 * @return (???)
 */
uint32_t be_strhash(const bstring *s);

/**
 * @fn const char be_str2cstr*(const bstring*)
 * @brief (???)
 *
 * @param s (???)
 * @return (???)
 */
const char* be_str2cstr(const bstring *s);

/**
 * @fn void be_str_setextra(bstring*, int)
 * @brief (???)
 *
 * @param s (???)
 * @param extra (???)
 */
void be_str_setextra(bstring *s, int extra);

#endif
