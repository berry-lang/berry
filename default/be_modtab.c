/* this file contains the declaration of the module table. */

#include "berry.h"

/* default modules declare */
be_extern_native_module(string);
be_extern_native_module(json);
be_extern_native_module(math);
be_extern_native_module(time);
be_extern_native_module(os);

/* user-defined modules declare start */

/* user-defined modules declare end */

/* module list declaration */
bntvmodule* const be_module_table[] = {
    /* default modules register */
#if BE_USE_STRING_MODULE
    &be_native_module(string),
#endif
#if BE_USE_JSON_MODULE
    &be_native_module(json),
#endif
#if BE_USE_MATH_MODULE
    &be_native_module(math),
#endif
#if BE_USE_TIME_MODULE
    &be_native_module(time),
#endif
#if BE_USE_OS_MODULE
    &be_native_module(os),
#endif
    /* user-defined modules register start */

    /* user-defined modules register end */
    NULL /* do not remove */
};
