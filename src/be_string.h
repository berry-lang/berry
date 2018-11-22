#ifndef __BE_STRING_H
#define __BE_STRING_H

#include "be_object.h"

#define str(str)             ((str)->s)
#define str_extra(str)       ((str)->extra)
#define str_setextra(str, d) ((str)->extra = (bbyte)(d))
#define hash(str)            ((str)->hash)

void be_string_init(bvm *vm);
int be_eqstr(bstring *s1, bstring *s2);
bstring* be_newstr(bvm *vm, const char *str);
bstring* be_newstrn(bvm *vm, const char *str, int len);
bstring* be_newconststr(bvm *vm, const char *str);
void be_deletestrgc(bvm *vm, bstring *str);

#endif
