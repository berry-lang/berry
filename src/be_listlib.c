#include "be_listlib.h"
#include "be_class.h"
#include "be_api.h"
#include "be_string.h"
#include "be_debug.h"
#include "be_mem.h"
#include "be_list.h"
#include "be_baselib.h"
#include "be_func.h"
#include "be_vm.h"

#define m_data(vm)      be_newstr(vm, "__data__")

static bvalue* list_getitem(bvm *vm, bobject *obj, int index)
{
    bvalue data;
    blist *list;
    be_object_member(obj, m_data(vm), &data);
    list = data.v.p;
    if (index < be_list_count(list)) {
        return be_list_at(list, index);
    }
    return NULL;
}

static int m_init(bvm *vm)
{
    bvalue data;
    bobject *obj = be_api_getptr(vm, 0);
    be_settype(&data, VT_LIST);
    data.v.p = be_list_new(vm);
    be_object_setmember(obj, m_data(vm), &data);
    return 0;
}

static int m_print(bvm *vm)
{
    int count, i;
    bvalue data;
    blist *list;
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(vm), &data);
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

    be_object_member(obj, m_data(vm), &data);
    list = data.v.p;
    be_list_append(list, value);
    return 0;
}

static int m_item(bvm *vm)
{
    int idx = be_api_getint(vm, 1);
    bobject *obj = be_api_getptr(vm, 0);
    be_api_retvalue(vm, list_getitem(vm, obj, idx));
    return 0;
}

static int m_setitem(bvm *vm)
{
    bvalue data;
    blist *list;
    int idx = be_api_getint(vm, 1);
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(vm), &data);
    list = data.v.p;
    if (idx < be_list_count(list)) {
        bvalue *src = be_api_getvalue(vm, 2);
        bvalue *dst = be_list_at(list, idx);
        *dst = *src;
    }
    return 0;
}

static int m_size(bvm *vm)
{
    bvalue data;
    blist *list;
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(vm), &data);
    list = data.v.p;
    be_api_retint(vm, be_list_count(list));
    return 0;
}

static int m_resize(bvm *vm)
{
    blist *list;
    bvalue data, *v, *end;
    bvalue *fill = be_api_getvalue(vm, 2);
    int lastsize, size = be_api_getint(vm, 1);
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(vm), &data);
    list = data.v.p;
    lastsize = be_list_count(list);
    be_list_resize(list, size);
    end = be_list_end(list);
    for (v = be_list_at(list, lastsize); v < end; ++v) {
        *v = *fill;
    }
    return 0;
}

static int m_it(bvm *vm)
{
    int idx;
    bupval *uv;
    bobject *obj;
    bntvfunc *f = vm->cf->s.uf.f;

    uv = be_ntvfunc_upval(f, 0);
    obj = var_getobj(uv->value);
    uv = be_ntvfunc_upval(f, 1);
    idx = var_getint(uv->value);
    var_setint(uv->value, idx + 1);
    be_api_retvalue(vm, list_getitem(vm, obj, idx));
    return 0;
}

static int m_iter(bvm *vm)
{
    bvalue data;
    bntvfunc *f;
    bupval *uv;
    bobject *obj = be_api_getptr(vm, 0);

    be_object_member(obj, m_data(vm), &data);
    f = be_newprimclosure(vm, m_it, 0, 2);
    uv = be_ntvfunc_upval(f, 0);
    var_setinstance(uv->value, obj);
    uv = be_ntvfunc_upval(f, 1);
    var_setint(uv->value, 0);
    be_api_retfunc(vm, f);
    return 0;
}

static bclass* newlist(bvm *vm)
{
    bclass *c = be_newclass(vm, be_newstr(vm, "list"), NULL);
    be_member_bind(c, m_data(vm));
    be_prim_method_bind(vm, c, "init", m_init, 1);
    be_prim_method_bind(vm, c, "print", m_print, 1);
    be_prim_method_bind(vm, c, "append", m_append, 2);
    be_prim_method_bind(vm, c, "item", m_item, 2);
    be_prim_method_bind(vm, c, "setitem", m_setitem, 3);
    be_prim_method_bind(vm, c, "size", m_size, 1);
    be_prim_method_bind(vm, c, "resize", m_resize, 3);
    be_prim_method_bind(vm, c, "iter", m_iter, 1);
    return c;
}

void be_list_init(bvm *vm)
{
    bclass *c = newlist(vm);
    be_reg_class(vm, "list", c);
}
