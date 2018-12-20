#include "be_baselib.h"
#include "be_mem.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

static int l_print(bvm *vm)
{
    int i, n = be_top(vm);
    for (i = 1; i <= n; ++i) {
        be_printf("%s ", be_tostring(vm, i));
    }
    be_printf("\n");
    return be_returnnil(vm);
}

static int l_clock(bvm *vm)
{
    be_pushreal(vm, clock() / (breal)CLOCKS_PER_SEC);
    return be_return(vm);
}

static int l_memcount(bvm *vm)
{
    size_t count = be_mcount();
    if (count < 0x80000000) {
        be_pushint(vm, (bint)count);
    } else {
        be_pushreal(vm, (breal)count);
    }
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

static int l_number(bvm *vm)
{
    if (be_isstring(vm, 1)) {
        const char *str = be_tostring(vm, 1);
        if (strchr(str, '.')) {
            be_pushreal(vm, be_str2real(str, NULL));
        } else {
            be_pushint(vm, be_str2int(str, NULL));
        }
        return be_return(vm);
    } else if (be_isnumber(vm, -1)) {
        return be_return(vm);
    }
    return be_returnnil(vm);
}

static int l_random(bvm *vm)
{
    be_pushint(vm, rand());
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
    return be_returnnil(vm);
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
    return be_returnnil(vm);
}

static int l_string(bvm *vm)
{
    be_tostring(vm, 1);
    return be_return(vm);
}

void be_loadbaselib(bvm *vm)
{
    be_regcfunc(vm, "print", l_print);
    be_regcfunc(vm, "clock", l_clock);
    be_regcfunc(vm, "super", l_super);
    be_regcfunc(vm, "memcount", l_memcount);
    be_regcfunc(vm, "type", l_type);
    be_regcfunc(vm, "classname", l_classname);
    be_regcfunc(vm, "number", l_number);
    be_regcfunc(vm, "random", l_random);
    be_regcfunc(vm, "string", l_string);
    be_regcfunc(vm, "__iterator__", l_iterator);
    be_regcfunc(vm, "__hasnext__", l_hasnext);
    be_regcfunc(vm, "__next__", l_next);
}
