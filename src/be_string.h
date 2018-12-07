#ifndef __BE_STRING_H
#define __BE_STRING_H

#include "be_object.h"
#include "string.h"

#define SHORT_STR_MAX_LEN   64

typedef struct {
    bstring str;
    int llen;
} blstring;

#define str(_s)             ((_s)->s)
#define str_extra(_s)       ((_s)->extra)
#define str_setextra(_s, d) ((_s)->extra = (bbyte)(d))
#define str_len(_s)         ((_s)->slen == 255 ? \
                                cast(blstring*, _s)->llen : (_s)->slen)
#define str_hash(_s)        be_strhash((_s)->s, str_len(_s))

void be_string_init(bvm *vm);
int be_eqstr(bstring *s1, bstring *s2);
uint32_t be_strhash(const char *str, int len);
bstring* be_newstr(bvm *vm, const char *str);
bstring* be_newstrn(bvm *vm, const char *str, int len);
bstring* be_newconststr(bvm *vm, const char *str);
void be_gcstrtab(bvm *vm);

#endif
