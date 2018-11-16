#include "be_api.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_class.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_var.h"
#include "be_list.h"
#include "be_map.h"
#include "be_parser.h"
#include "be_debug.h"

#define getbase(vm)     ((vm)->cf->reg)
#define gettop(vm)      ((vm)->cf->s.ur.top)
#define pushtop(vm)     (gettop(vm)++)

static bvalue* index2value(bvm *vm, int idx)
{
    if (idx > 0) { /* argument */
        return getbase(vm) + idx - 1;
    }
    return gettop(vm) + idx;
}

static bvalue* retreg(bvm *vm)
{
    int notmet = var_istype(vm->cf->reg - 2, BE_NOTMETHOD);
    return vm->cf->reg - (notmet ? 2 : 1);
}

void be_regcfunc(bvm *vm, const char *name, bcfunction f, int argc)
{
    bstring *s = be_newstr(vm, name);
    int idx = be_globalvar_find(vm, s);
    if (idx == -1) { /* new function */
        bntvfunc *func;
        bvalue *var;
        idx = be_globalvar_new(vm, s);
        var = be_globalvar(vm, idx);
        func = be_newntvfunc(vm, f, argc);
        var_setntvfunc(var, func);
    } /* else error */
}

void be_regclass(bvm *vm, const char *name, const bfieldinfo *lib)
{
    bstring *s = be_newstr(vm, name);
    bclass *c = be_newclass(vm, s, NULL);
    bvalue *var = be_globalvar(vm, be_globalvar_new(vm, s));
    var_setclass(var, c);
    /* bind fields */
    while (lib->name) {
        s = be_newstr(vm, lib->name);
        if (lib->function) { /* method */
            be_prim_method_bind(vm, c, s, lib->function, lib->argc);
        } else {
            be_field_bind(c, s); /* member */
        }
        ++lib;
    }
}

int be_top(bvm *vm)
{
    return gettop(vm) - getbase(vm);
}

int be_type(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_type(v);
}

void be_pop(bvm *vm, int n)
{
    gettop(vm) -= n;
}

int be_isnil(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isnil(v);
}

int be_isbool(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isbool(v);
}

int be_isint(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isint(v);
}

int be_isreal(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isreal(v);
}

int be_isstring(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isstring(v);
}

int be_isclosure(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isclosure(v);
}

int be_isntvfunc(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isntvfunc(v);
}

int be_isfunction(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isfunction(v);
}

int be_isproto(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isproto(v);
}

int be_isclass(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isclass(v);
}

int be_isinstance(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isinstance(v);
}

int be_islist(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_islist(v);
}

int be_ismap(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_ismap(v);
}

int be_toint(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_toint(v);
}

breal be_toreal(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_toreal(v);
}

bbool be_tobool(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_tobool(v);
}

const char* be_tostring(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return str(var_tostr(v));
}

void be_pushnil(bvm *vm)
{
    bvalue *reg = pushtop(vm);
    var_setnil(reg);
}

void be_pushbool(bvm *vm, int b)
{
    bvalue *reg = pushtop(vm);
    var_setbool(reg, b != 0);
}

void be_pushint(bvm *vm, bint i)
{
    bvalue *reg = pushtop(vm);
    var_setint(reg, i);
}

void be_pushreal(bvm *vm, breal r)
{
    bvalue *reg = pushtop(vm);
    var_setreal(reg, r);
}

void be_pushstring(bvm *vm, const char *str)
{
    bvalue *reg = pushtop(vm);
    bstring *s = be_newstr(vm, str);
    var_setstr(reg, s);
}

void be_pushvalue(bvm *vm, int index)
{
    bvalue *reg = pushtop(vm);
    var_setval(reg, index2value(vm, index));
}

void be_pushntvclosure(bvm *vm, bcfunction f, int argc, int nupvals)
{
    bvalue *top = pushtop(vm);
    bntvfunc *nf = be_newprimclosure(vm, f, argc, nupvals);
    var_setntvfunc(top, nf);
}

void be_getsuper(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *top = pushtop(vm);

    if (var_isclass(v)) {
        bclass *c = var_toobj(v);
        c = be_class_super(c);
        if (c) {
            var_setclass(top, c);
            return;
        }
    } else if (var_isinstance(v)) {
        binstance *o = var_toobj(v);
        o = be_instance_super(o);
        if (o) {
            var_setinstance(top, o);
            return;
        }
    }
    var_setnil(top);
}

const char* be_typename(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    switch(var_type(v)) {
    case BE_NIL: return "nil";
    case BE_INT: return "int";
    case BE_REAL: return "real";
    case BE_BOOL: return "bool";
    case BE_CLOSURE: return "closure";
    case BE_NTVFUNC: return "ntvfunc";
    case BE_PROTO: return "proto";
    case BE_CLASS: return "class";
    case BE_STRING: return "string";
    case BE_LIST: return "list";
    case BE_MAP: return "map";
    case BE_INSTANCE: return "instance";
    default: return "invalid type";
    }
}

const char* be_objecttype(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (var_isclass(v)) {
        bclass *c = var_toobj(v);
        return str(be_class_name(c));
    }
    if (var_isinstance(v)) {
        binstance *i = var_toobj(v);
        return str(be_instance_name(i));
    }
    return NULL;
}

void be_newlist(bvm *vm)
{
    bvalue *top = pushtop(vm);
    var_setobj(top, BE_LIST, be_list_new(vm));
}

void be_setfield(bvm *vm, int index, const char *k)
{
    bvalue *o = index2value(vm, index);
    if (var_isinstance(o)) {
        bvalue *v = index2value(vm, -1);
        binstance *obj = var_toobj(o);
        be_instance_setfield(obj, be_newstr(vm, k), v);
    }
}

void be_getfield(bvm *vm, int index, const char *k)
{
    bvalue *o = index2value(vm, index);
    if (var_isinstance(o)) {
        bvalue *top = pushtop(vm);
        binstance *obj = var_toobj(o);
        be_instance_field(obj, be_newstr(vm, k), top);
    }
}

void be_getindex(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *k = index2value(vm, -1);
    bvalue *dst = pushtop(vm);
    switch (var_type(o)) {
    case BE_LIST:
        if (var_isint(k)) {
            blist *list = var_toobj(o);
            int idx = var_toint(k);
            if (idx < be_list_count(list)) {
                var_setval(dst, be_list_at(list, idx));
                return;
            }
        }
        break;
    default:
        break;
    }
    var_setnil(dst);
}

void be_setindex(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *k = index2value(vm, -2);
    bvalue *v = index2value(vm, -1);
    switch (var_type(o)) {
    case BE_LIST:
        if (var_isint(k)) {
            blist *list = var_toobj(o);
            int idx = var_toint(k);
            if (idx < be_list_count(list)) {
                bvalue *dst = be_list_at(list, idx);
                *dst = *v;
            }
        }
        break;
    default:
        break;
    }
}

void be_getupval(bvm *vm, int index, int pos)
{
    bvalue *f = index2value(vm, index);
    bvalue *top = pushtop(vm);
    if (var_istype(f, BE_NTVFUNC)) {
        bntvfunc *nf = var_toobj(f);
        bvalue *uv = be_ntvfunc_upval(nf, pos)->value;
        var_setval(top, uv);
    } else {
        var_setnil(top);
    }
}

void be_setupval(bvm *vm, int index, int pos)
{
    bvalue *f = index2value(vm, index);
    bvalue *v = index2value(vm, -1);
    if (var_istype(f, BE_NTVFUNC)) {
        bntvfunc *nf = var_toobj(f);
        bvalue *uv = be_ntvfunc_upval(nf, pos)->value;
        var_setval(uv, v);
    }
}

void be_getfunction(bvm *vm)
{
    bvalue *v = retreg(vm);
    bvalue *top = pushtop(vm);
    if (var_istype(v, BE_NTVFUNC)) {
        var_setval(top, v);
    } else {
        var_setnil(top);
    }
}

void be_getsize(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *dst = pushtop(vm);
    if (var_istype(v, BE_LIST)) {
        blist *list = var_toobj(v);
        var_setint(dst, be_list_count(list));
    } else {
        var_setnil(dst);
    }
}

int be_size(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (var_istype(v, BE_LIST)) {
        blist *list = var_toobj(v);
        return be_list_count(list);
    }
    return -1;
}

void be_append(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *v = index2value(vm, -1);
    if (var_istype(o, BE_LIST)) {
        blist *list = var_toobj(o);
        be_list_append(list, v);
    }
}

void be_resize(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *v = index2value(vm, -1);
    if (var_istype(o, BE_LIST)) {
        blist *list = var_toobj(o);
        if (var_isint(v)) {
            be_list_resize(list, var_toint(v));
        }
    }
}

int be_returnvalue(bvm *vm)
{
    bvalue *v = gettop(vm) - 1;
    bvalue *ret = retreg(vm);
    *ret = *v;
    return 0;
}

void be_call(bvm *vm, int argc)
{
    bvalue *f = gettop(vm) - argc - 1;
    be_pop(vm, argc + 1);
    be_dofuncvar(vm, f, argc);
}

static void print_instance(bvm *vm, int index)
{
    be_getfield(vm, index, "print"); /* get method 'print' */
    if (be_isnil(vm, -1)) {
        be_pop(vm, 1);
        be_printf("print error: object without 'print' method.");
    } else {
        be_pushvalue(vm, index);
        be_call(vm, 1);
    }
}

void be_printvalue(bvm *vm, int quote, int index)
{
    bvalue *v = index2value(vm, index);
    switch (v->type) {
    case BE_NIL:
        be_printf("nil");
        break;
    case BE_BOOL:
        be_printf("%s", var_tobool(v) ? "true" : "false");
        break;
    case BE_INT:
        be_printf("%d", var_toint(v));
        break;
    case BE_REAL:
        be_printf("%g", var_toreal(v));
        break;
    case BE_STRING:
        be_printf(quote ? "\"%s\"" : "%s", str(var_tostr(v)));
        break;
    case BE_CLOSURE:
        be_printf("%p", var_toobj(v));
        break;
    case BE_INSTANCE:
        print_instance(vm, index);
        break;
    default:
        be_printf("Unknow type: %d", var_type(v));
        break;
    }
}

void be_dostring(bvm *vm, const char *src)
{
    bclosure *cl = be_parser_source(vm, "", src);
    be_dofunc(vm, cl, 1);
}
