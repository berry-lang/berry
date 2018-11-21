#include "be_maplib.h"

static int m_init(bvm *vm)
{
    be_newmap(vm);
    be_setmember(vm, 1, "__data__");
    return be_noreturn(vm);
}

static int m_print(bvm *vm)
{
    int res, idx = 0, count;
    be_getmember(vm, 1, "__data__");
    count = be_size(vm, -1);
    be_pushnil(vm);
    be_printf("{");
    do {
        res = be_next(vm, -2);  /* self.__data__ */
        if (res == 2) { /* key and value */
            be_printvalue(vm, 1, -2);
            be_printf(": ");
            be_printvalue(vm, 1, -1);
            be_pop(vm, res);
        }
        if (++idx < count) {
            be_printf(", ");
        }
    } while (res);
    be_printf("}");
    return be_noreturn(vm);
}

static int m_insert(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_insert(vm, -3);
    return be_noreturn(vm);
}

static int m_remove(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_remove(vm, -2);
    return be_noreturn(vm);
}

static int m_item(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_getindex(vm, -2);
    return be_return(vm);
}

static int m_setitem(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_setindex(vm, -3);
    return be_noreturn(vm);
}

static int m_size(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_getsize(vm, -1);
    return be_return(vm);
}

static const bmemberinfo l_member[] = {
    { "__data__", NULL },
    { "init", m_init },
    { "print", m_print },
    { "insert", m_insert },
    { "remove", m_remove },
    { "item", m_item },
    { "setitem", m_setitem },
    { "size", m_size },
    { NULL, NULL }
};

void be_map_init(bvm *vm)
{
    be_regclass(vm, "map", l_member);
}
