#include "be_listlib.h"

static int m_init(bvm *vm)
{
    int i, argc = be_top(vm);
    be_newlist(vm);
    be_setmember(vm, 1, "__data__");
    for (i = 2; i <= argc; ++i) {
        be_getmember(vm, 1, "__data__");
        be_pushvalue(vm, i);
        be_append(vm, -2);
    }
    return be_returnnil(vm);
}

static int m_tostring(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushstring(vm, "[");
    be_pushiter(vm, -2);
    while (be_hasnext(vm, -3)) {
        be_next(vm, -3);
        if (be_isstring(vm, -1)) { /* Add '"' to strings */
            be_pushfstring(vm, "'%s'", be_tostring(vm, -1));
            be_moveto(vm, -2);
            be_pop(vm, 1);
        } else {
            be_tostring(vm, -1);
        }
        be_strconcat(vm, -3);
        be_pop(vm, 1);
        if (be_hasnext(vm, -3)) {
            be_pushstring(vm, ", ");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        }
    }
    be_pop(vm, 1); /* pop iterator */
    be_pushstring(vm, "]");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    return be_return(vm);
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
        { "tostring", m_tostring },
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
