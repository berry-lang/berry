#include "be_exec.h"
#include "be_parser.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_mem.h"
#include "be_debug.h"
#include <setjmp.h>
#include <stdlib.h>

#define exec_try(j)     if (setjmp((j)->b) == 0)
#define exec_throw(j)   longjmp((j)->b, 1)

typedef jmp_buf bjmpbuf;

struct blongjmp {
    bjmpbuf b;
    struct blongjmp *prev;
    volatile int status;
};

struct pparser {
    const char *fname;
    const char *source;
    size_t length;
};

struct pcall {
    bvalue *v;
    int argc;
};

void be_throw(bvm *vm, int errorcode)
{
    if (vm->errjmp) {
        vm->errjmp->status = errorcode;
        exec_throw(vm->errjmp);
    } else {
        abort();
    }
}

int be_execprotected(bvm *vm, bpfunc f, void *data)
{
    struct blongjmp jmp;
    jmp.status = 0;
    jmp.prev = vm->errjmp;
    vm->errjmp = &jmp;
    exec_try(vm->errjmp) {
        f(vm, data);
    }
    return jmp.status;
}

static void m_parser(bvm *vm, void *data)
{
    struct pparser *p = cast(struct pparser*, data);
    bclosure *cl = be_parser_source(vm, p->fname, p->source, p->length);
    var_setclosure(vm->top, cl);
    vm->top++;
}

int be_protectedparser(bvm *vm,
    const char *fname, const char *source, size_t length)
{
    int res;
    struct pparser s;
    bvalue *top = vm->top;
    s.fname = fname;
    s.source = source;
    s.length = length;
    res = be_execprotected(vm, m_parser, &s);
    if (res) { /* recovery call stack */
        int idx = cast_int(vm->top - vm->reg);
        vm->top = top;
        be_pushvalue(vm, idx); /* copy error information */
    }
    return res;
}

static void m_pcall(bvm *vm, void *data)
{
    struct pcall *p = cast(struct pcall*, data);
    be_dofunc(vm, p->v, p->argc);
}

int be_protectedcall(bvm *vm, bvalue *v, int argc)
{
    int res;
    struct pcall s;
    int calldepth = be_vector_count(&vm->callstack);
    int reg = cast_int(vm->reg - vm->stack);
    int top = cast_int(vm->top - vm->stack);
    s.v = v;
    s.argc = argc;
    res = be_execprotected(vm, m_pcall, &s);
    if (res) { /* recovery call stack */
        int idx = cast_int(vm->top - (vm->stack + reg));
        be_vector_resize(&vm->callstack, calldepth);
        vm->reg = vm->stack + reg;
        vm->top = vm->stack + top;
        vm->cf = be_stack_top(&vm->callstack);
        be_pushvalue(vm, idx); /* copy error information */
    }
    return res;
}

void be_stackpush(bvm *vm)
{
    if (++vm->top >= vm->stacktop) {
        be_stack_expansion(vm, BE_STACK_FREE_MIN);
    }
}

void be_stackcheck(bvm *vm, int need)
{
    if (vm->top + need >= vm->stacktop) {
        be_stack_expansion(vm, need);
    }
}

static void update_callstack(bvm *vm, bvalue *oldstack)
{
    bcallframe *cf = be_stack_top(&vm->callstack);
    bcallframe *base = be_stack_base(&vm->callstack);
	bvalue *stack = vm->stack;
    for (; cf >= base; --cf) {
        cf->func = stack + (cf->func - oldstack);
        cf->top = stack + (cf->top - oldstack);
        cf->reg = stack + (cf->reg - oldstack);
    }
    vm->top = stack + (vm->top - oldstack);
    vm->reg = stack + (vm->reg - oldstack);
}

void be_stack_expansion(bvm *vm, int n)
{
    size_t newsize;
    bvalue *oldstack = vm->stack;

    newsize = vm->stacktop - oldstack + n;
    if (newsize > BE_STACK_TOTAL_MAX) {
        be_debug_error(vm, BE_EXEC_ERROR,
            "stack overflow (maxim stack size: %d)",
            BE_STACK_TOTAL_MAX);
    }
    vm->stack = be_realloc(oldstack, sizeof(bvalue) * newsize);
    vm->stacktop = vm->stack + newsize;
    update_callstack(vm, oldstack);
}
