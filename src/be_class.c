#include "be_class.h"
#include "be_string.h"
#include "be_vector.h"
#include "be_map.h"
#include "be_exec.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_var.h"

bclass* be_newclass(bvm *vm, bstring *name, bclass *super)
{
    bgcobject *gco = be_gcnew(vm, BE_CLASS, bclass);
    bclass *obj = cast_class(gco);
    bvalue *buf = be_incrtop(vm); /* protect new objects from GC */
    var_setclass(buf, obj);
    if (obj) {
        obj->super = super;
        obj->members = NULL; /* gc protection */
        obj->nvar = 0;
        obj->name = name;
        obj->members = be_map_new(vm);
    }
    be_stackpop(vm, 1);
    return obj;
}

int be_class_attribute(bclass *c, bstring *attr)
{
    while (c) {
        bvalue *v = be_map_findstr(c->members, attr);
        if (v) {
            return var_type(v);
        }
        c = c->super;
    }
    return BE_NIL;
}

void be_member_bind(bvm *vm, bclass *c, bstring *name)
{
    bmap *map = c->members;
    bvalue *v = be_map_insertstr(vm, map, name, NULL);
    v->v.i = c->nvar++;
    v->type = MT_VARIABLE;
}

void be_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p)
{
    bvalue *m = be_map_insertstr(vm, c->members, name, NULL);
    bclosure *cl = be_newclosure(vm, 0);
    cl->proto = p;
    m->v.p = cl;
    m->type = MT_METHOD;
}

void be_prim_method_bind(bvm *vm, bclass *c, bstring *name, bntvfunc f)
{
    bvalue *m = be_map_insertstr(vm, c->members, name, NULL);
    m->v.nf = f;
    m->type = MT_PRIMMETHOD;
}

binstance* instance_member(binstance *obj, bstring *name, bvalue *dst)
{
    while (obj) {
        bvalue *v = be_map_findstr(obj->class->members, name);
        if (v) {
            *dst = *v;
            return obj;
        }
        obj = obj->super;
    }
    var_setnil(dst);
    return NULL;
}

static binstance* newobjself(bvm *vm, bclass *c)
{
    size_t size = sizeof(binstance) + sizeof(bvalue) * (c->nvar - 1);
    bgcobject *gco = be_newgcobj(vm, BE_INSTANCE, size);
    binstance *obj = cast_instance(gco);
    be_assert(obj != NULL);
    if (obj) { /* initialize members */
        bvalue *v = obj->members, *end = v + c->nvar;
        while (v < end) { var_setnil(v); ++v; }
        obj->class = c;
        obj->super = NULL;
    }
    return obj;
}

static binstance* newobject(bvm *vm, bclass *c)
{
    binstance *obj, *prev;
    bvalue *buf = be_incrtop(vm); /* protect new objects from GC */
    be_assert(c != NULL);
    obj = prev = newobjself(vm, c);
    var_setinstance(buf, obj);
    for (c = c->super; c; c = c->super) {
        prev->super = newobjself(vm, c);
        prev = prev->super;
    }
    be_stackpop(vm, 1);
    return obj;
}

int be_class_newobj(bvm *vm, bclass *c, bvalue *argv, int argc)
{
    bvalue init;
    binstance *obj = newobject(vm, c);
    var_setinstance(argv, obj);
    /* find constructor */
    obj = instance_member(obj, be_newstr(vm, "init"), &init);
    if (obj && var_type(&init) != MT_VARIABLE) {
        /* user constructor */
        bvalue *reg = argv + 1;
        /* copy argv */
        for (; argc > 0; --argc) {
            reg[argc] = argv[argc - 1];
        }
        *reg = init; /* set constructor */
        return 1;
    }
    return 0;
}

int be_instance_member(binstance *obj, bstring *name, bvalue *dst)
{
    int type;
    be_assert(name != NULL);
    obj = instance_member(obj, name, dst);
    type = var_type(dst);
    if (obj && type == MT_VARIABLE) {
        *dst = obj->members[dst->v.i];
    }
    return type;
}

int be_instance_setmember(binstance *obj, bstring *name, bvalue *src)
{
    bvalue v;
    be_assert(name != NULL);
    obj = instance_member(obj, name, &v);
    if (obj && var_istype(&v, MT_VARIABLE)) {
        obj->members[var_toint(&v)] = *src;
        return 1;
    }
    return 0;
}
