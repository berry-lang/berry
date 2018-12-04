#include "be_gc.h"
#include "be_vm.h"
#include "be_mem.h"
#include "be_var.h"
#include "be_vector.h"
#include "be_string.h"
#include "be_class.h"
#include "be_list.h"
#include "be_func.h"
#include "be_map.h"
#include "be_debug.h"

struct bgc {
    bgcobject *list;
    bgcobject *gray;
    bgcobject *fixed;
    size_t mcount;
    bbyte steprate;
    bbyte pause;
};

static void mark_object(bvm *vm, bgcobject *obj, int type);

void be_gc_init(bvm *vm)
{
    bgc *gc = be_malloc(sizeof(bgc));
    gc->list = NULL;
    gc->gray = NULL;
    gc->fixed = NULL;
    gc->mcount = be_mcount();
    gc->steprate = 200;
    gc->pause = 0;
    vm->gc = gc;
}

void be_gc_setsteprate(bvm *vm, int rate)
{
    vm->gc->steprate = (bbyte)rate;
}

void be_gc_setpause(bvm *vm, int pause)
{
    vm->gc->pause = (bbyte)pause;
}

bgcobject* be_newgcobj(bvm *vm, int type, int size)
{
    bgc *gc = vm->gc;
    bgcobject *obj = be_malloc(size);

    var_settype(obj, (bbyte)type);
    gc_setdark(obj);
    be_gc_auto(vm);
    obj->next = gc->list; /* insert to head */
    gc->list = obj;
    return obj;
}

bgcobject* be_gc_newstr(bvm *vm, int size, int islong)
{
    bgcobject *obj;
    if (islong) {
        return be_newgcobj(vm, BE_STRING, size);
    }
    obj = be_malloc(size);
    var_settype(obj, BE_STRING);
    gc_setdark(obj);
    return obj;
}

void be_gc_fix(bvm *vm, bgcobject *obj)
{
    (void)vm;
    gc_setfixed(obj);
}

void be_gc_unfix(bvm *vm, bgcobject *obj)
{
    (void)vm;
    gc_clearfixed(obj);
}

static void mark_map(bvm *vm, bgcobject *obj)
{
    bmap *map = cast_map(obj);
    if (map) {
        bmapentry *node;
        bmapiter iter = be_map_iter();
        while ((node = be_map_next(map, &iter)) != NULL) {
            bvalue *key = &node->key;
            bvalue *val = &node->value;
            mark_object(vm, key->v.gc, var_type(key));
            mark_object(vm, val->v.gc, var_type(val));
        }
        gc_setdark(obj);
    }
}

static void mark_list(bvm *vm, bgcobject *obj)
{
    blist *list = cast_list(obj);
    if (list) {
        bvalue *val = be_list_data(list);
        int count = be_list_count(list);
        for (; count--; val++) {
            mark_object(vm, val->v.gc, var_type(val));
        }
        gc_setdark(obj);
    }
}

static void mark_proto(bvm *vm, bgcobject *obj)
{
    bproto *p = cast_proto(obj);
    if (p) {
        int count;
        bvalue *k = p->ktab;
        bproto **ptab = p->ptab;
        for (count = p->nconst; count--; ++k) {
            mark_object(vm, k->v.gc, var_type(k));
        }
        for (count = p->nproto; count--; ++ptab) {
            mark_object(vm, gc_object(*ptab), BE_PROTO);
        }
        gc_setdark(gc_object(p->name));
        gc_setdark(obj);
    }
}

static void mark_closure(bvm *vm, bgcobject *obj)
{
    bclosure *cl = cast_closure(obj);
    if (cl) {
        int count = cl->nupvals;
        bupval **uv = cl->upvals;
        for (; count--; ++uv) {
            if ((*uv)->refcnt) {
                bvalue *v = (*uv)->value;
                mark_object(vm, v->v.gc, var_type(v));
            }
        }
        mark_proto(vm, gc_object(cl->proto));
        gc_setdark(obj);
    }
}

static void mark_ntvclos(bvm *vm, bgcobject *obj)
{
    bntvclos *f = cast_ntvclos(obj);
    if (f) {
        int count = f->nupvals;
        bupval **uv = &be_ntvclos_upval(f, 0);
        for (; count--; ++uv) {
            if ((*uv)->refcnt) {
                bvalue *v = (*uv)->value;
                mark_object(vm, v->v.gc, var_type(v));
            }
        }
        gc_setdark(obj);
    }
}

static void mark_class(bvm *vm, bgcobject *obj)
{
    bclass *c = cast_class(obj);
    if (c) {
        mark_map(vm, gc_object(be_class_members(c)));
        gc_setdark(be_class_name(c));
        gc_setdark(obj);
    }
}

static void mark_instance(bvm *vm, bgcobject *obj)
{
    binstance *o = cast_instance(obj);
    while (o) {
        bvalue *var = be_instance_members(o);
        int nvar = be_instance_member_count(o);
        while (nvar--) {
            mark_object(vm, var->v.gc, var_type(var));
            var++;
        }
        mark_class(vm, gc_object(o->class));
        gc_setdark(o);
        o = be_instance_super(o);
    }
}

static void mark_object(bvm *vm, bgcobject *obj, int type)
{
    if (be_isgctype(type)) {
        switch (type) {
        case BE_STRING: gc_setdark(obj); break;
        case BE_CLASS: mark_class(vm, obj); break;
        case BE_PROTO: mark_proto(vm, obj); break;
        case BE_INSTANCE: mark_instance(vm, obj); break;
        case BE_MAP: mark_map(vm, obj); break;
        case BE_LIST: mark_list(vm, obj); break;
        case BE_CLOSURE: mark_closure(vm, obj); break;
        case BE_NTVCLOS: mark_ntvclos(vm, obj); break;
        default: break;
        }
    }
}

static void free_list(bgcobject *obj)
{
    blist *list = cast_list(obj);
    if (list) {
        be_free(be_list_data(list));
    }
    be_free(obj);
}

static void free_proto(bgcobject *obj)
{
    bproto *proto = cast_proto(obj);
    if (proto) {
        be_free(proto->upvals);
        be_free(proto->ktab);
        be_free(proto->ptab);
        be_free(proto->code);
        be_free(proto);
    }
}

static void free_closure(bgcobject *obj)
{
    bclosure *cl = cast_closure(obj);
    if (cl) {
        int i, count = cl->nupvals;
        for (i = 0; i < count; ++i) {
            bupval *uv = cl->upvals[i];
            if (uv->refcnt) {
                --uv->refcnt;
            }
            /* delete non-referenced closed upvalue */
            if (uv->value == &uv->u.value && !uv->refcnt) {
                be_free(uv);
            }
        }
    }
    be_free(obj);
}

static void free_ntvclos(bgcobject *obj)
{
    bntvclos *f = cast_ntvclos(obj);
    if (f) {
        int count = f->nupvals;
        bupval **uv = &be_ntvclos_upval(f, 0);
        while (count--) {
            be_free(*uv++);
        }
    }
}

static void free_object(bvm *vm, bgcobject *obj)
{
    (void)vm;
    switch (obj->type) {
    case BE_INSTANCE: be_free(obj); break;
    case BE_MAP: be_map_delete(cast_map(obj)); break;
    case BE_LIST: free_list(obj); break;
    case BE_CLOSURE: free_closure(obj); break;
    case BE_NTVCLOS: free_ntvclos(obj); break;
    case BE_PROTO: free_proto(obj); break;
    default: break; /* case BE_STRING: break; */
    }
}

static void premark_global(bvm *vm)
{
    bvalue *v = vm->global;
    bvalue *end = v + vm->gbldesc.nglobal;
    while (v < end) {
        if (be_isgcobj(v)) {
            gc_setgray(var_togc(v));
        }
        ++v;
    }
}

static void premark_stack(bvm *vm)
{
    bvalue *v = vm->stack, *end = vm->top;
    /* mark live objects */
    while (v < end) {
        if (be_isgcobj(v)) {
            gc_setgray(var_togc(v));
        }
        ++v;
    }
    /* set other values to nil */
    end = vm->stack + vm->stacksize;
    while (v < end) {
        var_setnil(v);
        ++v;
    }
}

static void premark_fixed(bvm *vm)
{
    bgcobject *node = vm->gc->list;
    for (; node; node = node->next) {
        if (gc_isfixed(node) && gc_iswhite(node)) {
            gc_setgray(node);
        }
    }
}

static void mark_unscanned(bvm *vm)
{
    bgc *gc = vm->gc;
    bgcobject *node;
    for (node = gc->list; node; node = node->next) {
        if (gc_isgray(node)) {
            mark_object(vm, node, var_type(node));
        }
    }
}

static void delete_white(bvm *vm)
{
    bgc *gc = vm->gc;
    bgcobject *node, *prev, *next;
    for (node = gc->list, prev = node; node; node = next) {
        next = node->next;
        if (gc_iswhite(node)) {
            if (node == gc->list) { /* first node */
                gc->list = node->next;
                prev = node->next;
            } else { /* not first node */
                prev->next = next;
            }
            free_object(vm, node);
        } else {
            gc_setwhite(node);
            prev = node;
        }
    }
}

static void clear_graylist(bvm *vm)
{
    bgc *gc = vm->gc;
    bgcobject *node;
    for (node = gc->fixed; node; node = node->next) {
        if (gc_isdark(node)) {
            gc_setgray(node);
        }
    }
}

void be_gc_auto(bvm *vm)
{
    bgc *gc = vm->gc;
    size_t mcount = be_mcount();
    if (gc->pause && mcount > gc->mcount * gc->steprate / 100) {
        be_gc_collect(vm);
    }
}

void be_gc_collect(bvm *vm)
{
    /* step 1: set root-set reference object to unscanned */
    premark_global(vm); /* global objects */
    premark_stack(vm); /* stack objects */
    premark_fixed(vm);
    /* step 2: set unscanned object to black */
    mark_unscanned(vm);
    /* step 3: delete unreachable object */
    delete_white(vm);
    clear_graylist(vm);
    be_gcstrtab(vm);
    vm->gc->mcount = be_mcount();
}
