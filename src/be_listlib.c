#include "be_listlib.h"
#include <string.h>

#define list_check_data(vm, argc)                       \
    if (!be_islist(vm, -1) || be_top(vm) - 1 < argc) {  \
        return be_returnnil(vm);                        \
    }

static int m_init(bvm *vm)
{
    int i, argc = be_top(vm);
    if (argc > 1 && be_islist(vm, 2)) {
        be_pushvalue(vm, 2);
        be_setmember(vm, 1, "__data__");
    } else {
        be_newlist(vm);
        be_setmember(vm, 1, "__data__");
        for (i = 2; i <= argc; ++i) {
            be_pushvalue(vm, i);
            be_append(vm, -2);
            be_pop(vm, 1);
        }
    }
    return be_returnnil(vm);
}

static int m_tostring(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 1);
    be_pushstring(vm, "[");
    be_pushiter(vm, -2);
    while (be_hasnext(vm, -3)) {
        be_next(vm, -3);
        if (be_isstring(vm, -1)) { /* Add '"' to strings */
            be_pushfstring(vm, "'%s'", be_tostring(vm, -1));
			be_removeone(vm, -2);
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
    list_check_data(vm, 2);
    be_pushvalue(vm, 2);
    be_append(vm, -2);
    return be_returnnil(vm);
}

static int m_insert(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 3);
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_insert(vm, -3);
    return be_returnnil(vm);
}

static int m_remove(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 2);
    be_pushvalue(vm, 2);
    be_remove(vm, -2);
    return be_returnnil(vm);
}

static int item_range(bvm *vm)
{
    int lower, upper;
    int size = be_size(vm, -1); /* get source list size */
    /* get index range */
    be_getmember(vm, 2, "__lower__");
    lower = be_toint(vm, -1);
    be_pop(vm, 1);
    be_getmember(vm, 2, "__upper__");
    upper = be_toint(vm, -1);
    be_pop(vm, 1);
    /* protection scope */
    upper = upper < size ? upper : size - 1;
    lower = lower < 0 ? 0 : lower;
    /* construction result list instance */
    be_getglobal(vm, "list");
    be_call(vm, 0);
    be_getmember(vm, -1, "__data__"); /* result list */
    be_getmember(vm, 1, "__data__"); /* source list */
    /* copy elements */
    for (; lower <= upper; ++lower) {
        be_pushint(vm, lower);
        be_getindex(vm, -2);
        be_append(vm, -4);
        be_pop(vm, 2);
    }
    be_pop(vm, 2);
    return be_return(vm);
}

static int item_list(bvm *vm)
{
    int i, srcsize, idxsize;
    be_getmember(vm, 2, "__data__"); /* get index list */
    srcsize = be_size(vm, -2); /* get source list size */
    idxsize = be_size(vm, -1); /* get index list size */
    /* construction result list instance */
    be_getglobal(vm, "list");
    be_call(vm, 0);
    be_getmember(vm, -1, "__data__"); /* result list */
    be_getmember(vm, 1, "__data__"); /* source list */
    /* copy elements */
    for (i = 0; i < idxsize; ++i) {
        be_pushint(vm, i);
        be_getindex(vm, -5);
        if (be_isint(vm, -1)) {
            int idx = be_toint(vm, -1);
            if (idx >= 0 && idx < srcsize) {
                be_getindex(vm, -3);
            } else {
                be_pushnil(vm);
            }
        } else {
            be_pushnil(vm);
        }
        be_append(vm, -5);
        be_pop(vm, 3);
    }
    be_pop(vm, 2);
    return be_return(vm);
}

static int m_item(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 2);
    if (be_isint(vm, 2)) {
        be_pushvalue(vm, 2);
        be_getindex(vm, -2);
        return be_return(vm);
    }
    if (be_isinstance(vm, 2)) {
        const char *cname = be_classname(vm, 2);
        if (!strcmp(cname, "range")) {
            return item_range(vm);
        }
        if (!strcmp(cname, "list")) {
            return item_list(vm);
        }
    }
    return be_returnnil(vm);
}

static int m_setitem(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 3);
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_setindex(vm, -3);
    return be_returnnil(vm);
}

static int m_size(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 1);
    be_getsize(vm, -1);
    return be_return(vm);
}

static int m_resize(bvm *vm)
{
    be_getmember(vm, 1, "__data__");
    list_check_data(vm, 2);
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
        { "insert", m_insert },
        { "remove", m_remove },
        { "item", m_item },
        { "setitem", m_setitem },
        { "size", m_size },
        { "resize", m_resize },
        { "iter", m_iter },
        { NULL, NULL }
    };
    be_regclass(vm, "list", members);
}
