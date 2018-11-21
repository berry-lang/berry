#include "be_rangelib.h"

static int m_init(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, "__lower__");
    be_pop(vm, 1);
    be_pushvalue(vm, 3);
    be_setmember(vm, 1, "__upper__");
    return be_nonereturn(vm);
}

static int m_print(bvm *vm)
{
    be_printf("(");
    be_getmember(vm, 1, "__lower__");
    be_printvalue(vm, 0, -1);
    be_printf("..");
    be_pop(vm, 1);
    be_getmember(vm, 1, "__upper__");
    be_printvalue(vm, 0, -1);
    be_printf(")");
    return be_nonereturn(vm);
}

static int m_upper(bvm *vm)
{
    be_getmember(vm, 1, "__upper__");
    return be_return(vm);
}

static int m_lower(bvm *vm)
{
    be_getmember(vm, 1, "__lower__");
    return be_return(vm);
}

static int m_setrange(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, "__lower__");
    be_pop(vm, 1);
    be_pushvalue(vm, 3);
    be_setmember(vm, 1, "__upper__");
    return be_nonereturn(vm);
}

static const bmemberinfo l_member[] = {
    { "__lower__", NULL },
    { "__upper__", NULL },
    { "init", m_init },
    { "print", m_print },
    { "upper", m_upper },
    { "lower", m_lower },
    { "setrange", m_setrange },
    { NULL, NULL }
};

void be_range_init(bvm *vm)
{
    be_regclass(vm, "range", l_member);
}
