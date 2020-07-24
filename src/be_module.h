#ifndef BE_MODULE_H
#define BE_MODULE_H

#include "be_object.h"

#define BE_MODULE_NAME    1

typedef struct bmodule {
    bcommon_header;
    bmap *table;
    union infodata {
        const bntvmodule *native;
        const char *name;
        const bstring *sname;
#if __cplusplus >= 199711L
        constexpr infodata(const char *name) : name(name) {}
#endif
    } info;
    bgcobject *gray; /* for gc gray list */
#if __cplusplus >= 199711L
    constexpr bmodule(bmap *tab, const char *name) :
        next(0), type(BE_MODULE), marked(GC_CONST),
        table(tab), info(infodata(name)), gray(0) {}
#endif
} bmodule;

bmodule* be_module_new(bvm *vm);
void be_module_delete(bvm *vm, bmodule *module);
int be_module_load(bvm *vm, bstring *path);
bvalue* be_module_attr(bvm *vm, bmodule *module, bstring *attr);
bvalue* be_module_bind(bvm *vm, bmodule *module, bstring *attr);
const char* be_module_name(bmodule *module);
bbool be_module_setname(bmodule *module, bstring *name);

#endif
