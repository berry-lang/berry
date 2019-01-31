#ifndef __BE_STRING_H
#define __BE_STRING_H

#include "be_object.h"
#include <string.h>

#define SHORT_STR_MAX_LEN   64

typedef struct {
    bstring_header;
    char s[];
} bsstring;

typedef struct {
    bstring str;
    int llen;
    char s[];
} blstring;

typedef struct {
    bstring_header;
    const char *s;
} bcstring;

#define str_len(_s) \
    ((_s)->slen == 255 ? cast(blstring*, _s)->llen : (_s)->slen)

#define str(_s)             be_str2cstr(_s)
#define str_extra(_s)       ((_s)->extra)
#define str_setextra(_s, d) ((_s)->extra = (bbyte)(d))
#define str_hash(_s)        be_strhash(str(_s), str_len(_s))

void be_string_init(bvm *vm);
void be_string_deleteall(bvm *vm);
int be_eqstr(bstring *s1, bstring *s2);
uint32_t be_strhash(const char *str, size_t len);
bstring* be_newstr(bvm *vm, const char *str);
bstring* be_newstrn(bvm *vm, const char *str, size_t len);
void be_gcstrtab(bvm *vm);
const char* be_str2cstr(bstring *s);

#endif
