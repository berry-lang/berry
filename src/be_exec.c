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

#define exec_try(j) if      (be_setjmp((j)->b) == 0)
#define exec_throw(j)       be_longjmp((j)->b, 1)

struct pparser {
    const char *fname;
    breader reader;
    void *data;
    bbyte islocal;
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

BERRY_API void be_exit(bvm *vm, int status)
{
    if (vm->errjmp) {
        be_pushint(vm, status);
        be_pop(vm, 1);
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

static void copy_exception(bvm *vm, int res, int dstindex)
{
    bvalue *dst = vm->stack + dstindex;
    if (res == BE_EXCEPTION || res == BE_EXIT) {
        bvalue *src = vm->top;
        *dst++ = *src++;
        if (res == BE_EXCEPTION) {
            *dst++ = *src++;
        }
    }
    vm->top = dst;
}

static void vm_state_restore(bvm *vm, const struct vmstate *state, int res)
{
    vm->reg = vm->stack + state->reg;
    /* copy exception information to top */
    copy_exception(vm, res, state->top);
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
    bclosure *cl = be_parser_source(vm,
        p->fname, p->reader, p->data, p->islocal);
    var_setclosure(vm->top, cl);
    be_incrtop(vm);
}

int be_protectedparser(bvm *vm,
    const char *fname, breader reader, void *data, int islocal)
{
    int res;
    struct pparser s;
    struct vmstate state;
    s.fname = fname;
    s.reader = reader;
    s.data = data;
    s.islocal = (bbyte)islocal;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, m_parser, &s);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state, res);
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

BERRY_API int be_loadbuffer(bvm *vm,
    const char *name, const char *buffer, size_t length)
{
    struct strbuf sbuf;
    sbuf.s = buffer;
    sbuf.len = length;
    return be_protectedparser(vm, name, _sgets, &sbuf, 0);
}

int be_fileparser(bvm *vm, const char *name, int islocal)
{
    int res = BE_IO_ERROR;
    struct filebuf *fbuf = be_malloc(vm, sizeof(struct filebuf));
    fbuf->fp = be_fopen(name, "r");
    if (fbuf->fp) {
        res = be_protectedparser(vm, name, _fgets, fbuf, islocal);
        be_fclose(fbuf->fp);
    }
    be_free(vm, fbuf, sizeof(struct filebuf));
    return res;
}

static int _loadmode(bvm *vm, const char *name, int islocal)
{
    int res = be_fileparser(vm, name, islocal);
    if (res == BE_IO_ERROR) {
        be_pushfstring(vm, "error: can not open file '%s'.", name);
    }
    return res;
}

BERRY_API int be_loadfile(bvm *vm, const char *name)
{
    return _loadmode(vm, name, 0);
}

/* load file as local scope */
BERRY_API int be_loadmodule(bvm *vm, const char *name)
{
    return _loadmode(vm, name, 1);
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
BERRY_API int be_loadexec(bvm *vm, const char *name)
{
    int res;
    struct vmstate state;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, _bytecode_load, (void *)name);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state, res);
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
BERRY_API int be_saveexec(bvm *vm, const char *name)
{
    int res;
    struct vmstate state;
    vm_state_save(vm, &state);
    res = be_execprotected(vm, _bytecode_save, (void *)name);
    if (res) { /* restore call stack */
        vm_state_restore(vm, &state, res);
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
        vm_state_restore(vm, &state, res);
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
    /* make sure there is enough stack space */
    be_stack_require(vm, 1 + BE_STACK_FREE_MIN);
    be_incrtop(vm);
}

void be_stack_require(bvm *vm, int count)
{
    if (vm->top + count >= vm->stacktop) {
        be_stack_expansion(vm, count);
    }
}

static void update_callstack(bvm *vm, intptr_t offset)
{
    bcallframe *cf = be_stack_top(&vm->callstack);
    bcallframe *base = be_stack_base(&vm->callstack);
    for (; cf >= base; --cf) {
        cf->func += offset;
        cf->top += offset;
        cf->reg += offset;
    }
    vm->top += offset;
    vm->reg += offset;
}

static void update_upvalues(bvm *vm, intptr_t offset)
{
    bupval *node = vm->upvalist;
    /* update the value referenced by open upvalues */
    for (; node != NULL; node = node->u.next) {
        node->value += offset;
    }
}

static void stack_resize(bvm *vm, size_t size)
{
    intptr_t offset;
    bvalue *old = vm->stack;
    size_t os = (vm->stacktop - old) * sizeof(bvalue);
    vm->stack = be_realloc(vm, old, os, sizeof(bvalue) * size);
    vm->stacktop = vm->stack + size;
    offset = vm->stack - old;
    /* update callframes */
    update_callstack(vm, offset);
    /* update open upvalues */
    update_upvalues(vm, offset);
}

void be_stack_expansion(bvm *vm, int n)
{
    size_t size = vm->stacktop - vm->stack;
    /* check new stack size */
    if (size + n > BE_STACK_TOTAL_MAX) {
        /* ensure the stack is enough when generating error messages. */
        stack_resize(vm, size + 1);
        be_raise(vm, "runtime_error", STACK_OVER_MSG(BE_STACK));
    }
    stack_resize(vm, size + n);
}

static void fixup_exceptstack(bvm* vm, struct bexecptframe* lbase)
{
    struct bexecptframe *base = be_stack_base(&vm->exceptstack);
    if (lbase != base) { /* the address has changed when the stack is expanded */
        struct bexecptframe *top = be_stack_top(&vm->exceptstack);
        bbyte *begin = (bbyte*)&lbase->errjmp;
        bbyte *end = (bbyte*)&(lbase + (top - base))->errjmp;
        intptr_t offset = (bbyte*)base - (bbyte*)lbase;
        struct blongjmp *errjmp = vm->errjmp;
        while (errjmp) {
            bbyte *prev = (bbyte*)errjmp->prev;
            if (prev >= begin && prev < end) {
                prev += offset; /* fixup the prev pointer */
                errjmp->prev = (struct blongjmp*)prev;
            }
            errjmp = (struct blongjmp*)prev;
        }
    }
}

/* set an exception handling recovery point. To do this, we have to
 * push some VM states into the exception stack. */
void be_except_block_setup(bvm *vm)
{
    struct bexecptframe *frame;
    struct bexecptframe *lbase = be_stack_base(&vm->exceptstack);
    be_stack_push(vm, &vm->exceptstack, NULL);
    frame = be_stack_top(&vm->exceptstack);
    frame->depth = be_stack_count(&vm->callstack); /* the call stack depth */
    frame->ip = vm->ip; /* OP_EXBLK's next instruction pointer */
    /* set longjmp() jump point */
    frame->errjmp.status = 0;
    frame->errjmp.prev = vm->errjmp; /* save long jump list */
    vm->errjmp = &frame->errjmp;
    fixup_exceptstack(vm, lbase);
}

/* resumes to the state of the previous frame when an exception occurs. */
void be_except_block_resume(bvm *vm)
{
    int errorcode = vm->errjmp->status;
    struct bexecptframe *frame = be_stack_top(&vm->exceptstack);
    if (errorcode == BE_EXCEPTION) {
        vm->errjmp = vm->errjmp->prev;
        /* jump to except instruction */
        vm->ip = frame->ip + IGET_sBx(frame->ip[-1]);
        if (be_stack_count(&vm->callstack) > frame->depth) {
            bvalue *top = vm->top;
            bcallframe *cf = be_vector_at(&vm->callstack, frame->depth);
            vm->top = cf->top;
            vm->reg = cf->reg;
            vm->cf = frame->depth ? cf - 1 : NULL;
            be_vector_resize(vm, &vm->callstack, frame->depth);
            /* copy the exception value and argument to the top of
             * the current function */
            vm->top[0] = top[0]; /* exception value */
            vm->top[1] = top[1]; /* exception argument */
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

/* TODO: consider GC */
void be_save_stacktrace(bvm *vm)
{
    be_stack_clear(&vm->tracestack);
    if (be_vector_count(&vm->callstack)) {
        bcallframe *cf;
        bcallframe *begin = be_vector_first(&vm->callstack);
        bcallframe *end = be_vector_end(&vm->callstack);
        for (cf = begin; cf <= end; ++cf) {
            bcallsnapshot st = {
                *cf->func,
                cf == end ? vm->ip : cf[1].ip
            };
            be_stack_push(vm, &vm->tracestack, &st);
        }
    }
}
