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
 * @typedef bexpdesc_t
 * @brief bexpdesc_t
 *
 */
typedef struct {
    union {
        struct {                /**< for suffix */
            unsigned int idx:9; /**< suffix RK index */
            unsigned int obj:9; /**< object RK index */
            unsigned int tt:5;  /**< object type */
        } ss;                   /**< ss */
        breal_t r;              /**< for ETREAL */
        bint_t i;               /**< for ETINT */
        bstring_t *s;           /**< for ETSTRING */
        bproto_t *p;            /**< for ETPROTO */
        int idx;                /**< variable index */
    } v;                        /**< v */
    int t;                      /**< patch list of 'exit when true' */
    int f;                      /**< patch list of 'exit when false' */
    bbyte_t not;                /**< not mark */
    bbyte_t type;               /**< type */
} bexpdesc_t;

/**
 * @struct bblockinfo
 * @brief
 *
 */
typedef struct bblockinfo {
    struct bblockinfo *prev;
    bbyte_t nactlocals; /**< number of active local variables */
    bbyte_t type;       /**< block type mask */
    bbyte_t hasupval;   /**< has upvalue mark */
    int breaklist;      /**< break list */
    int beginpc;        /**< begin pc */
    int continuelist;   /**< continue list */
} bblockinfo_t;         /**< bblockinfo_t */

/**
 * @struct bfuncinfo
 * @brief bfuncinfo
 *
 */
typedef struct bfuncinfo {
    struct bfuncinfo *prev; /**< outer function */
    bproto_t *proto;        /**< the function prototype */
    bblockinfo_t *binfo;    /**< block information */
    struct blexer *lexer;   /**< the lexer pointer */
    blist_t *local;         /**< local variable */
    bmap_t *upval;          /**< upvalue variable */
    bvector_t code;         /**< code vector */
    bvector_t kvec;         /**< constants table */
    bvector_t pvec;         /**< proto table */
#if BE_DEBUG_RUNTIME_INFO /* debug information */
    bvector_t linevec;      /**< linevec */
#endif
#if BE_DEBUG_VAR_INFO
    bvector_t varvec;       /**< varvec */
#endif
    int pc;                 /**< program count */
    bbyte_t freereg;        /**< first free register */
    bbyte_t flags;          /**< some flages */
} bfuncinfo_t;              /**< bfuncinfo_t */

#define BLOCK_LOOP      1   /**< code block type definition: BLOCK_LOOP */
#define BLOCK_EXCEPT    2   /**< code block type definition: BLOCK_EXCEPT */

/**
 * @fn bclosure_t be_parser_source*(bvm_t*, const char*, breader, void*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param fname (???)
 * @param reader (???)
 * @param data (???)
 * @param islocal (???)
 * @return (???)
 */
bclosure_t *be_parser_source(bvm_t *vm, const char *fname, breader reader, void *data, bbool islocal);

#endif
