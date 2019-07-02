#ifndef BE_CODE_H
#define BE_CODE_H

#include "be_parser.h"

#define be_code_goiftrue(f, e)      be_code_jumpbool(f, e, 0)
#define be_code_goiffalse(f, e)     be_code_jumpbool(f, e, 1)
#define be_code_freeregs(f, n)      ((f)->freereg -= (bbyte)(n))

int be_code_allocregs(bfuncinfo *finfo, int count);
void be_code_prebinop(bfuncinfo *finfo, int op, bexpdesc *e);
void be_code_binop(bfuncinfo *finfo, int op, bexpdesc *e1, bexpdesc *e2);
int be_code_unop(bfuncinfo *finfo, int op, bexpdesc *e);
int be_code_setvar(bfuncinfo *finfo, bexpdesc *e1, bexpdesc *e2);
int be_code_nextreg(bfuncinfo *finfo, bexpdesc *e);
int be_code_jump(bfuncinfo *finfo);
void be_code_jumpto(bfuncinfo *finfo, int dst);
void be_code_jumpbool(bfuncinfo *finfo, bexpdesc *e, int jumptrue);
void be_code_conjump(bfuncinfo *finfo, int *list, int jmp);
void be_code_patchlist(bfuncinfo *finfo, int list, int dst);
void be_code_patchjump(bfuncinfo *finfo, int jmp);
int be_code_getmethod(bfuncinfo *finfo, bexpdesc *e);
void be_code_call(bfuncinfo *finfo, int base, int argc);
int be_code_proto(bfuncinfo *finfo, bproto *proto);
void be_code_closure(bfuncinfo *finfo, bexpdesc *e, int idx);
void be_code_close(bfuncinfo *finfo, int isret);
void be_code_ret(bfuncinfo *finfo, bexpdesc *e);
void be_code_member(bfuncinfo *finfo, bexpdesc *e1, bexpdesc *e2);
void be_code_index(bfuncinfo *finfo, bexpdesc *c, bexpdesc *k);
bvalue* be_code_localobject(bfuncinfo *finfo, int dst);
void be_code_setsuper(bfuncinfo *finfo, bexpdesc *c, bexpdesc *s);
void be_code_import(bfuncinfo *finfo, bexpdesc *m, bexpdesc *v);

#endif
