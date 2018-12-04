#include "be_strlib.h"
#include "be_string.h"
#include "be_vm.h"
#include "be_class.h"
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

static void sim2str(bvm *vm, bvalue *v)
{
    char sbuf[32];
    switch (var_type(v)) {
    case BE_NIL:
        strcpy(sbuf, "nil");
        break;
    case BE_BOOL:
        strcpy(sbuf, var_tobool(v) ? "true" : "false");
        break;
    case BE_INT:
        sprintf(sbuf, "%d", var_toint(v));
        break;
    case BE_REAL:
        sprintf(sbuf, "%g", var_toreal(v));
        break;
    case BE_STRING:
        return;
    case BE_CLOSURE: case BE_NTVCLOS: case BE_NTVFUNC:
        sprintf(sbuf, "<function: %p>", var_toobj(v));
        break;
    case BE_CLASS:
        sprintf(sbuf, "<class: %s>",
            str(be_class_name(cast(bclass*, var_toobj(v)))));
        break;
    default:
        strcpy(sbuf, "(unknow value)");
        break;
    }
    var_setstr(v, be_newstr(vm, sbuf));
}

static void ins2str(bvm *vm, bvalue *v)
{
    bvalue *top = vm->top++;
    binstance *obj = var_toobj(v);
    /* get method 'tostring' */
    be_instance_member(obj, be_newstr(vm, "tostring"), top);
    if (var_isnil(top)) {
        char sbuf[2 * sizeof(void*) + 16];
        sprintf(sbuf, "<instance: %p>", var_toobj(v));
        --vm->top;
        var_setstr(v, be_newstr(vm, sbuf));
    } else {
        bvalue *ins = vm->top++;
        var_setval(ins, v);
        be_call(vm, 1);
        var_setval(v, top);
        vm->top -= 2;
    }
}

void be_val2str(bvm *vm, bvalue *v)
{
    if (var_isinstance(v)) {
        ins2str(vm, v);
    } else {
        sim2str(vm, v);
    }
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
        case 'p': {
            char buf[2 * sizeof(void*) + 4];
            sprintf(buf, "%p", va_arg(arg, void*));
            pushstr(vm, buf, strlen(buf));
            break;
        }
        default:
            pushstr(vm, "(unknow)", 8);
            break;
        }
        concat2(vm);
        format = p + 2;
    }
    pushstr(vm, format, strlen(format));
    concat2(vm);
}
