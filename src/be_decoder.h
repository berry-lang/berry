/**
 * @file       be_decoder.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_OPCODE_H
#define BE_OPCODE_H

#define NO_JUMP                 -1                                                /**< NO_JUMP */

/* define field's bit-width  */
#define IOP_BITS                6u                                                /**< IOP_BITS */
#define IRA_BITS                8u                                                /**< IRA_BITS */
#define IRKB_BITS               9u                                                /**< IRKB_BITS */
#define IRKC_BITS               9u                                                /**< IRKC_BITS */

/* define field's bit-width and positions */
#define IRKC_POS                0u                                                /**< IRKC_POS */
#define IRKB_POS                (IRKC_POS + IRKC_BITS)                            /**< IRKB_POS */
#define IRA_POS                 (IRKB_POS + IRKB_BITS)                            /**< IRA_POS */
#define IOP_POS                 (IRA_POS + IRA_BITS)                              /**< IOP_POS */
#define IAx_BITS                (IRA_BITS + IRKB_BITS + IRKC_BITS)                /**< IAx_BITS */
#define IBx_BITS                (IRKC_BITS + IRKB_BITS)                           /**< IBx_BITS */

/* basic field operation */
#define INS_MASK(pos, bits)     ((binstruction_t)((1 << (bits)) - 1) << (pos))    /**< INS_MASK */
#define INS_GETx(i, mask, pos)  cast_int(((binstruction_t)(i) & (mask)) >> (pos)) /**< INS_GETx */
#define INS_SETx(v, mask, pos)  (((binstruction_t)(v) << (pos)) & (mask))         /**< INS_SETx */

/* instruction operation */
#define isK(v)                  (((v) & (1 << (IRKB_BITS - 1))) != 0)             /**< isK */
#define setK(v)                 ((v) | (1 << (IRKB_BITS - 1)))                    /**< setK */
#define KR2idx(v)               ((v) & 0xFF)                                      /**< KR2idx */
#define isKB(v)                 (((v) & (1 << (IRA_POS - 1))) != 0)               /**< isKB */
#define isKC(v)                 (((v) & (1 << (IRKB_POS - 1))) != 0)              /**< isKC */

/* define masks and limit values */
#define IOP_MASK                INS_MASK(IOP_POS, IOP_BITS)                       /**< IOP_MASK */
#define IRA_MASK                INS_MASK(IRA_POS, IRA_BITS)                       /**< IRA_MASK */
#define IRKB_MASK               INS_MASK(IRKB_POS, IRKB_BITS)                     /**< IRKB_MASK */
#define IRKC_MASK               INS_MASK(IRKC_POS, IRKC_BITS)                     /**< IRKC_MASK */
#define IAx_MASK                INS_MASK(0, IAx_BITS)                             /**< IAx_MASK */
#define IBx_MASK                INS_MASK(0, IBx_BITS)                             /**< IBx_MASK */
#define IsBx_MAX                cast_int(IBx_MASK >> 1)                           /**< IsBx_MAX */
#define IsBx_MIN                cast_int(-IsBx_MAX - 1)                           /**< IsBx_MIN */

/* mask for K/R values */
#define KR_MASK                 ((1 << (IRKB_BITS-1)) - 1)                        /**< KR_MASK */

/* get field */
#define IGET_OP(i)              cast(bopcode_t, INS_GETx(i, IOP_MASK, IOP_POS))   /**< IGET_OP */
#define IGET_RA(i)              INS_GETx(i, IRA_MASK, IRA_POS)                    /**< IGET_RA */
#define IGET_RKB(i)             INS_GETx(i, IRKB_MASK, IRKB_POS)                  /**< IGET_RKB */
#define IGET_RKC(i)             INS_GETx(i, IRKC_MASK, IRKC_POS)                  /**< IGET_RKC */
#define IGET_Bx(i)              INS_GETx(i, IBx_MASK, 0)                          /**< IGET_Bx */
#define IGET_sBx(i)             (IGET_Bx(i) - IsBx_MAX)                           /**< IGET_sBx */

/* set field */
#define ISET_OP(i)              INS_SETx(i, IOP_MASK, IOP_POS)                    /**< ISET_OP */
#define ISET_RA(i)              INS_SETx(i, IRA_MASK, IRA_POS)                    /**< ISET_RA */
#define ISET_RKB(i)             INS_SETx(i, IRKB_MASK, IRKB_POS)                  /**< ISET_RKB */
#define ISET_RKC(i)             INS_SETx(i, IRKC_MASK, IRKC_POS)                  /**< ISET_RKC */
#define ISET_Bx(i)              INS_SETx(i, IBx_MASK, 0)                          /**< ISET_Bx */
#define ISET_sBx(i)             (ISET_Bx(cast_int(i) + IsBx_MAX))                 /**< ISET_sBx */

/**
 * @typedef bopcode_t
 * @brief bopcode_t
 *
 */
typedef enum {
    #define OPCODE(opc) OP_##opc  /**<  */
    #include "be_opcodes.h"
    #undef OPCODE
} bopcode_t;

#endif
