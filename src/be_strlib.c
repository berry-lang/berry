#include "be_strlib.h"
#include "be_string.h"
#include "be_vm.h"
#include "be_class.h"
#include "be_module.h"
#include "be_exec.h"
#include "be_mem.h"
#include <string.h>
#include <stdio.h>

#define is_space(c)     ((c) == ' ' || (c) == '\t' || (c) == '\r' || (c) == '\n')
#define is_digit(c)     ((c) >= '0' && (c) <= '9')
#define skip_space(s)   while (is_space(*(s))) { ++(s); }

bstring* be_strcat(bvm *vm, bstring *s1, bstring *s2)
{
    size_t len = (size_t)str_len(s1) + str_len(s2);
    if (len <= SHORT_STR_MAX_LEN) {
        char buf[SHORT_STR_MAX_LEN + 1];
        strcpy(buf, str(s1));
        strncat(buf, str(s2), len);
        return be_newstrn(vm, buf, len);
    } else { /* long string */
        bstring *s = be_newstrn(vm, NULL, len);
        char *sbuf = (char*)str(s);
        strcpy(sbuf, str(s1));
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

static void module2str(char *buf, bvalue *v)
{
    const char *name = be_module_name(cast(bmodule*, var_toobj(v)));
    if (name) {
        sprintf(buf, "<module: %s>", name);
    } else {
        sprintf(buf, "<module: %p>", var_toobj(v));
    }
}

static bstring* sim2str(bvm *vm, bvalue *v)
{
    char sbuf[64]; /* BUG: memory overflow */
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
    case BE_CLOSURE: case BE_NTVCLOS: case BE_NTVFUNC:
        sprintf(sbuf, "<function: %p>", var_toobj(v));
        break;
    case BE_CLASS:
        sprintf(sbuf, "<class: %s>",
            str(be_class_name(cast(bclass*, var_toobj(v)))));
        break;
    case BE_MODULE:
        module2str(sbuf, v);
        break;
    default:
        strcpy(sbuf, "(unknow value)");
        break;
    }
    return be_newstr(vm, sbuf);
}

static bstring* ins2str(bvm *vm, int idx)
{
    bstring *s = be_newstr(vm, "tostring");
    binstance *obj = var_toobj(vm->reg + idx);
    /* get method 'tostring' */
    int type = be_instance_member(vm, obj, s, vm->top);
    be_incrtop(vm); /* push the obj::tostring to stack */
    if (basetype(type) != BE_FUNCTION) {
        bstring *name = be_class_name(be_instance_class(obj));
        char *sbuf = be_malloc(vm, (size_t)str_len(name) + 16);
        sprintf(sbuf, "<instance: %s()>", str(name));
        be_stackpop(vm, 1); /* pop the obj::tostring */
        s = be_newstr(vm, sbuf);
        be_free(vm, sbuf, (size_t)str_len(name) + 16);
    } else {
        *vm->top = vm->reg[idx];
        be_dofunc(vm, vm->top - 1, 1);
        be_stackpop(vm, 1); /* pop the obj::tostring */
        if (!var_isstr(vm->top)) { /* check the return value */
            const char *name = str(be_instance_name(obj));
            be_raise(vm, "runtime_error", be_pushfstring(vm,
                "the value of `%s::tostring()` is not a 'string'",
                strlen(name) ? name : "<anonymous>"));
        }
        s = var_tostr(vm->top);
    }
    return s;
}

void be_val2str(bvm *vm, int index)
{
    bstring *s;
    int idx = be_absindex(vm, index) - 1;
    bvalue *v = vm->reg + idx;
    if (var_isstr(v)) return; /* do nothing */
    s = var_isinstance(v) ? ins2str(vm, idx) : sim2str(vm, v);
    v = vm->reg + idx; /* the stack may change */
    var_setstr(v, s);
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
    be_assert(var_isstr(vm->top - 2) && var_isstr(vm->top - 1));
    dst = vm->top - 2; /* the stack may change */
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
BERRY_API bint be_str2int(const char *str, const char **endstr)
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
BERRY_API breal be_str2real(const char *str, const char **endstr)
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
BERRY_API const char *be_str2num(bvm *vm, const char *str)
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

/* string subscript operation */
bstring* be_strindex(bvm *vm, bstring *str, bvalue *idx)
{
    if (var_isint(idx)) {
        int pos = var_toidx(idx);
        if (pos < str_len(str)) {
            return be_newstrn(vm, str(str) + pos, 1);
        }
        be_raise(vm, "index_error", "string index out of range");
    }
    be_raise(vm, "index_error", "string indices must be integers");
    return NULL;
}

const char* be_splitpath(const char *path)
{
    const char *p;
    for (p = path - 1; *path != '\0'; ++path) {
        if (*path == '/') {
            p = path;
        }
    }
    return p + 1; /* return the file name pointer */
}

const char* be_splitname(const char *path)
{
    const char *p, *q, *end = path + strlen(path);
    for (p = end; *p != '.' && p > path; --p); /* skip [^\.] */
    for (q = p; *q == '.' && q > path; --q); /* skip \. */
    if ((q == path && *q == '.') || *q == '/') {
        return end;
    }
    return p;
}

#if BE_USE_STRING_MODULE

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
                be_raise(vm, "runtime_error", be_pushfstring(vm,
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
            case 'c':
                if (be_isint(vm, index)) {
                    sprintf(buf, "%c", (int)be_toint(vm, index));
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
                be_raise(vm, "runtime_error", be_pushfstring(vm,
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

static int str_split(bvm *vm)
{
    int top = be_top(vm);
    be_newobject(vm, "list");
    if (top >= 2 && be_isstring(vm, 1) && be_isint(vm, 2)) {
        int len = be_strlen(vm, 1), idx = be_toindex(vm, 2);
        const char *s = be_tostring(vm, 1);
        idx = idx > len ? len : idx < -len ? -len : idx;
        if (idx < 0) {
            idx += len;
        }
        be_pushnstring(vm, s, idx);
        be_data_push(vm, -2);
        be_pop(vm, 1);
        be_pushnstring(vm, s + idx, len - idx);
        be_data_push(vm, -2);
        be_pop(vm, 1);
    }
    be_pop(vm, 1);
    be_return(vm);
}

static int str_i2hex(bvm *vm)
{
    int top = be_top(vm);
    if (top && be_isint(vm, 1)) {
        bint value = be_toint(vm, 1);
        char fmt[10] = { "%" BE_INT_FMTLEN "X" }, buf[18];
        if (top >= 2 && be_isint(vm, 2)) {
            bint num = be_toint(vm, 2);
            if (num > 0 && num <= 16) {
                sprintf(fmt, "%%.%d" BE_INT_FMTLEN "X", (int)num);
            }
        }
        sprintf(buf, fmt, value);
        be_pushstring(vm, buf);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int str_byte(bvm *vm)
{
    if (be_top(vm) && be_isstring(vm, 1)) {
        const bbyte *s = (const bbyte *)be_tostring(vm, 1);
        be_pushint(vm, *s);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int str_char(bvm *vm)
{
    if (be_top(vm) && be_isint(vm, 1)) {
        char c = be_toint(vm, 1) & 0xFF;
        be_pushnstring(vm, &c, 1);
        be_return(vm);
    }
    be_return_nil(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(string) {
    be_native_module_function("format", str_format),
    be_native_module_function("split", str_split),
    be_native_module_function("hex", str_i2hex),
    be_native_module_function("byte", str_byte),
    be_native_module_function("char", str_char)
};

be_define_native_module(string, NULL);
#else
/* @const_object_info_begin
module string (scope: global, depend: BE_USE_STRING_MODULE) {
    format, func(str_format)
    split, func(str_split)
    hex, func(str_i2hex)
    byte, func(str_byte)
    char, func(str_char)
}
@const_object_info_end */
#include "../generate/be_fixed_string.h"
#endif

#endif /* BE_USE_STRING_MODULE */
