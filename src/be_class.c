#include "be_class.h"
#include "be_vector.h"
#include "be_map.h"
#include "be_string.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_var.h"

bclass* be_newclass(bvm *vm, bstring *name, bclass *super)
{
    bgcobject *gco = be_gcnew(vm, VT_CLASS, bclass);
    bclass *obj = cast_class(gco);
    if (obj) {
        obj->super = super;
        obj->members = be_map_new(vm);
        obj->nvar = 0;
        obj->name = name;
        be_gc_fix(vm, gc_object(obj->members));
        be_gc_fix(vm, gc_object(obj->name));
        be_gc_fix(vm, gc_object(obj));
    }
    return obj;
}

void be_member_bind(bclass *c, bstring *name)
{
    bmap *map = c->members;
    bvalue *v = be_map_insertstr(map, name, NULL);
    v->v.i = c->nvar++;
    v->type = MT_VARIABLE;
}

void be_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p)
{
    bvalue *m = be_map_insertstr(c->members, name, NULL);
    bclosure *cl = be_newclosure(vm, 0);
    cl->proto = p;
    m->v.p = cl;
    m->type = MT_METHOD;
}

void be_prim_method_bind(bvm *vm, bclass *c, const char *name, bcfunction f, int argc)
{
    bvalue *m = be_map_insertstr(c->members, be_newstr(vm, name), NULL);
    m->v.p = be_newntvfunc(vm, f, argc);
    m->type = MT_PRIMMETHOD;
}

bvalue* be_class_member(bclass *c, bstring *name)
{
    if (name) {
        return be_map_findstr(c->members, name);
    }
    return NULL;
}

static bobject* newobject(bvm *vm, bclass *c)
{
    if (c) {
        size_t size = sizeof(bobject) + sizeof(bvalue) * (c->nvar - 1);
        bgcobject *gco = be_newgcobj(vm, VT_INSTANCE, size);
        bobject *obj = cast_object(gco);
        if (obj) {
            obj->class = c;
            obj->super = newobject(vm, c->super);
            return obj;
        }
    }
    return NULL;
}

int be_class_newobj(bvm *vm, bclass *c, bvalue *argv, int argc)
{
    bobject *obj = newobject(vm, c);
    bmap *map = obj->class->members;
    bvalue *init = be_map_findstr(map, be_newstr(vm, "init"));

    var_setinstance(argv, obj);
    if (init && init->type != MT_VARIABLE) { /* user constructor */
        bvalue *reg = argv + 1;
        /* copy argv */
        for (; argc > 0; --argc) {
            reg[argc] = argv[argc - 1];
        }
        *reg = *init; /* set constructor */
        return 1;
    }
    return 0;
}

bvalue* be_object_member(bobject *obj, bstring *name, bvalue *dst)
{
    while (obj) {
        bvalue *m = be_class_member(obj->class, name);
        if (m) {
            if (m->type == MT_VARIABLE) {
                *dst = obj->members[m->v.i];
            } else { /* method */
                *dst = *m;
            }
            return m;
        }
        obj = obj->super;
    }
    set_type(dst, VT_NIL);
    return NULL;
}

int be_object_setmember(bobject *obj, bstring *name, bvalue *src)
{
    while (obj) {
        bvalue *m = be_class_member(obj->class, name);
        if (m) {
            if (m->type == MT_VARIABLE) {
                obj->members[m->v.i] = *src;
                return 1;
            }
            return 0;
        }
        obj = obj->super;
    }
    return 0;
}
