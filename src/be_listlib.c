#include "be_listlib.h"

static int m_init(bvm *vm)
{
    be_newlist(vm);
    be_setmember(vm, 1, "__data__");
    return be_returnnil(vm);
}

static int m_print(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushiter(vm, -1);
    be_printf("[");
    while (be_hasnext(vm, -2)) {
        be_next(vm, -2);
        be_printvalue(vm, 1, -1);
        be_pop(vm, 1);
        if (be_hasnext(vm, -2)) {
            be_printf(", ");
        }
    }
    be_printf("]");
    return be_returnnil(vm);
}

static int m_append(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_append(vm, -2);
    return be_returnnil(vm);
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
    return be_returnnil(vm);
}

static int m_size(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_getsize(vm, -1);
    return be_return(vm);
}

static int m_resize(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_resize(vm, -2);
    return be_returnnil(vm);
}

static int i_init(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, "__obj__");
    be_pop(vm, 1);
    be_getmember(vm, 2, "__data__");
    be_pushiter(vm, -1);
    be_setmember(vm, 1, "__iter__");
    return be_returnnil(vm);
}

static int i_hashnext(bvm *vm)
{
    be_getmember(vm, 1, "__obj__");
    be_getmember(vm, -1, "__data__");
    be_getmember(vm, 1, "__iter__");
    be_pushbool(vm, be_hasnext(vm, -2));
    return be_return(vm);
}

static int i_next(bvm *vm)
{
    be_getmember(vm, 1, "__obj__");
    be_getmember(vm, -1, "__data__");
    be_getmember(vm, 1, "__iter__");
    be_next(vm, -2); /* list next value */
    be_pushvalue(vm, -2); /* push __iter__ to top */
    be_setmember(vm, 1, "__iter__");
    be_pop(vm, 1);
    return be_return(vm);
}

static int m_iter(bvm *vm)
{
    static const bmemberinfo members[] = {
        { "__obj__", NULL },
        { "__iter__", NULL },
        { "init", i_init },
        { "hasnext", i_hashnext },
        { "next", i_next },
        { NULL, NULL }
    };
    be_pushclass(vm, "iterator", members);
    be_pushvalue(vm, 1);
    be_call(vm, 1);
    be_pop(vm, 1);
    return be_return(vm);
}

void be_list_init(bvm *vm)
{
    static const bmemberinfo members[] = {
        { "__data__", NULL },
        { "init", m_init },
        { "print", m_print },
        { "append", m_append },
        { "item", m_item },
        { "setitem", m_setitem },
        { "size", m_size },
        { "resize", m_resize },
        { "iter", m_iter },
        { NULL, NULL }
    };
    be_regclass(vm, "list", members);
}
