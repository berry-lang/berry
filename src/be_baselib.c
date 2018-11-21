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
    return be_noreturn(vm);
}

static int l_clock(bvm *vm)
{
    be_pushreal(vm, clock() / (breal)CLOCKS_PER_SEC);
    return be_return(vm);
}

static int l_memcount(bvm *vm)
{
    be_pushreal(vm, (breal)be_mcount());
    return be_return(vm);
}

static int l_super(bvm *vm)
{
    be_getsuper(vm, 1);
    return be_return(vm);
}

static int l_type(bvm *vm)
{
    const char *t = be_typename(vm, 1);
    be_pushstring(vm, t);
    return be_return(vm);
}

static int l_classname(bvm *vm)
{
    const char *t = be_classname(vm, 1);
    if (t) {
        be_pushstring(vm, t);
    } else {
        be_pushnil(vm);
    }
    return be_return(vm);
}

static int l_iterator(bvm *vm)
{
    if (be_isinstance(vm, 1)) {
        be_getmember(vm, 1, "iter");
        be_pushvalue(vm, 1);
        be_call(vm, 1);
        be_pop(vm, 1);
        return be_return(vm);
    }
    return be_noreturn(vm);
}

static int l_hasnext(bvm *vm)
{
    if (be_isinstance(vm, 1)) {
        be_getmember(vm, 1, "hasnext");
        be_pushvalue(vm, 1);
        be_call(vm, 1);
        be_pop(vm, 1);
    } else {
        be_pushbool(vm, bfalse);
    }
    return be_return(vm);
}

static int l_next(bvm *vm)
{
    if (be_isinstance(vm, 1)) {
        be_getmember(vm, 1, "next");
        be_pushvalue(vm, 1);
        be_call(vm, 1);
        be_pop(vm, 1);
        return be_return(vm);
    }
    return be_noreturn(vm);
}

void be_loadbaselib(bvm *vm)
{
    be_regcfunc(vm, "print", l_print);
    be_regcfunc(vm, "clock", l_clock);
    be_regcfunc(vm, "super", l_super);
    be_regcfunc(vm, "memcount", l_memcount);
    be_regcfunc(vm, "type", l_type);
    be_regcfunc(vm, "classname", l_classname);
    be_regcfunc(vm, "__iterator__", l_iterator);
    be_regcfunc(vm, "__hasnext__", l_hasnext);
    be_regcfunc(vm, "__next__", l_next);
}
