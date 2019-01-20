#include "be_strlib.h"
#include "be_string.h"
#include "be_vm.h"
#include "be_class.h"
#include "be_module.h"
#include "be_exec.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define is_space(c)     ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define is_digit(c)     ((c) >= '0' && (c) <= '9')
#define skip_space(s)   while (is_space(*(s))) { ++(s); }

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2)
{
    size_t len = str_len(s1) + str_len(s2);
    if (len <= SHORT_STR_MAX_LEN) {
        char buf[SHORT_STR_MAX_LEN + 1];
        strcpy(buf, str(s1));
        strncat(buf, str(s2), len);
        return be_newstrn(vm, buf, len);
    } else { /* long string */
        bstring *s = be_newstrn(vm, str(s1), len);
        char *sbuf = (char*)str(s);
        strcpy(sbuf + str_len(s1), str(s2));
        return s;
    }
}

int be_strcmp(bstring *s1, bstring *s2)
{
    if (be_eqstr(s1, s2)) {
        return 0;
    }
    return strcmp(str(s1), str(s2));
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
    char sbuf[64];
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
    case BE_MODULE:
        sprintf(sbuf, "<module: %s>",
            be_module_name(cast(bmodule*, var_toobj(v))));
        break;
    default:
        strcpy(sbuf, "(unknow value)");
        break;
    }
    var_setstr(v, be_newstr(vm, sbuf));
}

static void ins2str(bvm *vm, int idx)
{
    bvalue *v = vm->reg + idx;
    bvalue *top = be_incrtop(vm);
    binstance *obj = var_toobj(v);
    /* get method 'tostring' */
    be_instance_member(obj, be_newstr(vm, "tostring"), top);
    if (var_isnil(top)) {
        char sbuf[2 * sizeof(void*) + 16];
        sprintf(sbuf, "<instance: %p>", var_toobj(v));
        --vm->top;
        var_setstr(v, be_newstr(vm, sbuf));
    } else {
        be_incrtop(vm);
        var_setval(top + 1, v);
        be_dofunc(vm, top, 1);
        vm->top -= 2;
        var_setval(vm->reg + idx, vm->top);
    }
}

void be_val2str(bvm *vm, int index)
{
    int absidx = be_absindex(vm, index) - 1;
    bvalue *v = vm->reg + absidx;
    if (var_isinstance(v)) {
        ins2str(vm, absidx);
    } else {
        sim2str(vm, v);
    }
}

static void pushstr(bvm *vm, const char *s, size_t len)
{
    bvalue *reg = be_incrtop(vm);
    bstring *str = be_newstrn(vm, s, len);
    var_setstr(reg, str);
}

static const char* concat2(bvm *vm)
{
    bvalue *dst = vm->top - 2;
    bstring *s1 = var_tostr(dst);
    bstring *s2 = var_tostr(dst + 1);
    bstring *s = be_strcat(vm, s1, s2);
    var_setstr(dst, s);
    --vm->top;
    return str(s);
}

const char* be_pushvfstr(bvm *vm, const char *format, va_list arg)
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
            bvalue *v = be_incrtop(vm);
            var_setint(v, va_arg(arg, int));
            s = be_num2str(vm, v);
            var_setstr(v, s);
            break;
        }
        case 'f': case 'g': {
            bstring *s;
            bvalue *v = be_incrtop(vm);
            var_setreal(v, cast(breal, va_arg(arg, double)));
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
    return concat2(vm);
}

/*******************************************************************
 * the function be_str2int():
 * >>-+------------+--+-----+----digits----><
 *    '-whitespace-'  +- + -+
 *                    '- - -'
 *******************************************************************/
bint be_str2int(const char *str, const char **endstr)
{
    int c, sign;
    int sum = 0;
    skip_space(str);
    sign = c = *str++;
    if (c == '+' || c == '-') {
        c = *str++;
    }
    while (is_digit(c)) {
        sum = sum * 10 + c - '0';
        c = *str++;
    }
    if (endstr) {
        *endstr = str - 1;
    }
    return sign == '-' ? -sum : sum;
}

/*******************************************************************
 * the function be_str2real():
 * >>-+------------+--+-----+--+-digits--+---+--+--------+-+------->
 *    '-whitespace-'  +- + -+  |         '-.-'  '-digits-' |   
 *                    '- - -'  '-.--digits-----------------'   
 * 
 * >--+------------------------+----------------------------------><
 *    '-+-e-+--+-----+--digits-'   
 *      '-E-'  +- + -+             
 *             '- - -'  
 *******************************************************************/
breal be_str2real(const char *str, const char **endstr)
{
    int c, sign;
    breal sum = 0, deci = 0, point = (breal)0.1;
    skip_space(str);
    sign = c = *str++;
    if (c == '+' || c == '-') {
        c = *str++;
    }
    while (is_digit(c)) {
        sum = sum * 10 + c - '0';
        c = *str++;
    }
    if (c == '.') {
        c = *str++;
        while (is_digit(c)) {
            deci = deci + ((breal)c - '0') * point;
            point *= (breal)0.1;
            c = *str++;
        }
    }
    sum = sum + deci;
    if (c == 'e' || c == 'E') {
        int e = 0;
        breal ratio = (c = *str++) == '-' ? (breal)0.1 : 10;
        if (c == '+' || c == '-') {
            c = *str++;
        }
        while (is_digit(c)) {
            e = e * 10 + c - '0';
            c = *str++;
        }
        while (e--) {
            sum *= ratio;
        }
    }
    if (endstr) {
        *endstr = str - 1;
    }
    return sign == '-' ? -sum : sum;
}

/* convert a string to a number (integer or real).
 * 1. skip \s*[\+\-]?\d*
 * 2. matched [.eE]? yes: real, no: integer.
 **/
const char* be_str2num(bvm *vm, const char *str)
{
    const char *sout; 
    int c, vint = be_str2int(str, &sout);
    c = *sout;
    if (c == '.' || c == 'e' || c == 'E') {
        be_pushreal(vm, be_str2real(str, &sout));
    } else {
        be_pushint(vm, vint);
    }
    return sout;
}
