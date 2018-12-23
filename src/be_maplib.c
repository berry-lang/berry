#include "be_maplib.h"

static int m_init(bvm *vm)
{
    if (be_top(vm) > 1 && be_ismap(vm, 2)) {
        be_pushvalue(vm, 2);
        be_setmember(vm, 1, "__data__");
    } else {
        be_newmap(vm);
        be_setmember(vm, 1, "__data__");
    }
    return be_returnnil(vm);
}

static int m_tostring(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushstring(vm, "{");
    be_pushiter(vm, -2); /* map iterator use 1 register */
    while (be_hasnext(vm, -3)) {
        be_next(vm, -3);
        /* key.tostring() */
        if (be_isstring(vm, -2)) { /* add ''' to strings */
            be_pushfstring(vm, "'%s'", be_tostring(vm, -2));
        } else {
            be_tostring(vm, -2);
            be_pushvalue(vm, -2); /* push to top */
        }
        be_strconcat(vm, -5);
        be_pop(vm, 1);
        be_pushstring(vm, ": "); /* add ': ' */
        be_strconcat(vm, -5);
        be_pop(vm, 1);
        /* value.tostring() */
        if (be_isstring(vm, -1)) { /* add ''' to strings */
            be_pushfstring(vm, "'%s'", be_tostring(vm, -1));
        } else {
            be_tostring(vm, -1);
            be_pushvalue(vm, -1); /* push to top */
        }
        be_strconcat(vm, -5);
        be_pop(vm, 3);
        if (be_hasnext(vm, -3)) {
            be_pushstring(vm, ", ");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        }
    }
    be_pop(vm, 1); /* pop iterator */
    be_pushstring(vm, "}");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    return be_return(vm);
}

static int m_insert(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_pushbool(vm, be_insert(vm, -3));
    return be_return(vm);
}

static int m_remove(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_pushbool(vm, be_remove(vm, -2));
    return be_return(vm);
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
    be_next(vm, -2); /* map next key and value */
    be_pushvalue(vm, -3); /* push __iter__ to top */
    be_setmember(vm, 1, "__iter__");
    be_pop(vm, 1);
    return be_return(vm); /* return value */
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

void be_map_init(bvm *vm)
{
    static const bmemberinfo members[] = {
        { "__data__", NULL },
        { "init", m_init },
        { "tostring", m_tostring },
        { "insert", m_insert },
        { "remove", m_remove },
        { "item", m_item },
        { "setitem", m_setitem },
        { "size", m_size },
        { "iter", m_iter },
        { NULL, NULL }
    };
    be_regclass(vm, "map", members);
}
