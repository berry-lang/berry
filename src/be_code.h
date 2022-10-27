/**
 * @file       be_code.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_CODE_H
#define BE_CODE_H

#include "be_parser.h"

#define be_code_freeregs(f, n)      ((f)->freereg -= (bbyte_t)(n)) /**< be_code_freeregs */

/**
 * @fn int be_code_allocregs(bfuncinfo_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param count (???)
 * @return (???)
 */
int be_code_allocregs(bfuncinfo_t *finfo, int count);

/**
 * @fn void be_code_prebinop(bfuncinfo_t*, int, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param op (???)
 * @param e (???)
 */
void be_code_prebinop(bfuncinfo_t *finfo, int op, bexpdesc_t *e);

/**
 * @fn void be_code_binop(bfuncinfo_t*, int, bexpdesc_t*, bexpdesc_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param op (???)
 * @param e1 (???)
 * @param e2 (???)
 * @param dst (???)
 */
void be_code_binop(bfuncinfo_t *finfo, int op, bexpdesc_t *e1, bexpdesc_t *e2, int dst);

/**
 * @fn int be_code_unop(bfuncinfo_t*, int, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param op (???)
 * @param e (???)
 * @return (???)
 */
int be_code_unop(bfuncinfo_t *finfo, int op, bexpdesc_t *e);

/**
 * @fn int be_code_setvar(bfuncinfo_t*, bexpdesc_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e1 (???)
 * @param e2 (???)
 * @return (???)
 */
int be_code_setvar(bfuncinfo_t *finfo, bexpdesc_t *e1, bexpdesc_t *e2);

/**
 * @fn int be_code_nextreg(bfuncinfo_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @return (???)
 */
int be_code_nextreg(bfuncinfo_t *finfo, bexpdesc_t *e);

/**
 * @fn int be_code_jump(bfuncinfo_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @return (???)
 */
int be_code_jump(bfuncinfo_t *finfo);

/**
 * @fn void be_code_jumpto(bfuncinfo_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param dst (???)
 */
void be_code_jumpto(bfuncinfo_t *finfo, int dst);

/**
 * @fn void be_code_jumpbool(bfuncinfo_t*, bexpdesc_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @param jumptrue (???)
 */
void be_code_jumpbool(bfuncinfo_t *finfo, bexpdesc_t *e, int jumptrue);

/**
 * @fn void be_code_conjump(bfuncinfo_t*, int*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param list (???)
 * @param jmp (???)
 */
void be_code_conjump(bfuncinfo_t *finfo, int *list, int jmp);

/**
 * @fn void be_code_patchlist(bfuncinfo_t*, int, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param list (???)
 * @param dst (???)
 */
void be_code_patchlist(bfuncinfo_t *finfo, int list, int dst);

/**
 * @fn void be_code_patchjump(bfuncinfo_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param jmp (???)
 */
void be_code_patchjump(bfuncinfo_t *finfo, int jmp);

/**
 * @fn int be_code_getmethod(bfuncinfo_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @return (???)
 */
int be_code_getmethod(bfuncinfo_t *finfo, bexpdesc_t *e);

/**
 * @fn void be_code_call(bfuncinfo_t*, int, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param base (???)
 * @param argc (???)
 */
void be_code_call(bfuncinfo_t *finfo, int base, int argc);

/**
 * @fn int be_code_proto(bfuncinfo_t*, bproto_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param proto (???)
 * @return (???)
 */
int be_code_proto(bfuncinfo_t *finfo, bproto_t *proto);

/**
 * @fn void be_code_closure(bfuncinfo_t*, bexpdesc_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @param idx (???)
 */
void be_code_closure(bfuncinfo_t *finfo, bexpdesc_t *e, int idx);

/**
 * @fn void be_code_close(bfuncinfo*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param isret (???)
 */
void be_code_close(bfuncinfo_t *finfo, int isret);

/**
 * @fn void be_code_class(bfuncinfo_t*, bexpdesc_t*, bclass_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param dst (???)
 * @param c (???)
 */
void be_code_class(bfuncinfo_t *finfo, bexpdesc_t *dst, bclass_t *c);

/**
 * @fn void be_code_ret(bfuncinfo_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 */
void be_code_ret(bfuncinfo_t *finfo, bexpdesc_t *e);

/**
 * @fn int be_code_nglobal(bfuncinfo_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param k (???)
 * @return (???)
 */
int be_code_nglobal(bfuncinfo_t *finfo, bexpdesc_t *k);

/**
 * @fn void be_code_member(bfuncinfo_t*, bexpdesc_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e1 (???)
 * @param e2 (???)
 */
void be_code_member(bfuncinfo_t *finfo, bexpdesc_t *e1, bexpdesc_t *e2);

/**
 * @fn void be_code_index(bfuncinfo_t*, bexpdesc_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param c (???)
 * @param k (???)
 */
void be_code_index(bfuncinfo_t *finfo, bexpdesc_t *c, bexpdesc_t *k);

/**
 * @fn void be_code_setsuper(bfuncinfo_t*, bexpdesc_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param c (???)
 * @param s (???)
 */
void be_code_setsuper(bfuncinfo_t *finfo, bexpdesc_t *c, bexpdesc_t *s);

/**
 * @fn void be_code_import(bfuncinfo_t*, bexpdesc_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param m (???)
 * @param v (???)
 */
void be_code_import(bfuncinfo_t *finfo, bexpdesc_t *m, bexpdesc_t *v);

/**
 * @fn int be_code_exblk(bfuncinfo_t*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param depth (???)
 * @return (???)
 */
int be_code_exblk(bfuncinfo_t *finfo, int depth);

/**
 * @fn void be_code_catch(bfuncinfo_t*, int, int, int, int*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param base (???)
 * @param ecnt (???)
 * @param vcnt (???)
 * @param jmp (???)
 */
void be_code_catch(bfuncinfo_t *finfo, int base, int ecnt, int vcnt, int *jmp);

/**
 * @fn void be_code_raise(bfuncinfo_t*, bexpdesc_t*, bexpdesc_t*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e1 (???)
 * @param e2 (???)
 */
void be_code_raise(bfuncinfo_t *finfo, bexpdesc_t *e1, bexpdesc_t *e2);

#endif
