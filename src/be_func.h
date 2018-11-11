#ifndef __BE_FUNC_H
#define __BE_FUNC_H

#include "be_object.h"

void be_initupvals(bvm *vm, bclosure *cl);
bupval* be_findupval(bvm *vm, bvalue *level);
void be_upvals_close(bvm *vm, bvalue *level);
bproto* be_newproto(bvm *vm);
bclosure* be_newclosure(bvm *vm, int nupval);
bprimfunc* be_newprimfunc(bvm *vm, bcfunction cf, int argc);

#endif
