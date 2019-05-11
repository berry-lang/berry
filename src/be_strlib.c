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
        sprintf(buf, BE_INT_FORMAT, var_toint(v));
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
        sprintf(sbuf, BE_INT_FORMAT, var_toint(v));
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
        /* check the return value */
        if (!var_isstr(vm->top)) {
            const char *name = str(be_instance_name(obj));
            be_pusherror(vm, be_pushfstring(vm,
                "`%s::tostring` return value error, the expected type is 'string'",
                strlen(name) ? name : "<anonymous>"));
        }
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
    /* to create a string and then update the top pointer,
     * otherwise the GC may crash due to uninitialized values.
     **/
    bstring *str = be_newstrn(vm, s, len);
    bvalue *reg = be_incrtop(vm);
    var_setstr(reg, str);
}

static const char* concat2(bvm *vm)
{
    bvalue *dst = vm->top - 2;
    bstring *s1 = var_tostr(dst);
    bstring *s2 = var_tostr(dst + 1);
    bstring *s = be_strcat(vm, s1, s2);
    dst = vm->top - 2; /* maybe GC (stack change) */
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
    bint sum = 0;
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
    bint c, vint = be_str2int(str, &sout);
    c = *sout;
    if (c == '.' || c == 'e' || c == 'E') {
        be_pushreal(vm, be_str2real(str, &sout));
    } else {
        be_pushint(vm, vint);
    }
    return sout;
}

bstring* be_strindex(bvm *vm, bstring *str, bvalue *idx)
{
    if (var_isint(idx)) {
        int pos = var_toidx(idx);
        if (pos < str_len(str)) {
            return be_newstrn(vm, str(str) + pos, 1);
        }
        be_pusherror(vm, "string index out of range");
    }
    be_pusherror(vm, "string indices must be integers");
    return NULL;
}

#ifdef BE_USE_STRING_MODULE

#define MAX_FORMAT_MODE     32
#define FLAGES              "+- #0"

static const char* skip2dig(const char *s)
{
    if (is_digit(*s)) {
        ++s;
    }
    if (is_digit(*s)) {
        ++s;
    }
    return s;
}

static const char* get_mode(const char *str, char *buf)
{
    const char *p = str;
    while (*p && strchr(FLAGES, *p)) { /* skip flags */
        ++p;
    }
    p = skip2dig(p); /* skip width (2 digits at most) */
    if (*p == '.') {
        p = skip2dig(++p); /* skip width (2 digits at most) */
    }
    *(buf++) = '%';
    strncpy(buf, str, p - str + 1);
    buf[p - str + 1] = '\0';
    return p;
}

static void mode_fixlen(char *mode, const char *lenmode)
{
    size_t l = strlen(mode), lm = strlen(lenmode);
    char spec = mode[l - 1];
    strcpy(mode + l - 1, lenmode);
    mode[l + lm - 1] = spec;
    mode[l + lm] = '\0';
}

static int str_format(bvm *vm)
{
    int top = be_top(vm);
    if (top > 0 && be_isstring(vm, 1)) {
        int index = 2;
        const char *format = be_tostring(vm, 1);
        pushstr(vm, "", 0);
        for (;;) {
            char mode[MAX_FORMAT_MODE];
            char buf[128];
            const char *p = strchr(format, '%');
            if (p == NULL) {
                break;
            }
            pushstr(vm, format, p - format);
            concat2(vm);
            p = get_mode(p + 1, mode);
            buf[0] = '\0';
            if (index > top) {
                be_pusherror(vm, be_pushfstring(vm,
                    "bad argument #%d to 'format': no value", index));
            }
            switch (*p) {
            case 'd': case 'i': case 'o':
            case 'u': case 'x': case 'X':
                if (be_isint(vm, index)) {
                    mode_fixlen(mode, BE_INT_FMTLEN);
                    sprintf(buf, mode, be_toint(vm, index));
                }
                be_pushstring(vm, buf);
                break;
            case 'e': case 'E':
            case 'f': case 'g': case 'G':
                if (be_isnumber(vm, index)) {
                    sprintf(buf, mode, be_toreal(vm, index));
                }
                be_pushstring(vm, buf);
                break;
            case 's': {
                const char *s = be_tostring(vm, index);
                int len = be_strlen(vm, 2);
                if (len > 100 && strlen(mode) == 2) {
                    be_pushvalue(vm, index);
                } else {
                    sprintf(buf, mode, s);
                    be_pushstring(vm, buf);
                }
                break;
            }
            default: /* error */
                be_pusherror(vm, be_pushfstring(vm,
                    "invalid option '%%%c' to 'format'", *p));
                break;
            }
            concat2(vm);
            format = p + 1;
            ++index;
        }
        pushstr(vm, format, strlen(format));
        concat2(vm);
        be_return(vm);
    }
    be_return_nil(vm);
}

be_native_module_attr_table(str_attr) {
    be_native_module_function("format", str_format)
};

be_define_native_module(string, str_attr);

#endif /* BE_USE_STRING_MODULE */
