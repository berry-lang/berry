#ifndef __BE_FUNC_H
#define __BE_FUNC_H

#include "be_object.h"

#define be_newntvclos(vm, cf) \
    be_newprimclosure(vm, cf, 0)

#define be_ntvclos_upval(cc, n) \
    (((bupval**)((size_t)cc + sizeof(bntvclos)))[n])

void be_initupvals(bvm *vm, bclosure *cl);
bupval* be_findupval(bvm *vm, bvalue *level);
void be_upvals_close(bvm *vm, bvalue *level);
bproto* be_newproto(bvm *vm);
bclosure* be_newclosure(bvm *vm, int nupval);
bntvclos* be_newprimclosure(bvm *vm, bntvfunc cf, int nupvals);

#endif
