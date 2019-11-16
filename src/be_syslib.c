#include "be_object.h"

#if BE_USE_SYS_MODULE

static int m_path(bvm *vm)
{
    be_getbuiltin(vm, "list");
    be_module_path(vm);
    be_call(vm, 1);
    be_pop(vm, 1);
    be_return(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(sys){
    be_native_module_function("path", m_path),
};

be_define_native_module(sys, NULL);
#else
/* @const_object_info_begin
module sys (scope: global, depend: BE_USE_SYS_MODULE) {
    path, func(m_path)
}
@const_object_info_end */
#include "../generate/be_fixed_sys.h"
#endif

#endif /* BE_USE_SYS_MODULE */
