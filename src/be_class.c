/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_class.h"
#include "be_string.h"
#include "be_map.h"
#include "be_exec.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_func.h"
#include "be_module.h"
#include <string.h>

#define check_members(vm, c)            \
    if (!(c)->members) {                \
        (c)->members = be_map_new(vm);  \
    }

bclass_t* be_newclass(bvm_t *vm, bstring_t *name, bclass_t *super)
{
    bgcobject_t *gco = be_gcnew(vm, BE_CLASS, bclass_t);
    bclass_t *obj = cast_class(gco);
    bvalue_t *buf = be_incrtop(vm); /* protect new objects from GC */
    var_setclass(buf, obj);
    if (obj) {
        obj->super = super;
        obj->members = NULL; /* gc protection */
        obj->nvar = 0;
        obj->name = name;
    }
    be_stackpop(vm, 1);
    return obj;
}

void be_class_compress(bvm_t *vm, bclass_t *c)
{
    if (!gc_isconst(c) && c->members) {
        be_map_compact(vm, c->members); /* clear space */
    }
}

/* Return the type of the class attribute, only used to check if the attribute already exists */
int be_class_attribute(bvm_t *vm, bclass_t *c, bstring_t *attr)
{
    for (; c; c = c->super) {
        if (c->members) {
            bvalue_t *v = be_map_findstr(vm, c->members, attr);
            if (v) {
                return var_type(v);
            }
        }
    }
    return BE_NONE;
}

void be_class_member_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bbool var)
{
    bvalue_t *attr;
    set_fixed(name);
    check_members(vm, c);
    attr = be_map_insertstr(vm, c->members, name, NULL);
    restore_fixed(name);
    if (var) {
        /* this is an instance variable so we set it as MT_VARIABLE */
        attr->v.i = c->nvar++;
        attr->type = MT_VARIABLE;
    } else {
        /* this is a static class constant, leave it as BE_NIL */
        attr->v.i = 0;
        attr->type = BE_NIL;
    }
}

void be_class_method_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bproto_t *p, bbool is_static)
{
    bclosure_t *cl;
    bvalue_t *attr;
    set_fixed(name);
    check_members(vm, c);
    attr = be_map_insertstr(vm, c->members, name, NULL);
    restore_fixed(name);
    var_setnil(attr);
    cl = be_newclosure(vm, p->nupvals);
    cl->proto = p;
    var_setclosure(attr, cl);
    if (is_static) {
        var_markstatic(attr);
    }
}

void be_class_native_method_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bntvfunc f)
{
    bvalue_t *attr;
    set_fixed(name);
    check_members(vm, c);
    attr = be_map_insertstr(vm, c->members, name, NULL);
    restore_fixed(name);
    attr->v.nf = f;
    attr->type = MT_PRIMMETHOD;
}

void be_class_closure_method_bind(bvm_t *vm, bclass_t *c, bstring_t *name, bclosure_t *cl)
{
    bvalue_t *attr;
    check_members(vm, c);
    attr = be_map_insertstr(vm, c->members, name, NULL);
    attr->v.gc = (bgcobject_t*) cl;
    attr->type = MT_METHOD;
}

/* get the closure method count that need upvalues */
int be_class_closure_count(bclass_t *c)
{
    int count = 0;
    if (c->members) {
        bmapnode_t *node;
        bmap_t *members = c->members;
        bmapiter_t iter = be_map_iter();
        while ((node = be_map_next(members, &iter)) != NULL) {
            if (var_isproto(&node->value)) {
                ++count;
            }
        }
    }
    return count;
}

static binstance_t* instance_member(bvm_t *vm,
    binstance_t *obj, bstring_t *name, bvalue_t *dst)
{
    for (; obj; obj = obj->super) {
        bmap_t *members = obj->_class->members;
        if (members) {
            bvalue_t *v = be_map_findstr(vm, members, name);
            if (v) {
                *dst = *v;
                return obj;
            }
        }
    }
    var_setnil(dst);
    return NULL;
}

static bclass_t* class_member(bvm_t *vm,
    bclass_t *obj, bstring_t *name, bvalue_t *dst)
{
    for (; obj; obj = obj->super) {
        bmap_t *members = obj->members;
        if (members) {
            bvalue_t *v = be_map_findstr(vm, members, name);
            if (v) {
                *dst = *v;
                return obj;
            }
        }
    }
    var_setnil(dst);
    return NULL;
}

void be_class_upvalue_init(bvm_t *vm, bclass_t *c)
{
    bmap_t *mbr = c->members;
    if (mbr != NULL) {
        bmapnode_t *node;
        bmapiter_t iter = be_map_iter();
        while ((node = be_map_next(mbr, &iter)) != NULL) {
            if (var_isclosure(&node->value)) {
                bclosure_t *cl = var_toobj(&node->value);
                if (cl->proto->nupvals) {
                    /* initialize the closure's upvalues */
                    be_release_upvalues(vm, cl);
                    be_initupvals(vm, cl);
                }
            }
        }
    }
}

/* (internal) Instanciate an instance for a single class and initialize variables to nil */
static binstance_t* newobjself(bvm_t *vm, bclass_t *c)
{
    size_t size = sizeof(binstance_t) + sizeof(bvalue_t) * (c->nvar - 1);
    bgcobject_t *gco = be_newgcobj(vm, BE_INSTANCE, size);
    binstance_t *obj = cast_instance(gco);
    be_assert(obj != NULL);
    if (obj) { /* initialize members */
        bvalue_t *v = obj->members, *end = v + c->nvar;  /* instance variables is a simple array of pointers at obj->members of size c->nvar */
        while (v < end) { var_setnil(v); ++v; }  /* Initialize all instance variables to `nil` */
        obj->_class = c;  /* set its class object */
        obj->super = NULL;  /* no super class instance for now */
        obj->sub = NULL;  /* no subclass instance for now */
    }
    return obj;
}

/* (internal) Instanciate the whole chain of instances when there is a class hierarchy */
/* All variables set to nil, constructors are not called here */
static binstance_t* newobject(bvm_t *vm, bclass_t *c)
{
    binstance_t *obj, *prev;
    be_assert(c != NULL);
    obj = prev = newobjself(vm, c);
    var_setinstance(vm->top, obj);
    be_incrtop(vm); /* protect new objects from GC */
    for (c = c->super; c; c = c->super) {  /* initialize one instance object per class and per superclass */
        prev->super = newobjself(vm, c);
        prev->super->sub = prev;  /* link the super/sub classes instances */
        prev = prev->super;
    }
    be_stackpop(vm, 1);
    return obj;
}

/* Instanciate new instance from stack with argc parameters */
/* Pushes the constructor on the stack to be executed if a construtor is found */
/* Returns true if a constructor is found */
bbool be_class_newobj(bvm_t *vm, bclass_t *c, int pos, int argc, int mode)
{
    bvalue_t init;
    binstance_t *obj = newobject(vm, c);  /* create empty object hierarchy from class hierarchy */
    // reg = vm->reg + pos - mode; /* the stack may have changed, mode=1 when class is instanciated from module #104 */
    var_setinstance(vm->reg + pos, obj);
    var_setinstance(vm->reg + pos - mode, obj);  /* copy to reg and reg+1 if mode==1 */
    /* find constructor */
    obj = instance_member(vm, obj, str_literal(vm, "init"), &init);
    if (obj && var_type(&init) != MT_VARIABLE) {
        /* copy argv */
        bvalue_t * reg;
        for (reg = vm->reg + pos + 1; argc > 0; --argc) {
            reg[argc] = reg[argc - 2];
        }
        *reg = init; /* set constructor */
        return btrue;
    }
    return bfalse;
}

/* Find instance member by name and copy value to `dst` */
/* Do not look into virtual members */
int be_instance_member_simple(bvm_t *vm, binstance_t *instance, bstring_t *name, bvalue_t *dst)
{
    int type;
    be_assert(name != NULL);
    binstance_t * obj = instance_member(vm, instance, name, dst);
    if (obj && var_type(dst) == MT_VARIABLE) {
        *dst = obj->members[dst->v.i];
    }
    type = var_type(dst);
    var_clearstatic(dst);
    return type;
}

/* Find instance member by name and copy value to `dst` */
/* Input: none of `obj`, `name` and `dst` may not be NULL */
/* Returns the type of the member or BE_NONE if member not found */
/* TODO need to support synthetic members */
int be_instance_member(bvm_t *vm, binstance_t *instance, bstring_t *name, bvalue_t *dst)
{
    int type;
    be_assert(name != NULL);
    binstance_t *obj = instance_member(vm, instance, name, dst);
    if (obj && var_type(dst) == MT_VARIABLE) {
        *dst = obj->members[dst->v.i];
    }
    type = var_type(dst);
    if (obj) {
        var_clearstatic(dst);
        return type;
    } else {  /* if no method found, try virtual */
        /* if 'init' does not exist, create a virtual empty constructor */
        if (strcmp(str(name), "init") == 0) {
            var_setntvfunc(dst, be_default_init_native_function);
            return var_primetype(dst);
        } else {
            /* get method 'member' */
            obj = instance_member(vm, instance, str_literal(vm, "member"), vm->top);
            if (obj && basetype(var_type(vm->top)) == BE_FUNCTION) {
                bvalue_t *top = vm->top;
                var_setinstance(&top[1], instance);
                var_setstr(&top[2], name);
                vm->top += 3;   /* prevent gc collection results */
                be_dofunc(vm, top, 2); /* call method 'member' */
                vm->top -= 3;
                *dst = *vm->top;   /* copy result to R(A) */
                if (obj && var_type(dst) == MT_VARIABLE) {
                    *dst = obj->members[dst->v.i];
                }
                type = var_type(dst);
                if (type == BE_MODULE) {
                    /* check if the module is named `undefined` */
                    bmodule_t *mod = var_toobj(dst);
                    if (strcmp(be_module_name(mod), "undefined") == 0) {
                        return BE_NONE;     /* if the return value is module `undefined`, consider it is an error */
                    }
                }
                    var_clearstatic(dst);
                    return type;
            }
        }
    }
    return BE_NONE;
}

int be_class_member(bvm_t *vm, bclass_t *obj, bstring_t *name, bvalue_t *dst)
{
    int type;
    be_assert(name != NULL);
    obj = class_member(vm, obj, name, dst);
    type = var_type(dst);
    var_clearstatic(dst);
    return obj ? type : BE_NONE;
}

bbool be_instance_setmember(bvm_t *vm, binstance_t *o, bstring_t *name, bvalue_t *src)
{
    bvalue_t v;
    be_assert(name != NULL);
    binstance_t * obj = instance_member(vm, o, name, &v);
    if (obj && var_istype(&v, MT_VARIABLE)) {
        obj->members[var_toint(&v)] = *src;
        return btrue;
    } else {
        obj = instance_member(vm, o, str_literal(vm, "setmember"), &v);
        if (obj && var_type(&v) == MT_VARIABLE) {
            v = obj->members[v.v.i];
        }
        if (var_basetype(&v) == BE_FUNCTION) {
            bvalue_t *top = vm->top;
            var_setval(top, &v);
            var_setinstance(top + 1, o); /* move instance to argv[0] */
            var_setstr(top + 2, name); /* move method name to argv[1] */
            var_setval(top + 3, src); /* move method name to argv[1] */
            vm->top += 4;   /* prevent collection results */
            be_dofunc(vm, top, 3); /* call method 'member' */
            vm->top -= 4;
            /* if return value is `false` or `undefined` signal an unknown attribute */
            int type = var_type(vm->top);
            if (type == BE_BOOL) {
                bbool ret = var_tobool(vm->top);
                if (!ret) {
                    return bfalse;
                }
            } else if (type == BE_MODULE) {
                /* check if the module is named `undefined` */
                bmodule_t *mod = var_toobj(vm->top);
                if (strcmp(be_module_name(mod), "undefined") == 0) {
                    return bfalse;     /* if the return value is module `undefined`, consider it is an error */
                }
            }
            return btrue;
        }
    }
    return bfalse;
}

bbool be_class_setmember(bvm_t *vm, bclass_t *o, bstring_t *name, bvalue_t *src)
{
    bvalue_t v;
    be_assert(name != NULL);
    if (!gc_isconst(o)) {
        bclass_t * obj = class_member(vm, o, name, &v);
        if (obj && !var_istype(&v, MT_VARIABLE)) {
            be_map_insertstr(vm, obj->members, name, src);
            return btrue;
        }
    }
    return bfalse;
}
