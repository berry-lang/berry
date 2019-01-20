#ifndef __BE_MODULE_H
#define __BE_MODULE_H

#include "be_object.h"

typedef struct bmodule {
    bcommon_header;
    const bnative_module *native;
    bmap *table;
    struct bmodule *mnext;
} bmodule;

#define be_module_name(module)  ((module)->native->name)

bmodule* be_module_load(bvm *vm, bstring *path, bvalue *dst);
void be_module_delete(bvm *vm, bmodule *module);
bvalue* be_module_attr(bmodule *module, bstring *attr);

#endif
