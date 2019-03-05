#include "be_object.h"
#include "be_mem.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

static int l_assert(bvm *vm)
{
    int pass = be_tobool(vm, 1);

    if (pass == bfalse) {
        be_pusherror(vm, "Assert failed!");
    }
    be_return_nil(vm);
}

static int l_print(bvm *vm)
{
    int i, n = be_top(vm);
    for (i = 1; i <= n; ++i) {
        const char *str = be_tostring(vm, i);
        size_t len = be_strlen(vm, i);
        be_fwrite(stdout, str, len);
        if (i < n) {
            be_fwrite(stdout, " ", 1);
        }
    }
    be_fwrite(stdout, "\n", 1);
    be_return_nil(vm);
}

static int l_clock(bvm *vm)
{
    be_pushreal(vm, clock() / (breal)CLOCKS_PER_SEC);
    be_return(vm);
}

static int l_exit(bvm *vm)
{
    int status = 0;
    if (be_isint(vm, -1)) {
        status = be_toint(vm, -1);
    }
    be_exit(vm, status);
    be_return_nil(vm);
}

static int l_memcount(bvm *vm)
{
    size_t count = be_mcount();
    if (count < 0x80000000) {
        be_pushint(vm, (bint)count);
    } else {
        be_pushreal(vm, (breal)count);
    }
    be_return(vm);
}

static int l_super(bvm *vm)
{
    be_getsuper(vm, 1);
    be_return(vm);
}

static int l_type(bvm *vm)
{
    const char *t = be_typename(vm, 1);
    be_pushstring(vm, t);
    be_return(vm);
}

static int l_classname(bvm *vm)
{
    const char *t = be_classname(vm, 1);
    if (t) {
        be_pushstring(vm, t);
    } else {
        be_pushnil(vm);
    }
    be_return(vm);
}

static int l_number(bvm *vm)
{
    if (be_isstring(vm, 1)) {
        const char *str = be_tostring(vm, 1);
        be_str2num(vm, str);
        be_return(vm);
    } else if (be_isnumber(vm, -1)) {
        be_return(vm);
    }
    be_return_nil(vm);
}

static int l_iterator(bvm *vm)
{
    if (be_isinstance(vm, 1)) {
        be_getmember(vm, 1, "iter");
        be_pushvalue(vm, 1);
        be_call(vm, 1);
        be_pop(vm, 1);
        be_return(vm);
    }
    be_return_nil(vm);
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
    be_return(vm);
}

static int l_next(bvm *vm)
{
    if (be_isinstance(vm, 1)) {
        be_getmember(vm, 1, "next");
        be_pushvalue(vm, 1);
        be_call(vm, 1);
        be_pop(vm, 1);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int l_str(bvm *vm)
{
    be_tostring(vm, 1);
    be_return(vm);
}

static int l_length(bvm *vm)
{
    if (be_isstring(vm, 1)) {
        be_pushint(vm, be_strlen(vm, 1));
    } else {
        be_pushint(vm, 0);
    }
    be_return(vm);
}

void be_load_baselib(bvm *vm)
{
    be_regcfunc(vm, "assert", l_assert);
    be_regcfunc(vm, "print", l_print);
    be_regcfunc(vm, "clock", l_clock);
    be_regcfunc(vm, "exit", l_exit);
    be_regcfunc(vm, "super", l_super);
    be_regcfunc(vm, "memcount", l_memcount);
    be_regcfunc(vm, "type", l_type);
    be_regcfunc(vm, "classname", l_classname);
    be_regcfunc(vm, "number", l_number);
    be_regcfunc(vm, "str", l_str);
    be_regcfunc(vm, "length", l_length);
    be_regcfunc(vm, "__iterator__", l_iterator);
    be_regcfunc(vm, "__hasnext__", l_hasnext);
    be_regcfunc(vm, "__next__", l_next);
}
