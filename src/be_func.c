/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_func.h"
#include "be_gc.h"
#include "be_mem.h"
#include "be_vm.h"
#include "be_exec.h"
#include <string.h>

#define clousersize(n) \
    (sizeof(bclosure_t) + sizeof(bupval_t*) * ((size_t)(n) - 1))

static bupval_t* findupval(bvm_t *vm, bvalue_t *level)
{
    bupval_t *node = vm->upvalist;
    while (node != NULL && node->value > level) {
        node = node->u.next;
    }
    if (!node || node->value != level) {
        /* not found */
        node = be_malloc(vm, sizeof(bupval_t));
        node->value = level;
        node->refcnt = 0;
        /* insert to list head */
        node->u.next = vm->upvalist;
        vm->upvalist = node;
    }
    return node;
}

void be_initupvals(bvm_t *vm, bclosure_t *cl)
{
    int count = cl->proto->nupvals;
    bupvaldesc_t *desc = cl->proto->upvals;
    bvalue_t *stack = vm->reg;
    bupval_t **uv = cl->upvals;
    bupval_t **superuv = cast(bclosure_t*, var_toobj(vm->cf->func))->upvals;
    for (; count--; desc++, uv++) {
        if (desc->instack) {
            bvalue_t *ref = stack + desc->idx;
            *uv = findupval(vm, ref);
        } else {
            *uv = superuv[desc->idx];
        }
        (*uv)->refcnt++;
    }
}

void be_upvals_close(bvm_t *vm, bvalue_t *level)
{
    bupval_t *node = vm->upvalist, *next;
    bupval_t **prev = &vm->upvalist;
    while (node) {
        next = node->u.next;
        if (node->value >= level) {
            if (!node->refcnt) {
                be_free(vm, node, sizeof(bupval_t));
            } else {
                node->u.value = *node->value; /* move value to upvalue slot */
                node->value = &node->u.value;
            }
            *prev = next;   /* remove from linked list */
        } else {
            prev = &node->u.next;
        }
        node = next;
    }
}

void be_release_upvalues(bvm_t *vm, bclosure_t *cl)
{
    int i, count = cl->nupvals;
    for (i = 0; i < count; ++i) {
        bupval_t *uv = cl->upvals[i];
        if (uv) {
            if (uv->refcnt) {
                --uv->refcnt;
            }
            /* delete non-referenced closed upvalue */
            if (uv->value == &uv->u.value && !uv->refcnt) {
                be_free(vm, uv, sizeof(bupval_t));
            }
        }
    }
}

bproto_t* be_newproto(bvm_t *vm)
{
    bgcobject_t *gco = be_gcnew(vm, BE_PROTO, bproto_t);
    bproto_t *p = cast_proto(gco);
    if (p) {
        p->upvals = NULL;
        p->ktab = NULL;
        p->ptab = NULL;
        p->code = NULL;
        p->name = NULL;
        p->gray = NULL;
        p->codesize = 0;
        p->nupvals = 0;
        p->nproto = 0;
        p->nconst = 0;
        p->nstack = 0;
        p->codesize = 0;
        p->argc = 0;
        p->varg = 0;
        p->source = NULL;
#if BE_DEBUG_RUNTIME_INFO
        p->lineinfo = NULL;
        p->nlineinfo = 0;
#endif
#if BE_DEBUG_VAR_INFO
        p->varinfo = NULL;
        p->nvarinfo = 0;
#endif
    }
    return p;
}

bclosure_t* be_newclosure(bvm_t *vm, int nupval)
{
    bgcobject_t *gco = be_newgcobj(vm, BE_CLOSURE, clousersize(nupval));
    bclosure_t *cl = cast_closure(gco);
    if (cl) {
        cl->proto = NULL;
        cl->nupvals = (bbyte_t)nupval;
        while (nupval--) {
            cl->upvals[nupval] = NULL;
        }
    }
    return cl;
}

static void init_upvals(bvm_t *vm, bntvclos_t *f)
{
    int count = f->nupvals;
    bupval_t **upvals = &be_ntvclos_upval(f, 0);
    while (count--) {
        bupval_t *uv = be_malloc(vm, sizeof(bupval_t)); /* was closed */
        uv->value = &uv->u.value;
        uv->refcnt = 1;
        var_setnil(uv->value);
        *upvals++ = uv;
    }
}

bntvclos_t* be_newntvclosure(bvm_t *vm, bntvfunc cf, int nupvals)
{
    size_t size = sizeof(bntvclos_t) + sizeof(bupval_t*) * nupvals;
    bgcobject_t *gco = be_newgcobj(vm, BE_NTVCLOS, size);
    bntvclos_t *f = cast_ntvclos(gco);
    if (f) {
        f->f = cf;
        f->nupvals = (bbyte_t)nupvals;
        if (nupvals) {
            var_setntvclos(vm->top, f);
            be_incrtop(vm);
            init_upvals(vm, f); /* may be GC */
            be_stackpop(vm, 1);
        }
    }
    return f;
}

#if BE_DEBUG_VAR_INFO
bstring_t* be_func_varname(bproto_t *proto, int index, int pc)
{
    int i, nvarinfo = proto->nvarinfo;
    bvarinfo_t *varinfo = proto->varinfo;
    for (i = 0; i < nvarinfo && varinfo[i].beginpc <= pc; ++i) {
        if (pc <= varinfo[i].endpc && index-- == 0) {
            return varinfo[i].name;
        }
    }
    return NULL;
}
#endif
