#ifndef BE_VAR_H
#define BE_VAR_H

#include "be_object.h"

#define be_globalvar(vm, idx)   ((bvalue*)(vm)->gbldesc.gvalist.data + (idx))
#define be_globalvar_count(vm)  ((vm)->gbldesc.gvalist.count)

void be_globalvar_init(bvm *vm);
void be_globalvar_deinit(bvm *vm);
int be_globalvar_find(bvm *vm, bstring *name);
int be_globalvar_new(bvm *vm, bstring *name);
void be_globalvar_release_space(bvm *vm);

#endif
