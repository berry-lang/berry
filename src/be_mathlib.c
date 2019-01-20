#include "be_object.h"
#include <math.h>
#include <stdlib.h>

#if BE_USE_MATH_LIB

#ifdef M_PI
#undef M_PI
#endif
#define M_PI        3.141592653589793238462643383279

#ifdef M_EXP
#undef M_EXP
#endif
#define M_EXP       2.718281828459045235360287471352

#if BE_SINGLE_FLOAT
#define mathfunc(func)          func##f
#else
#define mathfunc(func)          func
#endif

static int m_abs(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(fabs)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_ceil(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(ceil)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_floor(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(floor)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_sin(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(sin)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_cos(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(cos)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_tan(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(tan)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_asin(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(asin)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_acos(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(acos)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_atan(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(atan)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_sinh(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(sinh)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_cosh(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(cosh)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_tanh(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(tanh)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_sqrt(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(sqrt)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_exp(bvm *vm)
{
    if (be_top(vm) >= 1 && be_isnumber(vm, 1)) {
        breal x = be_toreal(vm, 1);
        be_pushreal(vm, mathfunc(exp)(x));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_pow(bvm *vm)
{
    if (be_top(vm) >= 2 && be_isnumber(vm, 1) && be_isnumber(vm, 2)) {
        breal x = be_toreal(vm, 1);
        breal y = be_toreal(vm, 2);
        be_pushreal(vm, mathfunc(pow)(x, y));
    } else {
        be_pushreal(vm, (breal)0.0);
    }
    return be_return(vm);
}

static int m_random(bvm *vm)
{
    be_pushint(vm, rand());
    return be_return(vm);
}

static bnative_module_obj attr_table[] = {
    be_native_module_function("abs", m_abs),
    be_native_module_function("ceil", m_ceil),
    be_native_module_function("floor", m_floor),
    be_native_module_function("sin", m_sin),
    be_native_module_function("cos", m_cos),
    be_native_module_function("tan", m_tan),
    be_native_module_function("asin", m_asin),
    be_native_module_function("acos", m_acos),
    be_native_module_function("atan", m_atan),
    be_native_module_function("sinh", m_sinh),
    be_native_module_function("cosh", m_cosh),
    be_native_module_function("tanh", m_tanh),
    be_native_module_function("sqrt", m_sqrt),
    be_native_module_function("exp", m_exp),
    be_native_module_function("pow", m_pow),
    be_native_module_function("random", m_random),
    be_native_module_real("pi", (breal)M_PI),
    be_native_module_real("e", (breal)M_PI)
};

be_define_native_module(math, attr_table);

#endif
