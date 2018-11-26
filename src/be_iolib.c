#include "be_iolib.h"
#include <stdio.h>

static int l_input(bvm *vm)
{
    char str[128];
    fgets(str, sizeof(str), stdin);
    be_pushstring(vm, str);
    return be_return(vm);
}

void be_loadiolib(bvm *vm)
{
    be_regcfunc(vm, "input", l_input);
}
