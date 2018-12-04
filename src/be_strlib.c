#include "be_strlib.h"
#include "be_string.h"
#include "be_vm.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2)
{
    int len = str_len(s1) + str_len(s2);
    bstring *s = be_newstrn(vm, str(s1), len);
    strncat((char*)str(s), str(s2), len);
    return s;
}

bstring* be_num2str(bvm *vm, bvalue *v)
{
    char buf[25];
    if (var_isint(v)) {
        sprintf(buf, "%d", var_toint(v));
    } else if (var_isreal(v)) {
        sprintf(buf, "%g", var_toreal(v));
    } else {
        sprintf(buf, "(nan)");
    }
    return be_newstr(vm, buf);
}

static void pushstr(bvm *vm, const char *s, int len)
{
    bvalue *reg = vm->top++;
    bstring *str = be_newstrn(vm, s, len);
    var_setstr(reg, str);
}

static void concat2(bvm *vm)
{
    bvalue *dst = vm->top - 2;
    bstring *s1 = var_tostr(dst);
    bstring *s2 = var_tostr(dst + 1);
    bstring *s = be_strcat(vm, s1, s2);
    var_setstr(dst, s);
    --vm->top;
}

void be_pushvfstr(bvm *vm, const char *format, va_list arg)
{
    pushstr(vm, "", 0);
    for (;;) {
        const char *p = strchr(format, '%');
        if (p == NULL) {
            break;
        }
        pushstr(vm, format, p - format);
        concat2(vm);
        switch (p[1]) {
        case 's': {
            const char *s = va_arg(arg, char*);
            if (s == NULL) {
                s = "(null)";
            }
            pushstr(vm, s, strlen(s));
            break;
        }
        case 'd': {
            bstring *s;
            bvalue *v = vm->top++;
            var_setint(v, va_arg(arg, int));
            s = be_num2str(vm, v);
            var_setstr(v, s);
            break;
        }
        case 'c': {
            char c = cast(char, va_arg(arg, int));
            pushstr(vm, &c, 1);
            break;
        }
        case '%': {
            pushstr(vm, "%", 1);
            break;
        }
        default:
            break;
        }
        concat2(vm);
        format = p + 2;
    }
    pushstr(vm, format, strlen(format));
    concat2(vm);
}
