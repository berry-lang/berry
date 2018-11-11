#include "be_api.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_class.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_var.h"

#define be_api_retvar(vm)       ((vm)->cf->reg - (be_checktype((vm)->cf->reg - 2, VT_NOTMETHOD) ? 2 : 1))

void be_api_regcfunc(bvm *vm, const char *name, bcfunction f, int argc)
{
    bstring *s = be_newstr(vm, name);
    int idx = be_globalvar_find(vm, s);
    if (idx == -1) { /* new function */
        bprimfunc *func;
        bvalue *var;
        idx = be_globalvar_new(vm, s);
        var = globalvar(vm, idx);
        func = be_newprimfunc(vm, f, argc);
        set_type(var, VT_PRIMFUNC);
        var->v.p = func;
    } /* else error */
}

void be_reg_class(bvm *vm, const char *name, bclass *c)
{
    bstring *s = be_newstr(vm, name);
    int idx = be_globalvar_new(vm, s);
    bvalue *var = globalvar(vm, idx);
    set_type(var, VT_CLASS);
    var->v.p = c;
}

bvalue* be_api_getvalue(bvm *vm, int n)
{
    return vm->cf->reg + n;
}

void be_api_retvalue(bvm *vm, bvalue *value)
{
    bvalue *ret = be_api_retvar(vm);
    *ret = *value;
}

void be_api_retnil(bvm *vm)
{
    bvalue *ret = be_api_retvar(vm);
    set_type(ret, VT_NIL);
}

void be_api_retint(bvm *vm, bint i)
{
    bvalue *ret = be_api_retvar(vm);
    set_type(ret, VT_INT);
    ret->v.i = i;
}

void be_api_retreal(bvm *vm, breal r)
{
    bvalue *ret = be_api_retvar(vm);
    set_type(ret, VT_REAL);
    ret->v.r = r;
}

void be_api_retbool(bvm *vm, bbool b)
{
    bvalue *ret = be_api_retvar(vm);
    set_type(ret, VT_BOOL);
    ret->v.b = b;
}

void be_api_retclass(bvm *vm, bclass *c)
{
    bvalue *ret = be_api_retvar(vm);
    set_type(ret, VT_CLASS);
    ret->v.p = c;
}

void be_api_retinstance(bvm *vm, bobject *i)
{
    bvalue *ret = be_api_retvar(vm);
    set_type(ret, VT_INSTANCE);
    ret->v.p = i;
}
