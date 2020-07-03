#include "berry.h"
#include <time.h>

#if defined(_WIN32)
    #include <windows.h>
    #define delay(x) Sleep((x))
#else
    #include <unistd.h>
#endif

#if BE_USE_TIME_MODULE

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
        be_newobject(vm, "map");
        time_insert(vm, "year", t->tm_year + 1900);
        time_insert(vm, "month", t->tm_mon + 1);
        time_insert(vm, "day", t->tm_mday);
        time_insert(vm, "hour", t->tm_hour);
        time_insert(vm, "min", t->tm_min);
        time_insert(vm, "sec", t->tm_sec);
        time_insert(vm, "weekday", t->tm_wday);
        be_pop(vm, 1);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int m_clock(bvm *vm)
{
    be_pushreal(vm, clock() / (breal)CLOCKS_PER_SEC);
    be_return(vm);
}

static int m_sleep(bvm *vm)
{
    if (vm != NULL) {
        double milisecs = be_toreal(vm, 1);
        #if defined(_WIN32)
            unsigned int secs = milisecs * 1000; // 1
        #else 
            unsigned int secs = milisecs * 1000000; // 2
        #endif 
        usleep(secs);
        be_return_nil(vm);
    } else {
        be_return_nil(vm);
    }
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(time) {
    be_native_module_function("time", m_time),
    be_native_module_function("dump", m_dump),
    be_native_module_function("clock", m_clock),
    be_native_module_function("sleep", m_sleep)
};

be_define_native_module(time, NULL);
#else
/* @const_object_info_begin
module time (scope: global, depend: BE_USE_TIME_MODULE) {
    time, func(m_time)
    dump, func(m_dump)
    clock, func(m_clock)
    sleep, func(m_sleep)
}
@const_object_info_end */
#include "../generate/be_fixed_time.h"
#endif

#endif /* BE_USE_TIME_MODULE */
