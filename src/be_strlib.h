#ifndef __BE_STRLIB_H
#define __BE_STRLIB_H

#include "be_object.h"
#include <stdarg.h>

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2);
void be_pushvfstr(bvm *vm, const char *format, va_list arg);

#endif
