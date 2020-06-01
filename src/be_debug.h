#ifndef BE_DEBUG_H
#define BE_DEBUG_H

#include "be_object.h"

#define BE_HOOK_LINE    1
#define BE_HOOK_CALL    2
#define BE_HOOK_RET     4

void be_dumpclosure(bclosure *cl);
void be_tracestack(bvm *vm);
void be_callhook(bvm *vm, int mask);

#endif
