#ifndef __BE_VM_H
#define __BE_VM_H

#include "be_object.h"

typedef struct bglobaldesc {
    bmap *idxtab; /* global variable index table */
    struct bvector *gvalist; /* global variable list */
    int nglobal; /* global variable count */
} bglobaldesc;

typedef struct {
    bvalue *reg; /* base register pointer */
    union {
        bvalue *top; /* top register pointer (only C-function) */
        struct {
            binstruction *ip; /* instruction pointer */
            bclosure *closure;
        } s; /* closure frame data */
    } u;
    int status;
} bcallframe;

struct bvm {
    bstringtable *strtab;
    bgc *gc;
    bvalue *global; /* global variable vector */
    bvalue *stack; /* stack space */
    bupval *upvalist; /* open upvalue list */
    struct bvector *callstack; /* function call stack */
    bglobaldesc gbldesc; /* global description */
    bcallframe *cf; /* function call frame */
};

#define NONE_FLAG           0
#define BASE_FRAME          (1 << 0)
#define PRIM_FUNC           (1 << 1)

bvm* be_vm_new(int nstack);
void be_exec(bvm *vm);
void be_dofunc(bvm *vm, bclosure *cl, int argc);
void be_doprimfunc(bvm *vm, bprimfunc *f, int argc);
void be_dofuncvar(bvm *vm, bvalue *v, int argc);

#endif
