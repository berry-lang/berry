#include "be_api.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_class.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_var.h"

void be_api_regcfunc(bvm *vm, const char *name, bcfunction f, int argc)
{
    bstring *s = be_newstr(vm, name);
    int idx = be_globalvar_find(vm, s);
    if (idx == -1) { /* new function */
        bntvfunc *func;
        bvalue *var;
        idx = be_globalvar_new(vm, s);
        var = globalvar(vm, idx);
        func = be_newntvfunc(vm, f, argc);
        set_type(var, VT_NTVFUNC);
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

bvalue* be_api_retreg(bvm *vm)
{
    int notmet = var_istype(vm->cf->reg - 2, VT_NOTMETHOD);
    return vm->cf->reg - (notmet ? 2 : 1);
}

void be_api_retvalue(bvm *vm, bvalue *value)
{
    bvalue *ret = be_api_retreg(vm);
    if (value) {
        *ret = *value;
    } else {
        var_setnil(ret);
    }
}
