#include "be_exec.h"
#include "be_parser.h"
#include "be_vm.h"
#include "be_vector.h"
#include <setjmp.h>
#include <stdlib.h>

#define exec_try(j)     if (setjmp((j)->b) == 0)
#define exec_throw(j)   longjmp((j)->b, 1)

typedef jmp_buf bjmpbuf;

struct blongjmp {
    struct blongjmp *prev;
    bjmpbuf b;
    volatile int status;
};

struct pparser {
    const char *fname;
    const char *source;
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
    bclosure *cl = be_parser_source(vm, p->fname, p->source);
    var_setclosure(vm->top, cl);
    vm->top++;
}

int be_protectedparser(bvm *vm, const char *fname, const char *text)
{
    struct pparser s;
    s.fname = fname;
    s.source = text;
    return be_execprotected(vm, m_parser, &s);
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
    int calldepth = be_vector_count(vm->callstack);
    bvalue *reg = vm->reg, *top = vm->top;
    s.v = v;
    s.argc = argc;
    res = be_execprotected(vm, m_pcall, &s);
    if (res) { /* recovery call stack */
        int idx = vm->top - reg;
        be_vector_resize(vm->callstack, calldepth);
        vm->reg = reg;
        vm->top = top;
        vm->cf = be_stack_top(vm->callstack);
        be_pushvalue(vm, idx); /* copy error infomation */
    }
    return res;
}
