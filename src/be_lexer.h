/**
 * @file       be_lexer.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_LEXER_H
#define BE_LEXER_H

#include "be_object.h"

/**
 * @typedef btokentype
 * @brief btokentype
 *
 */
typedef enum {
    TokenNone = 0,
    TokenEOS,       /**< end of source */
    TokenId,        /**< identifier */
    TokenInteger,   /**< TokenInteger */
    TokenReal,      /**< TokenReal */
    TokenString,    /**< TokenString */
    /* operator, don't change order */
    /* assign operator */
    OptAssign,      /**< operator, = */
    OptAddAssign,   /**< operator, += */
    OptSubAssign,   /**< operator, -= */
    OptMulAssign,   /**< operator, *= */
    OptDivAssign,   /**< operator, /= */
    OptModAssign,   /**< operator, %= */
    OptAndAssign,   /**< operator, &= */
    OptOrAssign,    /**< operator, |= */
    OptXorAssign,   /**< operator, ^= */
    OptLsfAssign,   /**< operator, <<= */
    OptRsfAssign,   /**< operator, >>= */
    /* binary operator */
    OptAdd,         /**< operator, + */
    OptSub,         /**< operator, - */
    OptMul,         /**< operator, * */
    OptDiv,         /**< operator, / */
    OptMod,         /**< operator, % */
    OptLT,          /**< operator, < */
    OptLE,          /**< operator, <= */
    OptEQ,          /**< operator, == */
    OptNE,          /**< operator, != */
    OptGT,          /**< operator, > */
    OptGE,          /**< operator, >= */
    OptBitAnd,      /**< operatoe, &  */
    OptBitOr,       /**< operatoe, |  */
    OptBitXor,      /**< operatoe, ^  */
    OptShiftL,      /**< operatoe, <<  */
    OptShiftR,      /**< operatoe, >>  */
    OptConnect,     /**< operator, .. */
    OptAnd,         /**< operator, && */
    OptOr,          /**< operator, || */
    /* unary operator */
    OptNot,         /**< operator, ! */
    OptFlip,        /**< operator, ~ */
    /* postfix operator or bracket */
    OptLBK,         /**< operator, ( bracket */
    OptRBK,         /**< operator, ) bracket */
    OptLSB,         /**< operator, [ square bracket */
    OptRSB,         /**< operator, ] square bracket */
    OptLBR,         /**< operator, { brace */
    OptRBR,         /**< operator, } brace */
    OptDot,         /**< operator, . dot */
    /* other symbol */
    OptComma,       /**< operator, , */
    OptSemic,       /**< operator, ; */
    OptColon,       /**< operator, : */
    OptQuestion,    /**< operator, ? */
    OptArrow,       /**< operator, -> */
    /* keyword */
    KeyIf,          /**< keyword if */
    KeyElif,        /**< keyword elif */
    KeyElse,        /**< keyword else */
    KeyWhile,       /**< keyword while */
    KeyFor,         /**< keyword for */
    KeyDef,         /**< keyword def */
    KeyEnd,         /**< keyword end */
    KeyClass,       /**< keyword class */
    KeyBreak,       /**< keyword break */
    KeyContinue,    /**< keyword continue */
    KeyReturn,      /**< keyword return */
    KeyTrue,        /**< keyword true */
    KeyFalse,       /**< keyword false */
    KeyNil,         /**< keyword nil */
    KeyVar,         /**< keyword var */
    KeyDo,          /**< keyword do */
    KeyImport,      /**< keyword import */
    KeyAs,          /**< keyword as */
    KeyTry,         /**< keyword try */
    KeyExcept,      /**< keyword except */
    KeyRaise,       /**< keyword raise */
    KeyStatic       /**< keyword static */
} btokentype;

/**
 * @struct blexerreader
 * @brief blexerreader
 *
 */
struct blexerreader {
    const char *s; /**< s */
    size_t len;    /**< len */
    void *data;    /**< data */
    breader readf; /**< readf */
};

/**
 * @struct blexerbuf
 * @brief blexerbuf
 *
 */
struct blexerbuf {
    char *s;          /**< s */
    size_t len, size; /**< len, size */
};

/**
 * @struct btoken
 * @brief
 *
 */
typedef struct btoken {
    btokentype type; /**< type */
    union {
        bstring *s;  /**< s */
        bint i;      /**< i */
        breal r;     /**< r */
    } u;             /**< u */
} btoken;

/**
 * @struct blexer
 * @brief
 *
 */
typedef struct blexer {
    const char *fname;          /**< fname */
    btoken token;               /**< token */
    int linenumber;             /**< linenumber */
    int lastline;               /**< lastline */
    btokentype cacheType;       /**< cacheType */
    struct blexerbuf buf;       /**< buf */
    struct blexerreader reader; /**< reader */
    bmap *strtab;               /**< strtab */
    bvm *vm;                    /**< vm */
    int cursor;                 /**< cursor */
} blexer;                       /**< blexer */

/**
 * @fn void be_lexer_init(blexer*, bvm*, const char*, breader, void*)
 * @brief (???)
 *
 * @param lexer (???)
 * @param vm (???)
 * @param fname (???)
 * @param reader (???)
 * @param data (???)
 */
void be_lexer_init(blexer *lexer, bvm *vm, const char *fname, breader reader, void *data);

/**
 * @fn void be_lexer_deinit(blexer*)
 * @brief (???)
 *
 * @param lexer (???)
 */
void be_lexer_deinit(blexer *lexer);

/**
 * @fn void be_lexerror(blexer*, const char*)
 * @brief (???)
 *
 * @param lexer (???)
 * @param msg (???)
 */
void be_lexerror(blexer *lexer, const char *msg);

/**
 * @fn int be_lexer_scan_next(blexer*)
 * @brief (???)
 *
 * @param lexer (???)
 * @return (???)
 */
int be_lexer_scan_next(blexer *lexer);

/**
 * @fn bstring be_lexer_newstr*(blexer*, const char*)
 * @brief (???)
 *
 * @param lexer (???)
 * @param str (???)
 * @return (???)
 */
bstring* be_lexer_newstr(blexer *lexer, const char *str);

/**
 * @fn const char be_token2str*(bvm*, btoken*)
 * @brief (???)
 *
 * @param vm (???)
 * @param token (???)
 * @return (???)
 */
const char* be_token2str(bvm *vm, btoken *token);

/**
 * @fn const char be_tokentype2str*(btokentype)
 * @brief (???)
 *
 * @param type (???)
 * @return (???)
 */
const char* be_tokentype2str(btokentype type);

#endif
