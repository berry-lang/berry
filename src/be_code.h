/**
 * @file       be_code.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_CODE_H
#define BE_CODE_H

#include "be_parser.h"

#define be_code_freeregs(f, n)      ((f)->freereg -= (bbyte)(n)) /**< be_code_freeregs */

/**
 * @fn int be_code_allocregs(bfuncinfo*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param count (???)
 * @return (???)
 */
int be_code_allocregs(bfuncinfo *finfo, int count);

/**
 * @fn void be_code_prebinop(bfuncinfo*, int, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param op (???)
 * @param e (???)
 */
void be_code_prebinop(bfuncinfo *finfo, int op, bexpdesc *e);

/**
 * @fn void be_code_binop(bfuncinfo*, int, bexpdesc*, bexpdesc*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param op (???)
 * @param e1 (???)
 * @param e2 (???)
 * @param dst (???)
 */
void be_code_binop(bfuncinfo *finfo, int op, bexpdesc *e1, bexpdesc *e2, int dst);

/**
 * @fn int be_code_unop(bfuncinfo*, int, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param op (???)
 * @param e (???)
 * @return (???)
 */
int be_code_unop(bfuncinfo *finfo, int op, bexpdesc *e);

/**
 * @fn int be_code_setvar(bfuncinfo*, bexpdesc*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e1 (???)
 * @param e2 (???)
 * @return (???)
 */
int be_code_setvar(bfuncinfo *finfo, bexpdesc *e1, bexpdesc *e2);

/**
 * @fn int be_code_nextreg(bfuncinfo*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @return (???)
 */
int be_code_nextreg(bfuncinfo *finfo, bexpdesc *e);

/**
 * @fn int be_code_jump(bfuncinfo*)
 * @brief (???)
 *
 * @param finfo (???)
 * @return (???)
 */
int be_code_jump(bfuncinfo *finfo);

/**
 * @fn void be_code_jumpto(bfuncinfo*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param dst (???)
 */
void be_code_jumpto(bfuncinfo *finfo, int dst);

/**
 * @fn void be_code_jumpbool(bfuncinfo*, bexpdesc*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @param jumptrue (???)
 */
void be_code_jumpbool(bfuncinfo *finfo, bexpdesc *e, int jumptrue);

/**
 * @fn void be_code_conjump(bfuncinfo*, int*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param list (???)
 * @param jmp (???)
 */
void be_code_conjump(bfuncinfo *finfo, int *list, int jmp);

/**
 * @fn void be_code_patchlist(bfuncinfo*, int, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param list (???)
 * @param dst (???)
 */
void be_code_patchlist(bfuncinfo *finfo, int list, int dst);

/**
 * @fn void be_code_patchjump(bfuncinfo*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param jmp (???)
 */
void be_code_patchjump(bfuncinfo *finfo, int jmp);

/**
 * @fn int be_code_getmethod(bfuncinfo*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @return (???)
 */
int be_code_getmethod(bfuncinfo *finfo, bexpdesc *e);

/**
 * @fn void be_code_call(bfuncinfo*, int, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param base (???)
 * @param argc (???)
 */
void be_code_call(bfuncinfo *finfo, int base, int argc);

/**
 * @fn int be_code_proto(bfuncinfo*, bproto*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param proto (???)
 * @return (???)
 */
int be_code_proto(bfuncinfo *finfo, bproto *proto);

/**
 * @fn void be_code_closure(bfuncinfo*, bexpdesc*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 * @param idx (???)
 */
void be_code_closure(bfuncinfo *finfo, bexpdesc *e, int idx);

/**
 * @fn void be_code_close(bfuncinfo*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param isret (???)
 */
void be_code_close(bfuncinfo *finfo, int isret);

/**
 * @fn void be_code_class(bfuncinfo*, bexpdesc*, bclass*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param dst (???)
 * @param c (???)
 */
void be_code_class(bfuncinfo *finfo, bexpdesc *dst, bclass *c);

/**
 * @fn void be_code_ret(bfuncinfo*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e (???)
 */
void be_code_ret(bfuncinfo *finfo, bexpdesc *e);

/**
 * @fn int be_code_nglobal(bfuncinfo*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param k (???)
 * @return (???)
 */
int be_code_nglobal(bfuncinfo *finfo, bexpdesc *k);

/**
 * @fn void be_code_member(bfuncinfo*, bexpdesc*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e1 (???)
 * @param e2 (???)
 */
void be_code_member(bfuncinfo *finfo, bexpdesc *e1, bexpdesc *e2);

/**
 * @fn void be_code_index(bfuncinfo*, bexpdesc*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param c (???)
 * @param k (???)
 */
void be_code_index(bfuncinfo *finfo, bexpdesc *c, bexpdesc *k);

/**
 * @fn void be_code_setsuper(bfuncinfo*, bexpdesc*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param c (???)
 * @param s (???)
 */
void be_code_setsuper(bfuncinfo *finfo, bexpdesc *c, bexpdesc *s);

/**
 * @fn void be_code_import(bfuncinfo*, bexpdesc*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param m (???)
 * @param v (???)
 */
void be_code_import(bfuncinfo *finfo, bexpdesc *m, bexpdesc *v);

/**
 * @fn int be_code_exblk(bfuncinfo*, int)
 * @brief (???)
 *
 * @param finfo (???)
 * @param depth (???)
 * @return (???)
 */
int be_code_exblk(bfuncinfo *finfo, int depth);

/**
 * @fn void be_code_catch(bfuncinfo*, int, int, int, int*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param base (???)
 * @param ecnt (???)
 * @param vcnt (???)
 * @param jmp (???)
 */
void be_code_catch(bfuncinfo *finfo, int base, int ecnt, int vcnt, int *jmp);

/**
 * @fn void be_code_raise(bfuncinfo*, bexpdesc*, bexpdesc*)
 * @brief (???)
 *
 * @param finfo (???)
 * @param e1 (???)
 * @param e2 (???)
 */
void be_code_raise(bfuncinfo *finfo, bexpdesc *e1, bexpdesc *e2);

#endif
