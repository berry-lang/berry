#ifndef __BE_STRLIB_H
#define __BE_STRLIB_H

#include "be_object.h"
#include <stdarg.h>

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2);
int be_strcmp(bstring *s1, bstring *s2);
bstring* be_num2str(bvm *vm, bvalue *v);
void be_val2str(bvm *vm, bvalue *v);
void be_pushvfstr(bvm *vm, const char *format, va_list arg);

#endif
