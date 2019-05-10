#ifndef BE_VAR_H
#define BE_VAR_H

#include "be_object.h"

#define be_global_count(vm) \
    ((vm)->gbldesc.global.vlist.count)

#define be_builtin_count(vm) \
    ((vm)->gbldesc.builtin.vlist.count)

void be_globalvar_init(bvm *vm);
void be_globalvar_deinit(bvm *vm);
int be_global_find(bvm *vm, bstring *name);
int be_global_new(bvm *vm, bstring *name);
bvalue* be_global_var(bvm *vm, int index);
void be_global_release_space(bvm *vm);
int be_builtin_find(bvm *vm, bstring *name);
int be_builtin_new(bvm *vm, bstring *name);
void be_bulitin_release_space(bvm *vm);
void be_const_builtin_set(bvm *vm, const bmap *map, const bvector *vec);

#endif
