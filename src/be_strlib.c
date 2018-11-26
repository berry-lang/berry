#include "be_strlib.h"
#include "be_string.h"
#include <string.h>

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2)
{
    int len = str_len(s1) + str_len(s2);
    bstring *s = be_newstrn(vm, str(s1), len);
    strncat((char*)str(s), str(s2), len);
    return s;
}
