#include "be_listlib.h"

#define m_data(vm)      be_newstr(vm, "__data__")

static int m_init(bvm *vm)
{
    be_newlist(vm);
    be_setfield(vm, 1, "__data__");
    return be_nonereturn(vm);
}

static int m_print(bvm *vm)
{
    int i, count;
    be_getfield(vm, 1, "__data__");
    count = be_size(vm, -1);
    be_printf("[");
    for (i = 0; i < count; ++i) {
        be_pushint(vm, i);
        be_getindex(vm, -2);
        be_printvalue(vm, 1, -1);
        be_pop(vm, 2);
        if (i < count - 1) {
            be_printf(", ");
        }
    }
    be_printf("]");
    return be_nonereturn(vm);
}

static int m_append(bvm *vm)
{
    be_getfield(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_append(vm, -2);
    return be_nonereturn(vm);
}

static int m_item(bvm *vm)
{
    be_getfield(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_getindex(vm, -2);
    return be_return(vm);
}

static int m_setitem(bvm *vm)
{
    be_getfield(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_pushvalue(vm, 3);
    be_setindex(vm, -3);
    return be_nonereturn(vm);
}

static int m_size(bvm *vm)
{
    be_getfield(vm, 1, "__data__");
    be_getsize(vm, -1);
    return be_return(vm);
}

static int m_resize(bvm *vm)
{
    be_getfield(vm, 1, "__data__");
    be_pushvalue(vm, 2);
    be_resize(vm, -2);
    return be_nonereturn(vm);
}

static int m_it(bvm *vm)
{
    be_getfunction(vm);
    be_getupval(vm, -1, 0); /* list data */
    be_getupval(vm, -2, 1); /* list data index */
    be_getindex(vm, -2);
    be_pushint(vm, be_toint(vm, -2) + 1);
    be_setupval(vm, -5, 1);
    be_pop(vm, 1);
    return be_return(vm);
}

static int m_iter(bvm *vm)
{
    be_pushntvclosure(vm, m_it, 0, 2);
    be_getfield(vm, 1, "__data__"); /* list data */
    be_setupval(vm, -2, 0);
    be_pop(vm, 1);
    be_pushint(vm, 0); /* list data index */
    be_setupval(vm, -2, 1);
    be_pop(vm, 1);
    return be_return(vm);
}

const bfieldinfo l_field[] = {
    { "__data__", NULL, 0 },
    { "init", m_init, 1 },
    { "print", m_print, 1 },
    { "append", m_append, 2 },
    { "item", m_item, 2 },
    { "setitem", m_setitem, 3 },
    { "size", m_size, 1 },
    { "resize", m_resize, 2 },
    { "iter", m_iter, 1 },
    { NULL, NULL, 0 }
};

void be_list_init(bvm *vm)
{
    be_regclass(vm, "list", l_field);
}
