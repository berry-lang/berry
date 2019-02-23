#include "be_constobj.h"
#include "be_var.h"
#include "be_vm.h"

void be_const_regclass(bvm *vm, const bclass *c)
{
    int idx = be_globalvar_new(vm, c->name);
    bvalue *var = be_globalvar(vm, idx);
    var_setclass(var, (bclass *)c);
}
