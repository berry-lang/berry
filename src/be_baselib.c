#include "be_baselib.h"
#include "be_vm.h"
#include "be_api.h"
#include "be_debug.h"
#include "be_class.h"
#include "be_string.h"
#include "be_mem.h"
#include <time.h>

static int l_print(bvm *vm)
{
    bvalue *v = be_api_getvalue(vm, 0);
    bvalue *end = be_api_regtop(vm);

    while (v < end) {
        be_print_value(vm, v++, 0);
        be_printf(" ");
    }
    be_printf("\n");
    return 0;
}

static int l_clock(bvm *vm)
{
    be_api_retreal(vm, clock() / (breal)CLOCKS_PER_SEC);
    return 0;
}

static int l_memcount(bvm *vm)
{
    be_api_retreal(vm, (breal)be_mcount());
    return 0;
}

static int l_super(bvm *vm)
{
    bvalue *v = be_api_getvalue(vm, 0);
    if (be_isclass(v)) {
        bclass *c = v->v.p;
        be_api_retclass(vm, be_class_super(c));
    } else if (be_isinstance(v)) {
        bobject *i = v->v.p;
        be_api_retinstance(vm, be_class_super(i));
    } else {
        be_api_retnil(vm);
    }
    return 0;
}

static int l_type(bvm *vm)
{
    bvalue *v = be_api_getvalue(vm, 0);
    switch(be_type(v)) {
    case VT_NIL:
        be_api_retstr(vm, be_newstr(vm, "nil"));
        break;
    case VT_BOOL:
        be_api_retstr(vm, be_newstr(vm, "bool"));
        break;
    case VT_INT:
        be_api_retstr(vm, be_newstr(vm, "int"));
        break;
    case VT_REAL:
        be_api_retstr(vm, be_newstr(vm, "real"));
        break;
    case VT_CLASS:
        be_api_retstr(vm, be_newstr(vm, "class"));
        break;
    case VT_INSTANCE: {
        bobject *obj = v->v.p;
        be_api_retstr(vm, obj->class->name);
        break;
    }
    default:
        be_api_retnil(vm);
    }
    return 0;
}

void be_loadbaselib(bvm *vm)
{
    be_api_regcfunc(vm, "print", l_print, -1);
    be_api_regcfunc(vm, "clock", l_clock, 0);
    be_api_regcfunc(vm, "super", l_super, 1);
    be_api_regcfunc(vm, "memcount", l_memcount, 0);
    be_api_regcfunc(vm, "type", l_type, 1);
}
