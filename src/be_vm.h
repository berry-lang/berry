#ifndef BE_VM_H
#define BE_VM_H

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
    bglobaldesc gbldesc; /* global description */
    bvalue *stack; /* stack space */
    bvalue *stacktop; /* stack top register */
    bupval *upvalist; /* open upvalue list */
    bstack callstack; /* function call stack */
    bcallframe *cf; /* function call frame */
    bvalue *reg; /* function base register */
    bvalue *top; /* function top register */
	binstruction *ip; /* function instruction pointer */
    struct blongjmp *errjmp; /* error jump point */
    bstack refstack; /* object reference stack */
    struct bmodule *modulelist;
};

#define NONE_FLAG           0
#define BASE_FRAME          (1 << 0)
#define PRIM_FUNC           (1 << 1)

#define var2cl(_v)          cast(bclosure*, var_toobj(_v))
#define curcl(_vm)          var2cl((_vm)->cf->func)

void be_dofunc(bvm *vm, bvalue *v, int argc);

#endif
