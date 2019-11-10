#ifndef BE_STRLIB_H
#define BE_STRLIB_H

#include "be_object.h"
#include <stdarg.h>

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2);
int be_strcmp(bstring *s1, bstring *s2);
bstring* be_num2str(bvm *vm, bvalue *v);
void be_val2str(bvm *vm, int index);
const char* be_splitpath(const char *path);
const char* be_splitname(const char *str);
const char* be_pushvfstr(bvm *vm, const char *format, va_list arg);
bstring* be_strindex(bvm *vm, bstring *str, bvalue *idx);

#endif
