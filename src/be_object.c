/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_object.h"
#include "be_exec.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"

#define cast_comobj(o)      gc_cast(o, BE_COMOBJ, bcommomobj_t)

const char* be_vtype2str(bvalue_t *v)
{
    switch(var_primetype(v)) {
    case BE_NIL: return "nil";
    case BE_INT: return "int";
    case BE_REAL: return "real";
    case BE_BOOL: return "bool";
    case BE_CLOSURE: case BE_NTVCLOS: case BE_CTYPE_FUNC:
    case BE_NTVFUNC: return "function";
    case BE_PROTO: return "proto";
    case BE_CLASS: return "class";
    case BE_STRING: return "string";
    case BE_LIST: return "list";
    case BE_MAP: return "map";
    case BE_INSTANCE: return "instance";
    case BE_MODULE: return "module";
    case BE_INDEX: return "var";
    case BE_COMPTR: return "ptr";
    default: return "invalid type";
    }
}

bvalue_t* be_indexof(bvm_t *vm, int idx)
{
    if (idx > 0) { /* absolute index */
        be_assert(vm->reg + idx <= vm->top);
        return vm->reg + idx - 1;
    }
    /* relative index */
    be_assert(vm->top + idx >= vm->reg);
    return vm->top + idx;
}

BERRY_API void be_newcomobj(bvm_t *vm, void *data, bntvfunc destroy)
{
    bcommomobj_t *obj;
    bgcobject_t *gco = be_gcnew(vm, BE_COMOBJ, bcommomobj_t);
    if ((obj = cast_comobj(gco)) != NULL) {
        bvalue_t* top = be_incrtop(vm);
        obj->data = data;
        obj->destroy = destroy;
        var_setobj(top, BE_COMOBJ, obj);
    }
}

void be_commonobj_delete(bvm_t *vm, bgcobject_t *obj)
{
    bcommomobj_t *co = cast_comobj(obj);
    if (co) {
        if (co->destroy && co->data) {
            be_pushntvfunction(vm, co->destroy);
            be_pushcomptr(vm, co->data);
            be_call(vm, 1);
            be_pop(vm, 2);
        }
        be_free(vm, co, sizeof(bcommomobj_t));
    }
}

/* generic destroy method for comobj, just call be_os_free() on the pointer */
int be_commonobj_destroy_generic(bvm_t* vm)
{
    int argc = be_top(vm);
    if (argc > 0) {
        void * obj = be_tocomptr(vm, 1);
        if (obj != NULL) { be_os_free(obj); }
    }
    be_return_nil(vm);
}
