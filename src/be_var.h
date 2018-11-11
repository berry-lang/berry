#ifndef __BE_VAR_H
#define __BE_VAR_H

#include "be_object.h"

#define type(v)             ((v)->type)
#define basetype(v)         (type(v) & 0x1F)
#define set_type(v, t)      ((v)->type = (bbyte)(t))
#define cast_bool(v)        ((bbool)(v) != bfalse ? btrue : bfalse)
#define globalvar(vm, idx)  ((vm)->global + (idx))

#define be_globalvar_count(vm)  ((vm)->gbldesc.nglobal)

void be_globalvar_init(bvm *vm);
int be_globalvar_find(bvm *vm, bstring *name);
int be_globalvar_new(bvm *vm, bstring *name);

#endif
