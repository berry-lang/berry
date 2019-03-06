#include "be_exec.h"
#include "be_parser.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_mem.h"
#include "be_debug.h"
#include <setjmp.h>
#include <stdlib.h>

#define exec_try(j)         if (setjmp((j)->b) == 0)
#define exec_throw(j)       longjmp((j)->b, 1)

#define STR(s)              #s

#define STACK_OVER_MSG(n) \
    "stack overflow (maximum stack size is " STR(n) ")"

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

void be_exit(bvm *vm, int status)
{
    if (vm->errjmp) {
        be_pushint(vm, status);
        be_throw(vm, BE_EXIT);
    } else {
        exit(status);
    }
}

int be_execprotected(bvm *vm, bpfunc f, void *data)
{
    struct blongjmp jmp;
    jmp.status = 0;
    jmp.prev = vm->errjmp; /* save long jump position */
    vm->errjmp = &jmp;
    exec_try(vm->errjmp) {
        f(vm, data);
    }
    vm->errjmp = jmp.prev; /* restore long jump position */
    return jmp.status;
}

static void m_parser(bvm *vm, void *data)
{
    struct pparser *p = cast(struct pparser*, data);
    bclosure *cl = be_parser_source(vm, p->fname, p->source, p->length);
    var_setclosure(vm->top, cl);
    be_incrtop(vm);
}

int be_protectedparser(bvm *vm,
    const char *fname, const char *source, size_t length)
{
    int res;
    struct pparser s;
    int top = cast_int(vm->top - vm->stack);
    s.fname = fname;
    s.source = source;
    s.length = length;
    res = be_execprotected(vm, m_parser, &s);
    if (res) { /* recovery call stack */
        int idx = cast_int(vm->top - vm->reg);
        vm->top = vm->stack + top;
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

#if BE_DEBUG && defined(be_assert)
/* increase top register */
bvalue* be_incrtop(bvm *vm)
{
    bvalue *top = vm->top++;
    be_assert(top < vm->stacktop);
    return top;
}
#endif

void be_stackpush(bvm *vm)
{
    be_stack_require(vm, 1);
    be_incrtop(vm);
}

void be_stack_require(bvm *vm, int count)
{
    if (vm->top + count >= vm->stacktop) {
        be_stack_expansion(vm, count);
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

static void stack_resize(bvm *vm, size_t size)
{
    bvalue *olds = vm->stack;
    vm->stack = be_realloc(olds, sizeof(bvalue) * size);
    vm->stacktop = vm->stack + size;
    update_callstack(vm, olds);
}

void be_stack_expansion(bvm *vm, int n)
{
    size_t size = vm->stacktop - vm->stack;
    /* check new stack size */
    if (size + n > BE_STACK_TOTAL_MAX) {
        /* ensure the stack is enough when generating error messages. */
        stack_resize(vm, size + 10);
        be_pusherror(vm, STACK_OVER_MSG(BE_STACK_TOTAL_MAX));
    }
    stack_resize(vm, size + n);
}
