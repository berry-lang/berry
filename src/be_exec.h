#ifndef __BE_EXEC_H
#define __BE_EXEC_H

#include "be_object.h"

typedef void (*bpfunc)(bvm *vm, void *data);

void be_throw(bvm *vm, int errorcode);
int be_execprotected(bvm *vm, bpfunc f, void *data);
int be_protectedparser(bvm *vm,
    const char *fname, const char *source, size_t length);
int be_protectedcall(bvm *vm, bvalue *v, int argc);
void be_stackpush(bvm *vm);
void be_stackcheck(bvm *vm, int need);
void be_stack_expansion(bvm *vm, int n);

#endif
