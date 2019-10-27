#include "be_exec.h"
#include "be_parser.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_mem.h"
#include "be_sys.h"
#include "be_debug.h"
#include "be_bytecode.h"
#include "be_opcode.h"
#include <stdlib.h>

#define FILE_BUFFER_SIZE    256

#define STR(s)              #s

#define STACK_OVER_MSG(n) \
    "stack overflow (maximum stack size is " STR(n) ")"

#ifdef BE_EXPLICIT_ABORT
  #define abort             BE_EXPLICIT_ABORT
#endif

#ifdef BE_EXPLICIT_EXIT
  #define exit              BE_EXPLICIT_EXIT
#endif

#define exec_try(j) if (setjmp((j)->b) == 0)
#define exec_throw(j) longjmp((j)->b, 1)

struct pparser {
    const char *fname;
    breader reader;
    void *data;
};

struct pcall {
    bvalue *v;
    int argc;
};

struct vmstate {
    int top, reg, depth;
};

struct strbuf {
    size_t len;
    const char *s;
};

struct filebuf {
    void *fp;
    char buf[FILE_BUFFER_SIZE];
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

void be_throw_message(bvm *vm, int errorcode, const char *msg)
{
    be_pushstring(vm, msg);
    be_throw(vm, errorcode);
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

static void vm_state_save(bvm *vm, struct vmstate *state)
{
    state->depth = be_stack_count(&vm->callstack);
    state->top = cast_int(vm->top - vm->stack);
    state->reg = cast_int(vm->reg - vm->stack);
}

static void vm_state_restore(bvm *vm, const struct vmstate *state)
{
    int idx = cast_int(vm->top - (vm->stack + state->reg));
    vm->reg = vm->stack + state->reg;
    /* copy error information to top */
    be_moveto(vm, idx, state->top - state->reg + 1);
    vm->top = vm->stack + state->top + 1;
    be_assert(be_stack_count(&vm->callstack) >= state->depth);
    if (be_stack_count(&vm->callstack) > state->depth) {
        be_vector_resize(vm, &vm->callstack, state->depth);
        vm->cf = be_stack_top(&vm->callstack);
    }
}

#if BE_USE_SCRIPT_COMPILER

static void m_parser(bvm *vm, void *data)
{
    struct pparser *p = cast(struct pparser*, data);
    bclosure *cl = be_parser_source(vm, p->fname, p->reader, p->data);
    var_setclosure(vm->top, cl);
    be_incrtop(vm);
}

int be_protectedparser(bvm *vm,
    const char *fname, breader reader, void *data)
{
    int res;
    struct pparser s;
    struct vmstate state;
    s.fname = fname;
    s.reader = reader;
    s.data = data;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, m_parser, &s);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state);
    }
    return res;
}

static const char* _sgets(void *data, size_t *size)
{
    struct strbuf *sb = data;
    *size = sb->len;
    if (sb->len) {
        sb->len = 0;
        return sb->s;
    }
    return NULL;
}

static const char* _fgets(void *data, size_t *size)
{
    struct filebuf *fb = data;
    *size = be_fread(fb->fp, fb->buf, sizeof(fb->buf));
    if (*size) {
        return fb->buf;
    }
    return NULL;
}

int be_loadbuffer(bvm *vm,
    const char *name, const char *buffer, size_t length)
{
    struct strbuf sbuf;
    sbuf.s = buffer;
    sbuf.len = length;
    return be_protectedparser(vm, name, _sgets, &sbuf);
}

int be_loadfile(bvm *vm, const char *name)
{
    int res = BE_IO_ERROR;
    struct filebuf *fbuf = be_malloc(vm, sizeof(struct filebuf));
    fbuf->fp = be_fopen(name, "r");
    if (fbuf->fp) {
        res = be_protectedparser(vm, name, _fgets, fbuf);
        be_fclose(fbuf->fp);
    } else {
        be_pushfstring(vm, "error: can not open file '%s'.", name);
    }
    be_free(vm, fbuf, sizeof(struct filebuf));
    return res;
}

#endif

static void _bytecode_load(bvm *vm, void *data)
{
    bclosure *cl = be_bytecode_load(vm, (const char *)data);
    if (cl != NULL) {
        var_setclosure(vm->top, cl);
    } else {
        var_setnil(vm->top);
    }
    be_incrtop(vm);
}

/* load bytecode file */
int be_loadexec(bvm *vm, const char *name)
{
    int res;
    struct vmstate state;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, _bytecode_load, (void *)name);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state);
    }
    return res;
}

static void _bytecode_save(bvm *vm, void *data)
{
    if (be_top(vm) > 0 && var_isclosure(vm->top - 1)) {
        bclosure *cl = var_toobj(vm->top - 1);
        be_bytecode_save(vm, (const char *)data, cl->proto);
    }
}

/* save bytecode file */
int be_saveexec(bvm *vm, const char *name)
{
    int res;
    struct vmstate state;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, _bytecode_save, (void *)name);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state);
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
    struct vmstate state;
    s.v = v;
    s.argc = argc;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, m_pcall, &s);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state);
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
    be_incrtop(vm);
    /* make sure there is enough stack space */
    be_stack_require(vm, 1 + BE_STACK_FREE_MIN);
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
    bvalue *old = vm->stack;
    size_t os = (vm->stacktop - old) * sizeof(bvalue);
    vm->stack = be_realloc(vm, old, os, sizeof(bvalue) * size);
    vm->stacktop = vm->stack + size;
    update_callstack(vm, old);
}

void be_stack_expansion(bvm *vm, int n)
{
    size_t size = vm->stacktop - vm->stack;
    /* check new stack size */
    if (size + n > BE_STACK_TOTAL_MAX) {
        /* ensure the stack is enough when generating error messages. */
        stack_resize(vm, size + 1);
        be_pusherror(vm, STACK_OVER_MSG(BE_STACK_TOTAL_MAX));
    }
    stack_resize(vm, size + n);
}

/* set an exception handling recovery point. To do this, we have to
 * push some VM states into the exception stack. */
void be_except_block_setup(bvm *vm)
{
    struct bexecptframe *frame;
    be_stack_push(vm, &vm->exceptstack, NULL);
    frame = be_stack_top(&vm->exceptstack);
    frame->depth = be_stack_count(&vm->callstack); /* the call stack depth */
    frame->ip = vm->ip; /* instruction pointer */
    /* set longjmp() jump point */
    frame->errjmp.status = 0;
    frame->errjmp.prev = vm->errjmp; /* save long jump list */
    vm->errjmp = &frame->errjmp;
}

/* resumes to the state of the previous frame when an exception occurs. */
void be_except_block_resume(bvm *vm)
{
    int errorcode = vm->errjmp->status;
    struct bexecptframe *frame = be_stack_top(&vm->exceptstack);
    if (errorcode == BE_EXCEPTION) {
        vm->errjmp = vm->errjmp->prev;
        /* jump to except instruction */
        vm->ip = frame->ip + IGET_sBx(*frame->ip);
        if (be_stack_count(&vm->callstack) > frame->depth) {
            bvalue *top = vm->top;
            bcallframe *cf = be_vector_at(&vm->callstack, frame->depth);
            vm->top = cf->top;
            vm->reg = cf->reg;
            vm->cf = frame->depth ? cf - 1 : NULL;
            be_vector_resize(vm, &vm->callstack, frame->depth);
            /* copy the exception value and argument to the top of
             * the current function */
            vm->top[0] = top[0];
            vm->top[1] = top[1];
        }
        be_stack_pop(&vm->exceptstack);
    } else { /* other errors cannot be catch by the except block */
        /* find the next error handling location */
        while (vm->errjmp == &frame->errjmp) {
            vm->errjmp = vm->errjmp->prev;
            be_stack_pop(&vm->exceptstack);
            frame = be_stack_top(&vm->exceptstack);
        }
        be_throw(vm, errorcode); /* rethrow this exception */
    }
}

/* only close the except block, no other operations */
void be_except_block_close(bvm *vm, int count)
{
    struct bexecptframe *frame;
    int size = be_stack_count(&vm->exceptstack);
    be_assert(count > 0 && count <= size);
    frame = be_vector_at(&vm->exceptstack, size - count);
    vm->errjmp = frame->errjmp.prev;
    be_vector_resize(vm, &vm->exceptstack, size - count);
}
