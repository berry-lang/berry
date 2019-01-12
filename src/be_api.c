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
#include "be_exec.h"
#include "be_strlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define retreg(vm)      ((vm)->cf->func)

static bvalue* index2value(bvm *vm, int idx)
{
    if (idx > 0) { /* argument */
        return vm->reg + idx - 1;
    }
    return vm->top + idx;
}

static void class_init(bvm *vm, bclass *c, const bmemberinfo *lib)
{
    while (lib->name) {
        bstring *s = be_newconststr(vm, lib->name);
        if (lib->function) { /* method */
            be_prim_method_bind(c, s, lib->function);
        } else {
            be_member_bind(c, s); /* member */
        }
        ++lib;
    }
}

void be_regcfunc(bvm *vm, const char *name, bcfunction f)
{
    bstring *s = be_newconststr(vm, name);
    int idx = be_globalvar_find(vm, s);
    if (idx == -1) { /* new function */
        bvalue *var;
        idx = be_globalvar_new(vm, s);
        var = be_globalvar(vm, idx);
        var_setntvfunc(var, (void*)f);
    } /* else error */
}

void be_regclass(bvm *vm, const char *name, const bmemberinfo *lib)
{
    bstring *s = be_newconststr(vm, name); /* immediate reference must be made, prevent GC. */
    int idx = be_globalvar_new(vm, s);   /* because relloc is possible, index must first figure out. */
    bclass *c = be_newclass(vm, s, NULL);
    bvalue *var = be_globalvar(vm, idx); /* attention evaluation order. */
    var_setclass(var, c);
    class_init(vm, c, lib); /* bind members */
}

int be_top(bvm *vm)
{
    return cast_int(vm->top - vm->reg);
}

int be_type(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_type(v);
}

void be_pop(bvm *vm, int n)
{
    be_stackpop(vm, n);
}

int be_absindex(bvm *vm, int index)
{
    if (index > 0) {
        return index;
    }
    return cast_int(vm->top + index - vm->reg + 1);
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

int be_isnumber(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isnumber(v);
}

int be_isstring(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isstr(v);
}

int be_isclosure(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isclosure(v);
}

int be_isntvclos(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_isntvclos(v);
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

int be_iscomptr(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_istype(v, BE_COMPTR);
}

int be_toint(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_toint(v);
}

breal be_toreal(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (var_isreal(v)) {
        return var_toreal(v);
    }
    if (var_isint(v)) {
        return (breal)var_toint(v);
    }
    return (breal)0.0;
}

int be_tobool(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_tobool(v) != bfalse;
}

const char* be_tostring(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (!var_isstr(v)) {
        be_val2str(vm, index);
        v = index2value(vm, index);
    }
    return str(var_tostr(v));
}

void* be_tocomptr(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    return var_toobj(v);
}

void be_moveto(bvm *vm, int from, int to)
{
    bvalue *src = index2value(vm, from);
    bvalue *dst = index2value(vm, to);
    var_setval(dst, src);
}

void be_pushnil(bvm *vm)
{
    bvalue *reg = be_incrtop(vm);
    var_setnil(reg);
}

void be_pushbool(bvm *vm, int b)
{
    bvalue *reg = be_incrtop(vm);
    var_setbool(reg, b != 0);
}

void be_pushint(bvm *vm, bint i)
{
    bvalue *reg = be_incrtop(vm);
    var_setint(reg, i);
}

void be_pushreal(bvm *vm, breal r)
{
    bvalue *reg = be_incrtop(vm);
    var_setreal(reg, r);
}

void be_pushstring(bvm *vm, const char *str)
{
    bvalue *reg = be_incrtop(vm);
    bstring *s = be_newstr(vm, str);
    be_assert(reg < vm->stacktop);
    var_setstr(reg, s);
}

void be_pushnstring(bvm *vm, const char *str, size_t n)
{
    bvalue *reg = be_incrtop(vm);
    bstring *s = be_newstrn(vm, str, n);
    var_setstr(reg, s);
}

const char* be_pushfstring(bvm *vm, const char *format, ...)
{
    const char* s;
    va_list arg_ptr;
    va_start(arg_ptr, format);
    s = be_pushvfstr(vm, format, arg_ptr);
    va_end(arg_ptr);
    return s;
}

void be_pushvalue(bvm *vm, int index)
{
    bvalue *reg = vm->top;
    var_setval(reg, index2value(vm, index));
    be_incrtop(vm);
}

void be_pushntvclosure(bvm *vm, bcfunction f, int nupvals)
{
    bvalue *top = be_incrtop(vm);
    bntvclos *cl = be_newprimclosure(vm, f, nupvals);
    var_setntvclos(top, cl);
}

void be_pushntvfunction(bvm *vm, bcfunction f)
{
    bvalue *top = be_incrtop(vm);
    var_setntvfunc(top, (void*)f);
}

void be_pushclass(bvm *vm, const char *name, const bmemberinfo *lib)
{
    bstring *s;
    bclass *c;
    bvalue *dst = be_incrtop(vm);
    s = be_newstr(vm, name);
    var_setstr(dst, s);
    c = be_newclass(vm, s, NULL);
    var_setclass(dst, c);
    class_init(vm, c, lib); /* bind members */
}

void be_pushcomptr(bvm *vm, void *ptr)
{
    bvalue *top = be_incrtop(vm);
    var_setobj(top, BE_COMPTR, ptr);
}

void be_removeone(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *top = --vm->top;
    while (v < top) {
        *v = v[1];
        ++v;
    }
}

void be_strconcat(bvm *vm, int index)
{
    bvalue *dst = index2value(vm, index);
    bvalue *src = index2value(vm, -1);
    if (var_isstr(src) && var_isstr(dst)) {
        bstring *s = be_strcat(vm, var_tostr(dst), var_tostr(src));
        var_setstr(dst, s);
    } else {
        var_setnil(dst);
        be_writestring("strconcat error: object not string.\n");
    }
}

void be_getsuper(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *top = be_incrtop(vm);

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
    return be_vtype2str(v);
}

const char* be_classname(bvm *vm, int index)
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

int be_strlen(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (var_isstr(v)) {
        return str_len(var_tostr(v));
    }
    return 0;
}

void be_newlist(bvm *vm)
{
    bvalue *top = be_incrtop(vm);
    var_setobj(top, BE_LIST, be_list_new(vm));
}

void be_newmap(bvm *vm)
{
    bvalue *top = be_incrtop(vm);
    var_setobj(top, BE_MAP, be_map_new(vm));
}

void be_getglobal(bvm *vm, const char *name)
{
    bvalue *top = be_incrtop(vm);
    int idx = be_globalvar_find(vm, be_newstr(vm, name));
    bvalue *gbl = be_globalvar(vm, idx);
    *top = *gbl;
}

void be_setmember(bvm *vm, int index, const char *k)
{
    bvalue *o = index2value(vm, index);
    if (var_isinstance(o)) {
        bvalue *v = index2value(vm, -1);
        binstance *obj = var_toobj(o);
        be_instance_setmember(obj, be_newstr(vm, k), v);
    }
}

void be_getmember(bvm *vm, int index, const char *k)
{
    bvalue *o = index2value(vm, index);
    bvalue *top = be_incrtop(vm);
    if (var_isinstance(o)) {
        binstance *obj = var_toobj(o);
        be_instance_member(obj, be_newstr(vm, k), top);
    } else {
        var_setnil(top);
    }
}

void be_getindex(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *k = index2value(vm, -1);
    bvalue *dst = be_incrtop(vm);
    switch (var_type(o)) {
    case BE_LIST:
        if (var_isint(k)) {
            blist *list = cast(blist*, var_toobj(o));
            int idx = var_toint(k);
            bvalue *src = be_list_index(list, idx);
            if (src) {
                var_setval(dst, src);
                return;
            }
        }
        break;
    case BE_MAP:
        if (!var_isnil(k)) {
            bmap *map = cast(bmap*, var_toobj(o));
            bvalue *src = be_map_find(map, k);
            if (src) {
                var_setval(dst, src);
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
            blist *list = cast(blist*, var_toobj(o));
            int idx = var_toint(k);
            if (idx < be_list_count(list)) {
                bvalue *dst = be_list_at(list, idx);
                var_setval(dst, v);
            }
        }
        break;
    case BE_MAP:
        if (!var_isnil(k)) {
            bmap *map = cast(bmap*, var_toobj(o));
            bvalue *dst = be_map_find(map, k);
            if (dst) {
                var_setval(dst, v);
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
    bvalue *top = be_incrtop(vm);
    if (var_istype(f, BE_NTVCLOS)) {
        bntvclos *nf = var_toobj(f);
        bvalue *uv = be_ntvclos_upval(nf, pos)->value;
        var_setval(top, uv);
    } else {
        var_setnil(top);
    }
}

void be_setupval(bvm *vm, int index, int pos)
{
    bvalue *f = index2value(vm, index);
    bvalue *v = index2value(vm, -1);
    if (var_istype(f, BE_NTVCLOS)) {
        bntvclos *nf = var_toobj(f);
        bvalue *uv = be_ntvclos_upval(nf, pos)->value;
        var_setval(uv, v);
    }
}

void be_getfunction(bvm *vm)
{
    bvalue *v = retreg(vm);
    bvalue *top = be_incrtop(vm);
    if (var_istype(v, BE_NTVCLOS)) {
        var_setval(top, v);
    } else {
        var_setnil(top);
    }
}

void be_getsize(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    bvalue *dst = be_incrtop(vm);
    if (var_islist(v)) {
        blist *list = cast(blist*, var_toobj(v));
        var_setint(dst, be_list_count(list));
    } else if (var_ismap(v)) {
        bmap *map = cast(bmap*, var_toobj(v));
        var_setint(dst, be_map_count(map));
    } else {
        var_setnil(dst);
    }
}

int be_size(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (var_islist(v)) {
        blist *list = var_toobj(v);
        return be_list_count(list);
    } else if (var_ismap(v)) {
        bmap *map = cast(bmap*, var_toobj(v));
        return be_map_count(map);
    }
    return -1;
}

void be_append(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *v = index2value(vm, -1);
    if (var_islist(o)) {
        blist *list = var_toobj(o);
        be_list_append(list, v);
    }
}

int be_insert(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *k = index2value(vm, -2);
    bvalue *v = index2value(vm, -1);
    switch (var_type(o)) {
    case BE_MAP:
        if (!var_isnil(k)) {
            bmap *map = cast(bmap*, var_toobj(o));
            return be_map_insert(map, k, v) != NULL;
        }
        break;
    case BE_LIST:
        if (var_isint(k)) {
            blist *list = cast(blist*, var_toobj(o));
            return be_list_insert(list, var_toint(k), v) != NULL;
        }
        break;
    default:
        break;
    }
    return bfalse;
}

int be_remove(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *k = index2value(vm, -1);
    switch (var_type(o)) {
    case BE_MAP:
        if (!var_isnil(k)) {
            bmap *map = cast(bmap*, var_toobj(o));
            return be_map_remove(map, k);
        }
        break;
    case BE_LIST:
        if (!var_isint(k)) {
            blist *list = cast(blist*, var_toobj(o));
            return be_list_remove(list, var_toint(k));
        }
        break;
    default:
        break;
    }
    return bfalse;
}

void be_resize(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    bvalue *v = index2value(vm, -1);
    if (var_islist(o)) {
        blist *list = var_toobj(o);
        if (var_isint(v)) {
            be_list_resize(list, var_toint(v));
        }
    }
}

int be_pushiter(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    if (var_ismap(v)) {
        bvalue *iter = be_incrtop(vm);
        var_setobj(iter, BE_COMPTR, NULL);
        return 1;
    } else if (var_islist(v)) {
        blist *list = var_toobj(v);
        bvalue *iter = be_incrtop(vm);
        var_setobj(iter, BE_COMPTR, be_list_data(list) - 1);
        return 1;
    }
    return 0;
}

static int list_next(bvm *vm)
{
    bvalue *iter = index2value(vm, -1);
    bvalue *next, *dst = be_incrtop(vm);
    next = cast(bvalue*, var_toobj(iter)) + 1;
    var_setobj(iter, BE_COMPTR, next);
    var_setval(dst, next);
    return 1;
}

static int list_hasnext(bvm *vm, bvalue *v)
{
    bvalue *next;
    bvalue *iter = index2value(vm, -1);
    blist *obj = var_toobj(v);
    next = cast(bvalue*, var_toobj(iter)) + 1;
    return next >= be_list_data(obj) && next < be_list_end(obj);
}

static int map_next(bvm *vm, bvalue *v)
{
    bmapiter iter;
    bmapnode *entry;
    bvalue *dst = vm->top;
    bvalue *itvar = index2value(vm, -1);
    iter = var_toobj(itvar);
    entry = be_map_next(var_toobj(v), &iter);
    var_setobj(itvar, BE_COMPTR, iter);
    if (entry) {
        bvalue vk = be_map_key2value(entry);
        var_setval(dst, &vk);
        var_setval(dst + 1, &entry->value);
        vm->top += 2;
        return 2;
    }
    return 0;
}

static int map_hasnext(bvm *vm, bvalue *v)
{
    bvalue *node = index2value(vm, -1);
    bmapiter iter = var_toobj(node);
    return be_map_next(var_toobj(v), &iter) != NULL;
}

int be_next(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    if (var_islist(o)) {
        return list_next(vm);
    } else if (var_ismap(o)) {
        return map_next(vm, o);
    }
    return 0;
}

int be_hasnext(bvm *vm, int index)
{
    bvalue *o = index2value(vm, index);
    if (var_islist(o)) {
        return list_hasnext(vm, o);
    } else if (var_ismap(o)) {
        return map_hasnext(vm, o);
    }
    return 0;
}

int be_refcontains(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    binstance **ref = be_stack_base(&vm->refstack);
    binstance **top = be_stack_top(&vm->refstack);
    binstance *ins = var_toobj(v);
    be_assert(var_isinstance(v));
    while (ref <= top && *ref != ins) {
        ++ref;
    }
    return ref <= top;
}

void be_refpush(bvm *vm, int index)
{
    bvalue *v = index2value(vm, index);
    binstance *ins = var_toobj(v);
    be_assert(var_isinstance(v));
    be_stack_push(&vm->refstack, &ins);
}

void be_refpop(bvm *vm)
{
    be_stack_pop(&vm->refstack);
    if (be_stack_isempty(&vm->refstack)) {
        be_vector_release(&vm->refstack);
    }
}

int be_return(bvm *vm)
{
    bvalue *src = vm->top - 1;
    bvalue *ret = retreg(vm);
    var_setval(ret, src);
    return 0;
}

int be_returnnil(bvm *vm)
{
    bvalue *ret = retreg(vm);
    var_setnil(ret);
    return 0;
}

void be_call(bvm *vm, int argc)
{
    bvalue *f = vm->top - argc - 1;
    be_dofunc(vm, f, argc);
}

int be_pcall(bvm *vm, int argc)
{
    bvalue *f = vm->top - argc - 1;
    int res = be_protectedcall(vm, f, argc);
    return res;
}

int be_loadbuffer(bvm *vm,
    const char *name, const char *buffer, size_t length)
{
    int res = be_protectedparser(vm, name, buffer, length);
#if 0
    if (!res) {
        be_writestring("bytecode:\n");
        be_dprintcode(var_toobj(vm->top - 1));
        be_writestring("bytecode end.\n");
    }
#endif
    return res;
}
