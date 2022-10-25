/**
 * @file       be_parser.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_PARSER_H
#define BE_PARSER_H

#include "be_object.h"
#include "be_string.h"

/**
 * @typedef exptype_t
 * @brief exptype_t
 *
 */
typedef enum {
    ETVOID,    /**< unknown (new variable or error) */
    ETNIL,     /**< ETNIL */
    ETBOOL,    /**< ETBOOL */
    ETREAL,    /**< ETREAL */
    ETINT,     /**< ETINT */
    ETSTRING,  /**< ETSTRING */
    ETPROTO,   /**< ETPROTO */
    ETCONST,   /**< ETCONST */
    ETLOCAL,   /**< local variable, allocated until end of scope */
    ETGLOBAL,  /**< global by index number */
    ETUPVAL,   /**< ETUPVAL */
    ETMEMBER,  /**< member accessor (by name) */
    ETINDEX,   /**< index accessor (ex array index) */
    ETREG,     /**< temporary register, can be freed if top of stack */
    ETNGLOBAL  /**< named global */
} exptype_t;

/**
 * @typedef bexpdesc
 * @brief bexpdesc
 *
 */
typedef struct {
    union {
        struct {                /**< for suffix */
            unsigned int idx:9; /**< suffix RK index */
            unsigned int obj:9; /**< object RK index */
            unsigned int tt:5;  /**< object type */
        } ss;                   /**< ss */
        breal r;                /**< for ETREAL */
        bint i;                 /**< for ETINT */
        bstring *s;             /**< for ETSTRING */
        bproto *p;              /**< for ETPROTO */
        int idx;                /**< variable index */
    } v;                        /**< v */
    int t;                      /**< patch list of 'exit when true' */
    int f;                      /**< patch list of 'exit when false' */
    bbyte not;                  /**< not mark */
    bbyte type;                 /**< type */
} bexpdesc;

/**
 * @struct bblockinfo
 * @brief
 *
 */
typedef struct bblockinfo {
    struct bblockinfo *prev;
    bbyte nactlocals; /**< number of active local variables */
    bbyte type;       /**< block type mask */
    bbyte hasupval;   /**< has upvalue mark */
    int breaklist;    /**< break list */
    int beginpc;      /**< begin pc */
    int continuelist; /**< continue list */
} bblockinfo;         /**< bblockinfo */

/**
 * @struct bfuncinfo
 * @brief bfuncinfo
 *
 */
typedef struct bfuncinfo {
    struct bfuncinfo *prev; /**< outer function */
    bproto *proto;          /**< the function prototype */
    bblockinfo *binfo;      /**< block information */
    struct blexer *lexer;   /**< the lexer pointer */
    blist *local;           /**< local variable */
    bmap *upval;            /**< upvalue variable */
    bvector code;           /**< code vector */
    bvector kvec;           /**< constants table */
    bvector pvec;           /**< proto table */
#if BE_DEBUG_RUNTIME_INFO /* debug information */
    bvector linevec;        /**< linevec */
#endif
#if BE_DEBUG_VAR_INFO
    bvector varvec;         /**< varvec */
#endif
    int pc;                 /**< program count */
    bbyte freereg;          /**< first free register */
    bbyte flags;            /**< some flages */
} bfuncinfo;                /**< bfuncinfo */

#define BLOCK_LOOP      1 /**< code block type definition: BLOCK_LOOP */
#define BLOCK_EXCEPT    2 /**< code block type definition: BLOCK_EXCEPT */

/**
 * @fn bclosure be_parser_source*(bvm*, const char*, breader, void*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param fname (???)
 * @param reader (???)
 * @param data (???)
 * @param islocal (???)
 * @return (???)
 */
bclosure *be_parser_source(bvm *vm, const char *fname, breader reader, void *data, bbool islocal);

#endif
