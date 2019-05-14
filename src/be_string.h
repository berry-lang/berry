#ifndef BE_STRING_H
#define BE_STRING_H

#include "be_object.h"
#include <string.h>

#define SHORT_STR_MAX_LEN   64

typedef struct {
    bstring_header;
#if BE_STR_HASH_CACHE
    uint32_t hash;
#endif
    /* char s[]; */
} bsstring;

typedef struct {
    bstring str;
    int llen;
    /* char s[]; */
} blstring;

typedef struct {
    bstring_header;
    uint32_t hash;
    const char *s;
} bcstring;

#define str_len(_s) \
    ((_s)->slen == 255 ? cast(blstring*, _s)->llen : (_s)->slen)

#define str(_s)             be_str2cstr(_s)
#define str_extra(_s)       ((_s)->extra)

#if BE_USE_PRECOMPILED_OBJECT
#include "../generate/be_const_strtab.h"
#endif

void be_string_init(bvm *vm);
void be_string_deleteall(bvm *vm);
int be_eqstr(bstring *s1, bstring *s2);
uint32_t str_hash(const char *str, size_t len);
bstring* be_newstr(bvm *vm, const char *str);
bstring* be_newstrn(bvm *vm, const char *str, size_t len);
void be_gcstrtab(bvm *vm);
uint32_t be_strhash(bstring *s);
const char* be_str2cstr(bstring *s);
void be_str_setextra(bstring *s, int extra);

#endif
