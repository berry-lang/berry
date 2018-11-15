#include "be_api.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_class.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_var.h"
#include "be_debug.h"

#define getbase(vm)     ((vm)->cf->reg)
#define gettop(vm)      ((vm)->cf->s.ur.top)
#define pushtop(vm)     (gettop(vm)++)

static bvalue* index2value(bvm *vm, int idx)
{
    if (idx > 0) { /* argument */
        return getbase(vm) + idx - 1;
    }
    return gettop(vm) + idx;
}

void be_regcfunc(bvm *vm, const char *name, bcfunction f, int argc)
{
    bstring *s = be_newstr(vm, name);
    int idx = be_globalvar_find(vm, s);
    if (idx == -1) { /* new function */
        bntvfunc *func;
        bvalue *var;
        idx = be_globalvar_new(vm, s);
        var = globalvar(vm, idx);
        func = be_newntvfunc(vm, f, argc);
        set_type(var, BE_NTVFUNC);
        var->v.p = func;
    } /* else error */
}

void be_regclass(bvm *vm, const char *name, bclass *c)
{
    bstring *s = be_newstr(vm, name);
    int idx = be_globalvar_new(vm, s);
    bvalue *var = globalvar(vm, idx);
    set_type(var, BE_CLASS);
    var->v.p = c;
}

bvalue* be_getvalue(bvm *vm, int n)
{
    return vm->cf->reg + n;
}

bvalue* be_retreg(bvm *vm)
{
    int notmet = var_istype(vm->cf->reg - 2, BE_NOTMETHOD);
    return vm->cf->reg - (notmet ? 2 : 1);
}

void be_retvalue(bvm *vm, bvalue *value)
{
    bvalue *ret = be_retreg(vm);
    if (value) {
        *ret = *value;
    } else {
        var_setnil(ret);
    }
}

int be_top(bvm *vm)
{
    return gettop(vm) - getbase(vm);
}

int be_type(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_type(v);
}

void be_pushnil(bvm *vm)
{
    bvalue *reg = pushtop(vm);
    var_setnil(reg);
}

void be_pushbool(bvm *vm, int b)
{
    bvalue *reg = pushtop(vm);
    var_setbool(reg, b != 0);
}

void be_pushint(bvm *vm, bint i)
{
    bvalue *reg = pushtop(vm);
    var_setint(reg, i);
}

void be_pushreal(bvm *vm, breal r)
{
    bvalue *reg = pushtop(vm);
    var_setreal(reg, r);
}

void be_pushstring(bvm *vm, const char *str)
{
    bvalue *reg = pushtop(vm);
    bstring *s = be_newstr(vm, str);
    var_setstr(reg, s);
}

void be_pushvalue(bvm *vm, int index)
{
    bvalue *reg = pushtop(vm);
    var_setval(reg, index2value(vm, index));
}

void be_getsuper(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *top = pushtop(vm);

    if (var_isclass(v)) {
        bclass *c = var_getobj(v);
        var_setclass(top, be_class_super(c));
    } else if (var_isinstance(v)) {
        binstance *o = var_getobj(v);
        var_setinstance(top, be_instance_super(o));
    } else {
        var_setnil(top);
    }
}

void be_getobjtype(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *top = pushtop(vm);

    if (var_isclass(v)) {
        bclass *c = var_getobj(v);
        var_setstr(top, be_class_name(c));
    } else if (var_isinstance(v)) {
        binstance *o = var_getobj(v);
        var_setstr(top, be_instance_name(o));
    } else {
        var_setnil(top);
    }
}

int be_returnvalue(bvm *vm)
{
    bvalue *v = gettop(vm) - 1;
    bvalue *ret = be_retreg(vm);
    *ret = *v;
    return 0;
}

void be_printvalue(bvm *vm, int quote, int index)
{
    bvalue *v = index2value(vm, index);
    be_print_value(vm, v, quote);
}
