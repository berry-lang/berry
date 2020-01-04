#ifndef BE_DEBUG_H
#define BE_DEBUG_H

#include "be_object.h"

void be_dumpclosure(bclosure *cl);
void be_tracestack(bvm *vm);
void be_debug_hook_init(bvm *vm);
void be_debug_hook(bvm *vm, binstruction ins);

#endif
