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
#include "be_module.h"
#include "be_exec.h"
#include "be_debug.h"

#define next_threshold(gc)  ((gc).usage * ((gc).steprate + 100) / 100)

#define GC_PAUSE    (1 << 0) /* GC will not be executed automatically */
#define GC_HALT     (2 << 0) /* GC completely stopped */

static void mark_object(bvm *vm, bgcobject *obj, int type);
static void destruct_object(bvm *vm, bgcobject *obj);
static void free_object(bvm *vm, bgcobject *obj);

void be_gc_init(bvm *vm)
{
    vm->gc.list = NULL;
    vm->gc.gray = NULL;
    vm->gc.fixed = NULL;
    vm->gc.usage = sizeof(bvm);
    vm->gc.status = 0;
    be_gc_setsteprate(vm, 200);
}

void be_gc_deleteall(bvm *vm)
{
    bgcobject *node, *next;
    /* first: call destructor */
    for (node = vm->gc.list; node; node = node->next) {
        destruct_object(vm, node);
    }
    /* second: free objects */
    for (node = vm->gc.list; node; node = next) {
        next = node->next;
        free_object(vm, node);
    }
    /* vm->gc will be used afterwards, so it is not free here. */
}

void be_gc_setsteprate(bvm *vm, int rate)
{
    be_assert(rate >= 100 && rate <= 355);
    vm->gc.steprate = (bbyte)(rate - 100);
    vm->gc.threshold = next_threshold(vm->gc);
}

void be_gc_setpause(bvm *vm, int pause)
{
    if (pause) {
        vm->gc.status |= GC_PAUSE;
    } else {
        vm->gc.status &= ~GC_PAUSE;
    }
}

static void* _realloc(void *ptr, size_t old_size, size_t new_size)
{
    if (old_size == new_size) { /* the block unchanged */
        return ptr;
    }
    if (ptr && new_size) { /* realloc block */
        return be_realloc(ptr, new_size);
    }
    if (new_size) { /* alloc a new block */
        be_assert(ptr == NULL && old_size == 0);
        return be_malloc(new_size);
    }
    be_assert(new_size == 0);
    be_free(ptr);
    return NULL;
}

void* be_gc_realloc(bvm *vm, void *ptr, size_t old_size, size_t new_size)
{
    void *block = _realloc(ptr, old_size, new_size);
    if (!block && new_size) { /* allocation failure */
        be_gc_collect(vm); /* try to allocate again after GC */
        block = _realloc(ptr, old_size, new_size);
        if (!block) { /* lack of heap space */
            be_throw(vm, BE_MALLOC_FAIL);
        }
    }
    vm->gc.usage = vm->gc.usage + new_size - old_size; /* update allocated count */
    return block;
}

size_t be_gc_memcount(bvm *vm)
{
    return vm->gc.usage;
}

bgcobject* be_newgcobj(bvm *vm, int type, size_t size)
{
    bgcobject *obj = be_gc_malloc(vm, size);
    be_gc_auto(vm);
    var_settype(obj, (bbyte)type); /* mark the object type */
    obj->marked = GC_WHITE; /* default gc object type is white */
    obj->next = vm->gc.list; /* link to the next field */
    vm->gc.list = obj; /* insert to head */
    return obj;
}

bgcobject* be_gc_newstr(bvm *vm, size_t size, int islong)
{
    bgcobject *obj;
    if (islong) { /* creating long strings is similar to ordinary GC objects */
        return be_newgcobj(vm, BE_STRING, size);
    }
    obj = be_gc_malloc(vm, size);
    be_gc_auto(vm);
    var_settype(obj, BE_STRING); /* mark the object type to BE_STRING */
    obj->marked = GC_WHITE; /* default string type is white */
    return obj;
}

void be_gc_fix(bvm *vm, bgcobject *obj)
{
    (void)vm;
    if (!gc_isconst(obj)) {
        gc_setfixed(obj);
    }
}

void be_gc_unfix(bvm *vm, bgcobject *obj)
{
    (void)vm;
    if (!gc_isconst(obj)) {
        gc_clearfixed(obj);
    }
}

static void mark_map(bvm *vm, bgcobject *obj)
{
    bmap *map = cast_map(obj);
    if (map) {
        bmapnode *node;
        bmapiter iter = be_map_iter();
        gc_setdark(obj);
        while ((node = be_map_next(map, &iter)) != NULL) {
            bmapkey *key = &node->key;
            bvalue *val = &node->value;
            mark_object(vm, key->v.gc, var_type(key));
            mark_object(vm, val->v.gc, var_type(val));
        }
    }
}

static void mark_list(bvm *vm, bgcobject *obj)
{
    blist *list = cast_list(obj);
    if (list) {
        bvalue *val = be_list_data(list);
        int count = be_list_count(list);
        gc_setdark(obj);
        for (; count--; val++) {
            mark_object(vm, val->v.gc, var_type(val));
        }
    }
}

static void mark_proto(bvm *vm, bgcobject *obj)
{
    bproto *p = cast_proto(obj);
    if (p) {
        int count;
        bvalue *k = p->ktab;
        bproto **ptab = p->ptab;
        gc_setdark(obj);
        for (count = p->nconst; count--; ++k) {
            mark_object(vm, k->v.gc, var_type(k));
        }
        for (count = p->nproto; count--; ++ptab) {
            mark_object(vm, gc_object(*ptab), BE_PROTO);
        }
        if (p->name) {
            gc_setdark(gc_object(p->name));
        }
#if BE_RUNTIME_DEBUG_INFO
        if (p->source) {
            gc_setdark(gc_object(p->source));
        }
#endif
    }
}

static void mark_closure(bvm *vm, bgcobject *obj)
{
    bclosure *cl = cast_closure(obj);
    if (cl) {
        int count = cl->nupvals;
        bupval **uv = cl->upvals;
        gc_setdark(obj);
        for (; count--; ++uv) {
            if ((*uv)->refcnt) {
                bvalue *v = (*uv)->value;
                mark_object(vm, v->v.gc, var_type(v));
            }
        }
        mark_proto(vm, gc_object(cl->proto));
    }
}

static void mark_ntvclos(bvm *vm, bgcobject *obj)
{
    bntvclos *f = cast_ntvclos(obj);
    if (f) {
        int count = f->nupvals;
        bupval **uv = &be_ntvclos_upval(f, 0);
        gc_setdark(obj);
        for (; count--; ++uv) {
            if ((*uv)->refcnt) {
                bvalue *v = (*uv)->value;
                mark_object(vm, v->v.gc, var_type(v));
            }
        }
    }
}

static void mark_class(bvm *vm, bgcobject *obj)
{
    bclass *c = cast_class(obj);
    /* mark this class and super class */
    while (c) {
        mark_map(vm, gc_object(be_class_members(c)));
        gc_setdark(be_class_name(c));
        gc_setdark(c);
        c = be_class_super(c);
    }
}

static void mark_instance(bvm *vm, bgcobject *obj)
{
    binstance *o = cast_instance(obj);
    if (o == NULL) {
        return;
    }
    mark_class(vm, gc_object(o->class));
    /* mark self instance and super instance */
    while (o) {
        bvalue *var = be_instance_members(o);
        int nvar = be_instance_member_count(o);
        gc_setdark(o);
        while (nvar--) {
            mark_object(vm, var->v.gc, var_type(var));
            var++;
        }
        o = be_instance_super(o);
    }
}

static void mark_module(bvm *vm, bgcobject *obj)
{
    bmodule *o = cast_module(obj);
    if (o) {
        gc_setdark(o);
        mark_map(vm, gc_object(o->table));
    }
}

static void mark_object(bvm *vm, bgcobject *obj, int type)
{
    if (obj && be_isgctype(type) && !gc_isdark(obj)) {
        switch (type) {
        case BE_STRING: gc_setdark(obj); break;
        case BE_CLASS: mark_class(vm, obj); break;
        case BE_PROTO: mark_proto(vm, obj); break;
        case BE_INSTANCE: mark_instance(vm, obj); break;
        case BE_MAP: mark_map(vm, obj); break;
        case BE_LIST: mark_list(vm, obj); break;
        case BE_CLOSURE: mark_closure(vm, obj); break;
        case BE_NTVCLOS: mark_ntvclos(vm, obj); break;
        case BE_MODULE: mark_module(vm, obj); break;
        default: break;
        }
    }
}

static void free_proto(bvm *vm, bgcobject *obj)
{
    bproto *proto = cast_proto(obj);
    if (proto) {
        be_gc_free(vm, proto->upvals, proto->nupvals * sizeof(bupvaldesc));
        be_gc_free(vm, proto->ktab, proto->nconst * sizeof(bvalue));
        be_gc_free(vm, proto->ptab, proto->nproto * sizeof(bproto*));
        be_gc_free(vm, proto->code, proto->codesize * sizeof(binstruction));
#if BE_RUNTIME_DEBUG_INFO
        be_gc_free(vm, proto->lineinfo, proto->nlineinfo * sizeof(blineinfo));
#endif
        be_gc_free(vm, proto, sizeof(bproto));
    }
}

static void free_closure(bvm *vm, bgcobject *obj)
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
                be_gc_free(vm, uv, sizeof(bupval));
            }
        }
        be_gc_free(vm, cl, sizeof(bclosure) + sizeof(bupval*) * (count - 1));
    }
}

static void free_ntvclos(bvm *vm, bgcobject *obj)
{
    bntvclos *f = cast_ntvclos(obj);
    if (f) {
        int count = f->nupvals;
        bupval **uv = &be_ntvclos_upval(f, 0);
        while (count--) {
            be_gc_free(vm, *uv++, sizeof(bupval));
        }
    }
}

static void free_lstring(bvm *vm, bgcobject *obj)
{
    blstring *ls = gc_cast(obj, BE_STRING, blstring);
    if (ls) {
        be_gc_free(vm, ls, sizeof(blstring) + ls->llen + 1);
    }
}

static void free_object(bvm *vm, bgcobject *obj)
{
    (void)vm;
    switch (obj->type) {
    case BE_STRING: free_lstring(vm, obj); break; /* long string */
    case BE_CLASS: be_gc_free(vm, obj, sizeof(bclass)); break;
    case BE_INSTANCE: be_gc_free(vm, obj, sizeof(binstance)); break;
    case BE_MAP: be_map_delete(vm, cast_map(obj)); break;
    case BE_LIST: be_list_delete(vm, cast_list(obj)); break;
    case BE_CLOSURE: free_closure(vm, obj); break;
    case BE_NTVCLOS: free_ntvclos(vm, obj); break;
    case BE_PROTO: free_proto(vm, obj); break;
    case BE_MODULE:
        be_module_delete(vm, cast_module(obj));
        break;
    default: break; /* case BE_STRING: break; */
    }
}

static void premark_global(bvm *vm)
{
    bvalue *v = vm->gbldesc.global.vlist.data;
    bvalue *end = v + be_global_count(vm);
    while (v < end) {
        if (be_isgcobj(v)) {
            gc_setgray(var_togc(v));
        }
        ++v;
    }
    v = vm->gbldesc.builtin.vlist.data;
    end = v + be_builtin_count(vm);
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
    end = vm->stacktop;
    while (v < end) {
        var_setnil(v);
        ++v;
    }
}

static void premark_fixed(bvm *vm)
{
    bgcobject *node = vm->gc.list;
    for (; node; node = node->next) {
        if (gc_isfixed(node) && gc_iswhite(node)) {
            gc_setgray(node);
        }
    }
}

static void mark_unscanned(bvm *vm)
{
    bgcobject *node;
    for (node = vm->gc.list; node; node = node->next) {
        if (gc_isgray(node)) {
            mark_object(vm, node, var_type(node));
        }
    }
}

static void destruct_object(bvm *vm, bgcobject *obj)
{
    if (obj->type == BE_INSTANCE) {
        int type;
        binstance *ins = cast_instance(obj);
        vm->gc.status |= GC_HALT;
        var_setinstance(vm->top, ins);
        be_incrtop(vm);
        type = be_instance_member(ins, be_newstr(vm, "deinit"), vm->top);
        be_incrtop(vm);
        if (basetype(type) == BE_FUNCTION) {
            be_dofunc(vm, vm->top - 1, 1);
        } else {
            be_stackpop(vm, 2);
        }
        vm->gc.status &= ~GC_HALT;
    }
}

static void destruct_white(bvm *vm)
{
    bgcobject *node = vm->gc.list;
    while (node) {
        if (gc_iswhite(node)) {
            destruct_object(vm, node);
        }
        node = node->next;
    }
}

static void delete_white(bvm *vm)
{
    bgcobject *node, *prev, *next;
    for (node = vm->gc.list, prev = node; node; node = next) {
        next = node->next;
        if (gc_iswhite(node)) {
            if (node == vm->gc.list) { /* first node */
                vm->gc.list = node->next;
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

static void reset_fixedlist(bvm *vm)
{
    bgcobject *node;
    for (node = vm->gc.fixed; node; node = node->next) {
        if (gc_isdark(node)) {
            gc_setgray(node);
        }
    }
}

void be_gc_auto(bvm *vm)
{
    if (vm->gc.status & GC_PAUSE && vm->gc.usage > vm->gc.threshold) {
        be_gc_collect(vm);
    }
}

void be_gc_collect(bvm *vm)
{
    if (vm->gc.status & GC_HALT) {
        return; /* the GC cannot run for some reason */
    }
    /* step 1: set root-set reference objects to unscanned */
    premark_global(vm); /* global objects */
    premark_stack(vm); /* stack objects */
    premark_fixed(vm);
    /* step 2: set unscanned objects to black */
    mark_unscanned(vm);
    /* step 3: destruct and delete unreachable objects */
    destruct_white(vm);
    delete_white(vm);
    be_gcstrtab(vm);
    /* step 4: reset the fixed objects */
    reset_fixedlist(vm);
    /* step 5: calculate the next GC threshold */
    vm->gc.threshold = next_threshold(vm->gc);
}
