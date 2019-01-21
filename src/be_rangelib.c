#include "be_object.h"

static int m_init(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, "__lower__");
    be_pop(vm, 1);
    be_pushvalue(vm, 3);
    be_setmember(vm, 1, "__upper__");
    be_return_nil(vm);
}

static int m_tostring(bvm *vm)
{
    be_pushstring(vm, "(");
    be_getmember(vm, 1, "__lower__");
    be_tostring(vm, -1);
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    be_pushstring(vm, "..");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    be_getmember(vm, 1, "__upper__");
    be_tostring(vm, -1);
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    be_pushstring(vm, ")");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
    be_return(vm);
}

static int m_upper(bvm *vm)
{
    be_getmember(vm, 1, "__upper__");
    be_return(vm);
}

static int m_lower(bvm *vm)
{
    be_getmember(vm, 1, "__lower__");
    be_return(vm);
}

static int m_setrange(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, "__lower__");
    be_pop(vm, 1);
    be_pushvalue(vm, 3);
    be_setmember(vm, 1, "__upper__");
    be_return_nil(vm);
}

static int i_init(bvm *vm)
{
    be_pushvalue(vm, 2);
    be_setmember(vm, 1, ".obj");
    be_return_nil(vm);
}

static int i_hashnext(bvm *vm)
{
    be_getmember(vm, 1, ".obj");
    be_getmember(vm, 1, ".iter");
    be_getmember(vm, -2, "__upper__");
    if (!be_isint(vm, -2) || be_toint(vm, -2) < be_toint(vm, -1)) {
        be_pushbool(vm, btrue);
    } else {
        be_pushbool(vm, bfalse);
    }
    be_return(vm);
}

static int i_next(bvm *vm)
{
    be_getmember(vm, 1, ".iter");
    if (!be_isint(vm, -1)) {
        be_getmember(vm, 1, ".obj");
        be_getmember(vm, -1, "__lower__");
        be_setmember(vm, 1, ".iter");
    } else {
        be_getmember(vm, 1, ".iter");
        be_pushint(vm, be_toint(vm, -1) + 1);
        be_setmember(vm, 1, ".iter");
    }
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

void be_load_rangelib(bvm *vm)
{
    static const bnfuncinfo members[] = {
        { "__lower__", NULL },
        { "__upper__", NULL },
        { "init", m_init },
        { "tostring", m_tostring },
        { "upper", m_upper },
        { "lower", m_lower },
        { "setrange", m_setrange },
        { "iter", m_iter },
        { NULL, NULL }
    };
    be_regclass(vm, "range", members);
}
