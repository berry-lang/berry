#ifndef __BE_CONSTOBJ_H
#define __BE_CONSTOBJ_H

#include "be_object.h"
#include "be_gc.h"
#include "be_map.h"
#include "be_class.h"
#include "be_string.h"

#define be_const_header(_t)  \
    .next = NULL,            \
    .type = (_t),            \
    .marked = GC_CONST

#define be_const_header_map()       be_const_header(BE_MAP)
#define be_const_header_class()     be_const_header(BE_CLASS)

#define be_const_key(_str, _next)   \
{ \
    .v.s = (bstring *)&(_str), \
    .type = BE_STRING, \
    .next = (uint32_t)(_next) & 0xFFFFFF \
}

#define be_const_func(_func) \
{ \
    .v.nf = (_func), \
    .type = BE_FUNCTION \
}

#define be_const_int(_val) \
{ \
    .v.i = (_val), \
    .type = BE_INT \
}

#define be_const_class(_class) \
{ \
    .v.p = (bclass *)&(_class), \
    .type = BE_CLASS \
}

#endif
