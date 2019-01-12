#ifndef __BE_VM_H
#define __BE_VM_H

#include "be_object.h"

typedef struct bglobaldesc {
    bmap *idxtab; /* global variable index table */
    bvector gvalist; /* global variable list */
    int nglobal; /* global variable count */
} bglobaldesc;

typedef struct {
    bvalue *func; /* function register pointer */
    bvalue *top; /* top register pointer */
    bvalue *reg; /* base register pointer */
    binstruction *ip; /* instruction pointer (only berry-function) */
    int status;
} bcallframe;

struct bvm {
    bstringtable *strtab;
    bgc *gc;
    bvalue *global; /* global variable vector */
    bvalue *stack; /* stack space */
    bvalue *stacktop;
    bupval *upvalist; /* open upvalue list */
    bstack callstack; /* function call stack */
    bglobaldesc gbldesc; /* global description */
    bcallframe *cf; /* function call frame */
    bvalue *reg;
    bvalue *top;
	binstruction *ip;
    struct blongjmp *errjmp; /* error jump point */
    bstack refstack; /* object reference stack */
};

#define NONE_FLAG           0
#define BASE_FRAME          (1 << 0)
#define PRIM_FUNC           (1 << 1)

#define var2cl(_v)          cast(bclosure*, var_toobj(_v))
#define curcl(_vm)          var2cl((_vm)->cf->func)

void be_dofunc(bvm *vm, bvalue *v, int argc);

#endif
