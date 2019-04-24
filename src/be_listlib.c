#include "be_object.h"
#include <string.h>

#define list_check_data(vm, argc)                       \
    if (!be_islist(vm, -1) || be_top(vm) - 1 < argc) {  \
        be_return_nil(vm);                              \
    }

#define list_check_ref(vm)                              \
    if (be_refcontains(vm, 1)) {                        \
        be_pushstring(vm, "[...]");                     \
        be_return(vm);                                  \
    }

static int m_init(bvm *vm)
{
    int i, argc = be_top(vm);
    if (argc > 1 && be_islist(vm, 2)) {
        be_pushvalue(vm, 2);
        be_setmember(vm, 1, ".data");
    } else {
        be_newlist(vm);
        be_setmember(vm, 1, ".data");
        for (i = 2; i <= argc; ++i) {
            be_pushvalue(vm, i);
            be_data_append(vm, -2);
            be_pop(vm, 1);
        }
    }
    be_return_nil(vm);
}

static void push_element(bvm *vm)
{
    if (be_isstring(vm, -1)) { /* Add '"' to strings */
        be_pushfstring(vm, "'%s'", be_tostring(vm, -1));
        be_remove(vm, -2);
    } else {
        be_tostring(vm, -1);
    }
    be_strconcat(vm, -3);
    be_pop(vm, 1);
}

static int m_tostring(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 1);
    list_check_ref(vm);
    be_refpush(vm, 1);
    be_pushstring(vm, "[");
    be_pushiter(vm, -2);
    while (be_iter_hasnext(vm, -3)) {
        be_iter_next(vm, -3);
        push_element(vm);
        if (be_iter_hasnext(vm, -3)) {
            be_pushstring(vm, ", ");
            be_strconcat(vm, -3);
            be_pop(vm, 1);
        }
    }
    be_pop(vm, 1); /* pop iterator */
    be_pushstring(vm, "]");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    be_refpop(vm);
    be_return(vm);
}

static int m_append(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 2);
    be_pushvalue(vm, 2);
    be_data_append(vm, -2);
    be_return_nil(vm);
}

static int m_insert(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 3);
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_data_insert(vm, -3);
    be_return_nil(vm);
}

static int m_remove(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 2);
    be_pushvalue(vm, 2);
    be_data_remove(vm, -2);
    be_return_nil(vm);
}

static int item_range(bvm *vm)
{
    bint lower, upper;
    int size = be_data_size(vm, -1); /* get source list size */
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
    be_getmember(vm, -1, ".data"); /* result list */
    be_getmember(vm, 1, ".data"); /* source list */
    /* copy elements */
    for (; lower <= upper; ++lower) {
        be_pushint(vm, lower);
        be_getindex(vm, -2);
        be_data_append(vm, -4);
        be_pop(vm, 2);
    }
    be_pop(vm, 2);
    be_return(vm);
}

static int item_list(bvm *vm)
{
    int i, srcsize, idxsize;
    be_getmember(vm, 2, ".data"); /* get index list */
    srcsize = be_data_size(vm, -2); /* get source list size */
    idxsize = be_data_size(vm, -1); /* get index list size */
    /* construction result list instance */
    be_getglobal(vm, "list");
    be_call(vm, 0);
    be_getmember(vm, -1, ".data"); /* result list */
    be_getmember(vm, 1, ".data"); /* source list */
    /* copy elements */
    for (i = 0; i < idxsize; ++i) {
        be_pushint(vm, i);
        be_getindex(vm, -5);
        if (be_isint(vm, -1)) {
            int idx = be_toindex(vm, -1);
            if (idx >= 0 && idx < srcsize) {
                be_getindex(vm, -3);
            } else {
                be_pushnil(vm);
            }
        } else {
            be_pushnil(vm);
        }
        be_data_append(vm, -5);
        be_pop(vm, 3);
    }
    be_pop(vm, 2);
    be_return(vm);
}

static int m_item(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 2);
    if (be_isint(vm, 2)) {
        be_pushvalue(vm, 2);
        be_getindex(vm, -2);
        be_return(vm);
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
    be_return_nil(vm);
}

static int m_setitem(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 3);
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_setindex(vm, -3);
    be_return_nil(vm);
}

static int m_size(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 1);
    be_pushint(vm, be_data_size(vm, -1));
    be_return(vm);
}

static int m_resize(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    list_check_data(vm, 2);
    be_pushvalue(vm, 2);
    be_data_resize(vm, -2);
    be_return_nil(vm);
}

static int i_init(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, ".obj");
    be_pop(vm, 1);
    be_getmember(vm, 2, ".data");
    be_pushiter(vm, -1);
    be_setmember(vm, 1, ".iter");
    be_return_nil(vm);
}

static int i_hashnext(bvm *vm)
{
    be_getmember(vm, 1, ".obj");
    be_getmember(vm, -1, ".data");
    be_getmember(vm, 1, ".iter");
    be_pushbool(vm, be_iter_hasnext(vm, -2));
    be_return(vm);
}

static int i_next(bvm *vm)
{
    be_getmember(vm, 1, ".obj");
    be_getmember(vm, -1, ".data");
    be_getmember(vm, 1, ".iter");
    be_iter_next(vm, -2); /* list next value */
    be_pushvalue(vm, -2); /* push .iter to top */
    be_setmember(vm, 1, ".iter");
    be_pop(vm, 1);
    be_return(vm);
}

static int m_iter(bvm *vm)
{
    static const bnfuncinfo members[] = {
        { ".obj", NULL },
        { ".iter", NULL },
        { "init", i_init },
        { "hasnext", i_hashnext },
        { "next", i_next },
        { NULL, NULL }
    };
    be_pushclass(vm, "iterator", members);
    be_pushvalue(vm, 1);
    be_call(vm, 1);
    be_pop(vm, 1);
    be_return(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
void be_load_listlib(bvm *vm)
{
    static const bnfuncinfo members[] = {
        { ".data", NULL },
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
#else
/* @const_object_info_begin
class m_cl_list (scope: local, name: list) {
    .data, var
    init, func(m_init)
    tostring, func(m_tostring)
    append, func(m_append)
    insert, func(m_insert)
    remove, func(m_remove)
    item, func(m_item)
    setitem, func(m_setitem)
    size, func(m_size)
    resize, func(m_resize)
    iter, func(m_iter)
}
@const_object_info_end */
#include "../generate/be_fixed_m_cl_list.h"

void be_load_listlib(bvm *vm)
{
    be_const_regclass(vm, &m_cl_list);
}
#endif
