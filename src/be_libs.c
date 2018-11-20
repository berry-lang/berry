#include "be_libs.h"
#include "be_baselib.h"
#include "be_listlib.h"
#include "be_maplib.h"

void be_loadlibs(bvm *vm)
{
    be_loadbaselib(vm);
    be_list_init(vm);
    be_map_init(vm);
}
