#include "be_listlib.h"
#include "be_class.h"
#include "be_api.h"
#include "be_string.h"
#include "be_debug.h"
#include "be_mem.h"
#include "be_list.h"
#include "be_baselib.h"

#define m_data()  be_newstr(vm, "__data__")

static int m_init(bvm *vm)
{
    bvalue data;
    bobject *obj = be_api_getptr(vm, 0);
    be_settype(&data, VT_LIST);
    data.v.p = be_list_new(vm);
    be_object_setmember(obj, m_data(), &data);
    return 0;
}

static int m_print(bvm *vm)
{
    int count, i;
    bvalue data;
    blist *list;
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(), &data);
    list = data.v.p;
    count = be_list_count(list);
    be_printf("[");
    for (i = 0; i < count; ++i) {
        be_print_value(vm, be_list_at(list, i), 1);
        if (i < count - 1) {
            be_printf(", ");
        }
    }
    be_printf("]");
    return 0;
}

static int m_append(bvm *vm)
{
    blist *list;
    bobject *obj = be_api_getptr(vm, 0);
    bvalue data, *value = be_api_getvalue(vm, 1);

    be_object_member(obj, m_data(), &data);
    list = data.v.p;
    be_list_append(list, value);
    return 0;
}

static int m_item(bvm *vm)
{
    bvalue data;
    blist *list;
    int idx = be_api_getint(vm, 1);
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(), &data);
    list = data.v.p;
    if (idx < be_list_count(list)) {
        be_api_retvalue(vm, be_list_at(list, idx));
    } else {
        be_api_retnil(vm);
    }
    return 0;
}

static int m_setitem(bvm *vm)
{
    blist *list;
    int idx = be_api_getint(vm, 1);
    bobject *obj = be_api_getptr(vm, 0);
    bvalue data;

    be_object_member(obj, m_data(), &data);
    list = data.v.p;
    if (idx < be_list_count(list)) {
        bvalue *src = be_api_getvalue(vm, 2);
        bvalue *dst = be_list_at(list, idx);
        *dst = *src;
    }
    return 0;
}

static bclass* newlist(bvm *vm)
{
    bclass *c = be_newclass(vm, be_newstr(vm, "list"), NULL);
    be_member_bind(c, m_data());
    be_prim_method_bind(vm, c, "init", m_init, 1);
    be_prim_method_bind(vm, c, "print", m_print, 1);
    be_prim_method_bind(vm, c, "append", m_append, 2);
    be_prim_method_bind(vm, c, "item", m_item, 2);
    be_prim_method_bind(vm, c, "setitem", m_setitem, 3);
    return c;
}

void be_list_init(bvm *vm)
{
    bclass *c = newlist(vm);
    be_reg_class(vm, "list", c);
}
