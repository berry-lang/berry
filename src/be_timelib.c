#include "berry.h"
#include <time.h>

#ifdef BE_USE_TIME_MODULE

static int m_time(bvm *vm)
{
    be_pushint(vm, (bint)time(NULL));
    be_return(vm);
}

static void time_insert(bvm *vm, const char *key, int value)
{
    be_pushstring(vm, key);
    be_pushint(vm, value);
    be_data_insert(vm, -3);
    be_pop(vm, 2);
}

static int m_dump(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isint(vm, 1)) {
        time_t ts = be_toint(vm, 1);
        struct tm *t = localtime(&ts);
        be_newmap(vm);
        time_insert(vm, "year", t->tm_year + 1900);
        time_insert(vm, "month", t->tm_mon + 1);
        time_insert(vm, "day", t->tm_mday);
        time_insert(vm, "hour", t->tm_hour);
        time_insert(vm, "min", t->tm_min);
        time_insert(vm, "sec", t->tm_sec);
        time_insert(vm, "weekday", t->tm_wday);
        be_getbuiltin(vm, "map");
        be_pushvalue(vm, -2);
        be_call(vm, 1);
        be_pop(vm, 1);
        be_return(vm);
    }
    be_return_nil(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(time_attr) {
    be_native_module_function("time", m_time),
    be_native_module_function("dump", m_dump)
};

be_define_native_module(time, time_attr);
#else
/* @const_object_info_begin
module time (scope: global) {
    time, func(m_time)
    dump, func(m_dump)
}
@const_object_info_end */
#include "../generate/be_fixed_time.h"
#endif

#endif /* BE_USE_TIME_MODULE */
