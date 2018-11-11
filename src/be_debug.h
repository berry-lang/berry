#ifndef __BE_DEBUG_H
#define __BE_DEBUG_H

#include "be_object.h"

void be_abort(void);
void be_printf(const char *format, ...);
void be_print_value(bvm *vm, bvalue *v, int refstr);
void be_dprintcode(bclosure *cl);

#endif
