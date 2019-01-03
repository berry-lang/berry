#ifndef __BE_DEBUG_H
#define __BE_DEBUG_H

#include "be_object.h"

void be_dprintcode(bclosure *cl);
void be_debug_error(bvm *vm, int errcode, const char *format, ...);
void be_debug_ins_info(bvm *vm);

#endif
