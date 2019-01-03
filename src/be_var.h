#ifndef __BE_VAR_H
#define __BE_VAR_H

#include "be_object.h"

#define be_globalvar(vm, idx)   ((idx) + (vm)->global)
#define be_globalvar_count(vm)  ((vm)->gbldesc.nglobal)

void be_globalvar_init(bvm *vm);
void be_globalvar_deinit(bvm *vm);
int be_globalvar_find(bvm *vm, bstring *name);
int be_globalvar_new(bvm *vm, bstring *name);

#endif
