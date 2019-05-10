#include "be_libs.h"

extern void be_load_baselib(bvm *vm);
extern void be_load_listlib(bvm *vm);
extern void be_load_maplib(bvm *vm);
extern void be_load_rangelib(bvm *vm);
extern void be_load_filelib(bvm *vm);

void be_loadlibs(bvm *vm)
{
    be_load_baselib(vm);
#if !BE_USE_PRECOMPILED_OBJECT
    be_load_listlib(vm);
    be_load_maplib(vm);
    be_load_rangelib(vm);
    be_load_filelib(vm);
#endif
}
