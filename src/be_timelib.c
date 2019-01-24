#include "berry.h"
#include <time.h>

#ifdef BE_USE_TIME_MODULE

int m_time(bvm *vm)
{
    be_pushint(vm, (bint)time(NULL));
    be_return(vm);
}

be_native_module_attr_table(time_attr) {
    be_native_module_function("time", m_time)
};

be_define_native_module(time, time_attr);

#endif /* BE_USE_TIME_MODULE */
