#ifndef __BE_FUNC_H
#define __BE_FUNC_H

#include "be_object.h"

#define be_newntvfunc(vm, cf, argc) \
    be_newprimclosure(vm, cf, argc, 0)

#define be_ntvfunc_upval(cc, n) \
    (((bupval**)((size_t)cc + sizeof(bntvfunc)))[n])

void be_initupvals(bvm *vm, bclosure *cl);
bupval* be_findupval(bvm *vm, bvalue *level);
void be_upvals_close(bvm *vm, bvalue *level);
bproto* be_newproto(bvm *vm);
bclosure* be_newclosure(bvm *vm, int nupval);
bntvfunc* be_newprimclosure(bvm *vm, bcfunction cf, int argc, int nupvals);

#endif
