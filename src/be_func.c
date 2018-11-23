#include "be_func.h"
#include "be_gc.h"
#include "be_mem.h"
#include "be_vm.h"

#define clousersize(n) \
    (int)(sizeof(bclosure) + sizeof(bupval*) * ((n) - 1))

void be_initupvals(bvm *vm, bclosure *cl)
{
    int i, count = cl->proto->nupvals;
    bupvaldesc *desc = cl->proto->upvals;
    bvalue *stack = vm->reg;
    bupval **superuv = curcl(vm)->upvals;
    for (i = 0; i < count; ++i) {
        bvalue *ref;
        if (desc->instack) {
            ref = stack + desc->idx;
        } else {
            ref = superuv[desc->idx]->value;
        }
        desc++;
        cl->upvals[i] = be_findupval(vm, ref);
        cl->upvals[i]->refcnt++;
    }
}

bupval* be_findupval(bvm *vm, bvalue *level)
{
    bupval *node = vm->upvalist;
    while (node != NULL && node->value > level) {
        node = node->u.next;
    }
    if (!node || node->value != level) {
        /* not found */
        node = be_malloc(sizeof(bupval));
        node->value = level;
        node->refcnt = 0;
        /* insert to list head */
        node->u.next = vm->upvalist;
        vm->upvalist = node;
    }
    return node;
}

void be_upvals_close(bvm *vm, bvalue *level)
{
    bupval *node = vm->upvalist, *next;
    while (node && node->value >= level) {
        next = node->u.next;
        if (!node->refcnt) {
            be_free(node);
        } else {
            node->u.value = *node->value; /* move value to upvalue slot */
            node->value = &node->u.value;
        }
        node = next;
    }
    vm->upvalist = node;
}

bproto* be_newproto(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_PROTO, bproto);
    bproto *p = cast_proto(gco);

    if (p) {
        p->upvals = NULL;
        p->ktab = NULL;
        p->ptab = NULL;
        p->code = NULL;
        p->name = NULL;
        p->codesize = 0;
        p->nlocal = 0;
        p->nupvals = 0;
        p->nproto = 0;
        p->nconst = 0;
        p->nstack = 0;
        p->codesize = 0;
        p->argc = 0;
    }
    return p;
}

bclosure* be_newclosure(bvm *vm, int nupval)
{
    bgcobject *gco = be_newgcobj(vm, BE_CLOSURE, clousersize(nupval));
    bclosure *cl = cast_closure(gco);
    if (cl) {
        cl->proto = NULL;
        cl->nupvals = (bbyte)nupval;
        while (nupval--) {
            cl->upvals[nupval] = NULL;
        }
    }
    return cl;
}

static void init_upvals(bntvclos *f)
{
    int count = f->nupvals;
    bupval **upvals = &be_ntvclos_upval(f, 0);
    while (count--) {
        bupval *uv = be_malloc(sizeof(bupval)); /* was closed */
        uv->value = &uv->u.value;
        uv->refcnt = 1;
        var_setnil(uv->value);
        *upvals++ = uv;
    }
}

bntvclos* be_newprimclosure(bvm *vm, bcfunction cf, int nupvals)
{
    int size = (int)(sizeof(bntvclos) + sizeof(bupval*) * nupvals);
    bgcobject *gco = be_newgcobj(vm, BE_NTVCLOS, size);
    bntvclos *f = cast_ntvclos(gco);
    if (f) {
        f->f = cf;
        f->nupvals = nupvals;
        if (nupvals) {
            init_upvals(f);
        }
    }
    return f;
}
