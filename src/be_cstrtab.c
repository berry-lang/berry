#include "be_cstrtab.h"
#include "be_gc.h"

#define be_define_const_str(_name, _s, _hash, _extra, _len, _next) \
const bcstring be_const_str_##_name = { \
    .next = (bgcobject *)_next, \
    .type = BE_STRING, \
    .marked = GC_CONST, \
    .extra = _extra, \
    .slen = _len, \
    .s = _s \
}

#include "../generate/be_const_strtab_def.h"
