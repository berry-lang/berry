#include "be_baselib.h"
#include "be_mem.h"
#include <time.h>

static int l_print(bvm *vm)
{
    int i, n = be_top(vm);
    for (i = 1; i <= n; ++i) {
        be_printvalue(vm, 0, i);
        be_printf(" ");
    }
    be_printf("\n");
    return 0;
}

static int l_clock(bvm *vm)
{
    be_pushreal(vm, clock() / (breal)CLOCKS_PER_SEC);
    return be_returnvalue(vm);
}

static int l_memcount(bvm *vm)
{
    be_pushreal(vm, (breal)be_mcount());
    return be_returnvalue(vm);
}

static int l_super(bvm *vm)
{
    be_pushvalue(vm, 1);
    be_getsuper(vm, -1);
    return be_returnvalue(vm);
}

static int l_type(bvm *vm)
{
    switch(be_type(vm, 1)) {
    case BE_NIL: be_pushstring(vm, "nil"); break;
    case BE_BOOL: be_pushstring(vm, "bool"); break;
    case BE_INT: be_pushstring(vm, "int"); break;
    case BE_REAL: be_pushstring(vm, "real"); break;
    case BE_CLASS: be_pushstring(vm, "class"); break;
    case BE_INSTANCE: be_getobjtype(vm, 1); break;
    default: be_pushnil(vm); break;
    }
    return be_returnvalue(vm);
}

void be_loadbaselib(bvm *vm)
{
    be_regcfunc(vm, "print", l_print, -1);
    be_regcfunc(vm, "clock", l_clock, 0);
    be_regcfunc(vm, "super", l_super, 1);
    be_regcfunc(vm, "memcount", l_memcount, 0);
    be_regcfunc(vm, "type", l_type, 1);
}
