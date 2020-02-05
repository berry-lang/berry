#include "be_object.h"
#include "be_module.h"
#include "be_string.h"
#include "be_class.h"
#include "be_debug.h"
#include "be_map.h"
#include "be_vm.h"
#include <string.h>

#if BE_USE_DEBUG_MODULE

static void dump_map(bmap *map)
{
    bmapnode *node;
    bmapiter iter = be_map_iter();
    while ((node = be_map_next(map, &iter)) != NULL) {
        if (var_isstr(&node->key)) {
            bstring *s = var_tostr(&node->key);
            be_writestring("\t");
            be_writebuffer(str(s), str_len(s));
            be_writestring(": <");
            be_writestring(be_vtype2str(&node->value));
            be_writestring(">\n");
        }
    }
}

static void dump_module(bmodule *module)
{
    dump_map(module->table);
}

static void dump_class(bclass *class)
{
    dump_map(class->members);
}

static void dump_instanse(binstance *ins)
{
    dump_class(ins->class);
}

static void dump_value(bvalue *value)
{
    be_writestring("value type <");
    be_writestring(be_vtype2str(value));
    be_writestring(">, attributes:\n");
}

static int m_attrdump(bvm *vm)
{
    if (be_top(vm) >= 1) {
        bvalue *v = be_indexof(vm, 1);
        void *obj = var_toobj(v);
        dump_value(v);
        switch (var_type(v)) {
        case BE_MODULE: dump_module(obj); break;
        case BE_CLASS: dump_class(obj); break;
        case BE_INSTANCE: dump_instanse(obj); break;
        default: break;
        }
    }
    be_return_nil(vm);
}

static int m_codedump(bvm *vm)
{
    if (be_top(vm) >= 1) {
        bvalue *v = be_indexof(vm, 1);
        if (var_isclosure(v)) {
            be_dumpclosure(var_toobj(v));
        }
    }
    be_return_nil(vm);
}

static int m_traceback(bvm *vm)
{
    be_tracestack(vm);
    be_return_nil(vm);
}

static int m_top(bvm *vm)
{
    bint top = vm->top - vm->stack + 1;
    be_pushint(vm, top);
    be_return(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(debug) {
    be_native_module_function("attrdump", m_attrdump),
    be_native_module_function("codedump", m_codedump),
    be_native_module_function("traceback", m_traceback),
    be_native_module_function("top", m_top)
};

be_define_native_module(debug, NULL);
#else
/* @const_object_info_begin
module debug (scope: global, depend: BE_USE_DEBUG_MODULE) {
    attrdump, func(m_attrdump)
    codedump, func(m_codedump)
    traceback, func(m_traceback)
    top, func(m_top)
}
@const_object_info_end */
#include "../generate/be_fixed_debug.h"
#endif

#endif /* BE_USE_DEBUG_MODULE */
