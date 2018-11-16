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
    be_getsuper(vm, 1);
    return be_returnvalue(vm);
}

static int l_type(bvm *vm)
{
    const char *t = be_typename(vm, 1);
    be_pushstring(vm, t);
    return be_returnvalue(vm);
}

static int l_otype(bvm *vm)
{
    const char *t = be_objecttype(vm, 1);
    if (t) {
        be_pushstring(vm, t);
    } else {
        be_pushnil(vm);
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
    be_regcfunc(vm, "otype", l_otype, 1);
}
