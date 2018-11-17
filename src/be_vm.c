#include "be_vm.h"
#include "be_opcode.h"
#include "be_string.h"
#include "be_class.h"
#include "be_func.h"
#include "be_vector.h"
#include "be_map.h"
#include "be_mem.h"
#include "be_var.h"
#include "be_gc.h"
#include "be_debug.h"

#define MAX_TMPREG      10

#define NOT_METHOD      BE_NONE

#define RA(i)   (vm->reg + IGET_RA(i))
#define RKB(i)  ((isKB(i) ? curcl(vm)->proto->ktab \
                          : vm->reg) + KR2idx(IGET_RKB(i)))
#define RKC(i)  ((isKC(i) ? curcl(vm)->proto->ktab \
                          : vm->reg) + KR2idx(IGET_RKC(i)))

#define var2real(_v) \
    (var_isreal(_v) ? (_v)->v.r : (breal)(_v)->v.i)

#define cast_bool(v)        ((v) ? btrue : bfalse)
#define ibinop(op, a, b)    ((a)->v.i op (b)->v.i)

#define define_function(name, block) \
    static void name(bvm *vm, binstruction ins) { \
        block \
    }

#define relop_block(op) \
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins); \
    if (var_isint(a) && var_isint(b)) { \
        var_setbool(dst, ibinop(op, a, b)); \
    } else if (var_isnumber(a) && var_isnumber(b)) { \
        breal x = var2real(a), y = var2real(b); \
        var_setbool(dst, x op y); \
    } else if (var_isinstance(a)) { \
        object_binop(vm, #op, dst, a, b); \
    } else { \
        vm_error(vm, "a " #op " b param error."); \
    }

#define equal_block(op, opstr) \
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins); \
    if (var_isint(a) && var_isint(b)) { \
        var_setbool(dst, ibinop(op, a, b)); \
    } else if (var_isnumber(a) && var_isnumber(b)) { \
        breal x = var2real(a), y = var2real(b); \
        var_setbool(dst, x op y); \
    } else if (var_isnil(a) || var_isnil(b)) { \
        bbool res = var_type(a) op var_type(b); \
        var_setbool(dst, res); \
    } else if (var_isstring(a) && var_isstring(b)) { \
        var_setbool(dst, opstr be_eqstr(a->v.s, b->v.s)); \
    } else if (var_isinstance(a)) { \
        object_binop(vm, #op, dst, a, b); \
    } else { \
        vm_error(vm, "a " #op " b param error."); \
    }

/* script closure call */
#define push_closure(_vm, _f, _argc, _t) { \
    bcallframe *_cf; \
    be_stack_push(_vm->callstack, NULL); \
    _cf = be_stack_top(_vm->callstack); \
    _cf->func = _t ? _f - 1 : _f; \
    _cf->top = _vm->top; \
    _cf->reg = _vm->reg; \
    _cf->ip = var2cl(_f)->proto->code; \
    _cf->status = NONE_FLAG; \
    _vm->reg = _f + 1; \
    _vm->top = _f + 1 + _argc; \
    _vm->cf = _cf; \
}

#define push_ntvfunc(_vm, _f, _argc, _t) { \
    bcallframe *_cf; \
    be_stack_push(_vm->callstack, NULL); \
    _cf = be_stack_top(_vm->callstack); \
    _cf->func = _t ? _f - 1 : _f; \
    _cf->top = _vm->top; \
    _cf->reg = _vm->reg; \
    _cf->status = PRIM_FUNC; \
    _vm->reg = _f + 1; \
    _vm->top = _f + 1 + _argc; \
    _vm->cf = _cf; \
}

#define ret_cfunction(vm) { \
    bcallframe *_cf = be_stack_top(vm->callstack); \
    be_stack_pop(vm->callstack); \
    vm->cf = be_stack_top(vm->callstack); \
    vm->reg = _cf->reg; \
    vm->top = _cf->top; \
}

static void vm_error(bvm *vm, const char *msg)
{
    (void)vm;
    be_printf("%s\n", msg);
    be_abort();
}

void do_closure(bvm *vm, bvalue *reg, bclosure *cl, int argc)
{
    if (argc != cl->proto->argc && cl->proto->argc != -1) {
        vm_error(vm, "function argc error");
    }
    var_setclosure(reg, cl);
    push_closure(vm, reg, cl->proto->nlocal + MAX_TMPREG, 0);
    be_exec(vm);
}

void do_ntvfunc(bvm *vm, bvalue *reg, bntvfunc *f, int argc)
{
    if (argc != f->argc && f->argc != -1) {
        vm_error(vm, "function argc error");
    }
    var_setntvfunc(reg, f);
    push_ntvfunc(vm, reg, argc, 0);
    f->f(vm); /* call C primitive function */
    ret_cfunction(vm);
}

void do_funcvar(bvm *vm, bvalue *reg, int argc)
{
    switch (var_type(reg)) {
    case BE_CLOSURE:
        do_closure(vm, reg, var_toobj(reg), argc);
        break;
    case BE_NTVFUNC: {
        do_ntvfunc(vm, reg, var_toobj(reg), argc);
        break;
    }
    default:
        break;
    }
}

static bbool obj2bool(bvm *vm, bvalue *obj)
{
    bvalue *top = topreg(vm);
    /* get operator method */
    be_instance_field(obj->v.p, be_newstr(vm, "tobool"), top);
    top[1] = *obj; /* move self to argv[0] */
    do_funcvar(vm, top, 1); /* call method 'item' */
    return var_isbool(top) ? var_tobool(top) : btrue;
}

static bbool var2bool(bvm *vm, bvalue *v)
{
    switch (var_basetype(v)) {
    case BE_BOOL:
        return v->v.b;
    case BE_INT:
        return cast_bool(v->v.i);
    case BE_REAL:
        return cast_bool(v->v.r);
    case BE_INSTANCE:
        return obj2bool(vm, v);
    default:
        return btrue;
    }
}

static void object_binop(bvm *vm, const char *op,
                         bvalue *dst, bvalue *a, bvalue *b)
{
    bvalue *top = topreg(vm);
    /* get operator method */
    be_instance_field(a->v.p, be_newstr(vm, op), top);
    top[1] = *a; /* move self to argv[0] */
    top[2] = *b; /* move other to argv[1] */
    do_funcvar(vm, top, 2); /* call method 'item' */
    *dst = *top; /* copy result to dst */
}

static void object_unop(bvm *vm, const char *op,
                        bvalue *dst, bvalue *src)
{
    bvalue *top = topreg(vm);
    /* get operator method */
    be_instance_field(src->v.p, be_newstr(vm, op), top);
    top[1] = *src; /* move self to argv[0] */
    do_funcvar(vm, top, 1); /* call method 'item' */
    *dst = *top; /* copy result to dst */
}

static void i_ldnil(bvm *vm, binstruction ins)
{
    var_setnil(RA(ins));
}

static void i_ldbool(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    var_setbool(v, IGET_RKB(ins));
    if (IGET_RKC(ins)) { /* skip next instruction */
        vm->cf->ip++;
    }
}

static void i_ldint(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    var_setint(v, IGET_sBx(ins));
}

static void i_getglobal(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    int idx = IGET_Bx(ins);
    *v = vm->global[idx];
}

static void i_setglobal(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    int idx = IGET_Bx(ins);
    vm->global[idx] = *v;
}

static void i_getupval(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    int idx = IGET_Bx(ins);
    *v = *curcl(vm)->upvals[idx]->value;
}

static void i_setupval(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    int idx = IGET_Bx(ins);
    *curcl(vm)->upvals[idx]->value = *v;
}

static void i_ldconst(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins);
    *dst = curcl(vm)->proto->ktab[IGET_Bx(ins)];
}

static void i_move(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins);
    *dst = *RKB(ins);
}

static void i_add(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (var_isint(a) && var_isint(b)) {
        var_setint(dst, ibinop(+, a, b));
    } else if (var_isnumber(a) && var_isnumber(b)) {
        breal x = var2real(a), y = var2real(b);
        var_setreal(dst, x + y);
    } else if (var_isinstance(a)) {
        object_binop(vm, "+", dst, a, b);
    } else {
        vm_error(vm, "ADD param error.");
    }
}

static void i_sub(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (var_isint(a) && var_isint(b)) {
        var_setint(dst, ibinop(-, a, b));
    } else if (var_isnumber(a) && var_isnumber(b)) {
        breal x = var2real(a), y = var2real(b);
        var_setreal(dst, x - y);
    } else if (var_isinstance(a)) {
        object_binop(vm, "-", dst, a, b);
    } else {
        vm_error(vm, "SUB param error.");
    }
}

static void i_mul(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (var_isint(a) && var_isint(b)) {
        var_setint(dst, ibinop(*, a, b));
    } else if (var_isnumber(a) && var_isnumber(b)) {
        breal x = var2real(a), y = var2real(b);
        var_setreal(dst, x * y);
    } else if (var_isinstance(a)) {
        object_binop(vm, "*", dst, a, b);
    } else {
        vm_error(vm, "MUL param error.");
    }
}

static void i_div(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (var_isint(a) && var_isint(b)) {
        var_setint(dst, ibinop(/, a, b));
    } else if (var_isnumber(a) && var_isnumber(b)) {
        breal x = var2real(a), y = var2real(b);
        var_setreal(dst, x / y);
    } else if (var_isinstance(a)) {
        object_binop(vm, "/", dst, a, b);
    } else {
        vm_error(vm, "DIV param error.");
    }
}

static void i_mod(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (var_isint(a) && var_isint(b)) {
        var_setint(dst, ibinop(%, a, b));
    } else if (var_isinstance(a)) {
        object_binop(vm, "%", dst, a, b);
    } else {
        vm_error(vm, "MOD param error.");
    }
}

static void i_neg(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins);
    if (var_isint(a)) {
        var_setint(dst, -a->v.i);
    } else if (var_isreal(a)) {
        var_setreal(dst, -a->v.r);
    } else if (var_isinstance(a)) {
        object_unop(vm, "-*", dst, a);
    } else {
        vm_error(vm, "NEG param error.");
    }
}

define_function(i_lt, relop_block(<))
define_function(i_le, relop_block(<=))
define_function(i_eq, equal_block(==, ))
define_function(i_ne, equal_block(!=, !))
define_function(i_gt, relop_block(>))
define_function(i_ge, relop_block(>=))

static void i_jump(bvm *vm, binstruction ins)
{
    bcallframe *cf = vm->cf;
    cf->ip += IGET_sBx(ins);
}

static void i_jumptrue(bvm *vm, binstruction ins)
{
    bcallframe *cf = vm->cf;
    if (var2bool(vm, RA(ins))) {
        cf->ip += IGET_sBx(ins);
    }
}

static void i_jumpfalse(bvm *vm, binstruction ins)
{
    bcallframe *cf = vm->cf;
    if (!var2bool(vm, RA(ins))) {
        cf->ip += IGET_sBx(ins);
    }
}

static void i_return(bvm *vm, binstruction ins)
{
    bcallframe *cf = vm->cf;
    bvalue *ret = vm->cf->func, *src = RKB(ins);
    be_stack_pop(vm->callstack); /* pop don't delete */
    vm->reg = cf->reg;
    vm->top = cf->top;
    if (cf->status & BASE_FRAME) { /* entrance function */
        *ret = *src;
        vm->cf = NULL; /* mainfunction return */
    } else {
        *ret = *src;
        vm->cf = be_stack_top(vm->callstack);
    }
}

static void i_call(bvm *vm, binstruction ins)
{
    bvalue *var = RA(ins);
    int mode = 0, argc = IGET_RKB(ins);

    recall: /* goto: instantiation class and call constructor */
    switch (var_type(var)) {
    case NOT_METHOD:
        ++var; --argc; mode = 1;
        goto recall;
    case BE_CLASS:
        if (be_class_newobj(vm, var_toobj(var), var, ++argc)) {
            ++var; /* to next register */
            goto recall; /* call constructor */
        }
        ++vm->cf->ip; /* to next instruction */
        break;
    case BE_CLOSURE: {
        bclosure *cl = var_toobj(var);
        if (argc != cl->proto->argc) {
            vm_error(vm, "function argc error");
        }
        push_closure(vm, var, cl->proto->nlocal + MAX_TMPREG, mode);
        break;
    }
    case BE_NTVFUNC: {
        bntvfunc *f = var_toobj(var);
        if (argc != f->argc && f->argc != -1) {
            vm_error(vm, "function argc error");
        }
        push_ntvfunc(vm, var, argc, mode);
        f->f(vm); /* call C primitive function */
        ret_cfunction(vm);
        ++vm->cf->ip; /* to next instruction */
        break;
    }
    default:
        vm_error(vm, "value is not function");
    }
}

static void i_closure(bvm *vm, binstruction ins)
{
    bvalue *reg = RA(ins);
    bproto *p = curcl(vm)->proto->ptab[IGET_Bx(ins)];
    bclosure *cl = be_newclosure(vm, p->nupvals);
    cl->proto = p;
    var_setclosure(reg, cl);
    be_initupvals(vm, cl);
}

static void i_getfield(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b) && var_isstring(c)) {
        be_instance_field(var_toobj(b), var_tostr(c), a);
    } else {
        vm_error(vm, "get field: object error\n");
    }
}

static void i_getmethod(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b) && var_isstring(c)) {
        bvalue self = *b;
        bvalue *m = be_instance_field(var_toobj(b), var_tostr(c), a);
        if (m && m->type != MT_VARIABLE) {
            a[1] = self;
        } else if (var_basetype(a) == BE_FUNCTION) {
            a[1] = *a;
            var_settype(a, NOT_METHOD);
        } else {
            vm_error(vm, "field is not function\n");
        }
    } else {
        vm_error(vm, "get method: error\n");
    }
}

static void i_setfield(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(a) && var_isstring(b)) {
        be_instance_setfield(var_toobj(a), var_tostr(b), c);
    } else {
        vm_error(vm, "set field: object error\n");
    }
}

static void i_getindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b)) {
        bvalue *top = topreg(vm);
        /* get method 'item' */
        be_instance_field(var_toobj(b), be_newstr(vm, "item"), top);
        top[1] = *b; /* move object to argv[0] */
        top[2] = *c; /* move key to argv[1] */
        do_funcvar(vm, top, 2); /* call method 'item' */
        *a = *top; /* copy result to R(A) */
    } else {
        vm_error(vm, "get index: object error\n");
    }
}

static void i_setindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(a)) {
        bvalue *top = topreg(vm);
        /* get method 'item' */
        be_instance_field(var_toobj(a), be_newstr(vm, "setitem"), top);
        top[1] = *a; /* move object to argv[0] */
        top[2] = *b; /* move key to argv[1] */
        top[3] = *c; /* move src to argv[2] */
        do_funcvar(vm, top, 3); /* call method 'setitem' */
    } else {
        vm_error(vm, "set index: object error\n");
    }
}

static void i_setsuper(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins);
    if (var_isclass(a) && var_isclass(b)) {
        be_class_setsuper(cast(bclass*, var_toobj(a)), var_toobj(b));
    } else {
        vm_error(vm, "set super: class error\n");
    }
}

static void i_close(bvm *vm, binstruction ins)
{
    be_upvals_close(vm, RA(ins));
}

bvm* be_newvm(int nstack)
{
    bvm *vm = be_malloc(sizeof(bvm));
    be_gc_init(vm);
    be_string_init(vm);
    be_globalvar_init(vm);
    vm->stack = be_malloc(sizeof(bvalue) * nstack);
    vm->callstack = be_stack_new(sizeof(bcallframe));
    vm->cf = NULL;
    vm->upvalist = NULL;
    vm->reg = vm->stack;
    vm->top = vm->reg;
    return vm;
}

void be_exec(bvm *vm)
{
    bcallframe *cf;
    binstruction ins;
    vm->cf->status |= BASE_FRAME;
    newframe:
    cf = vm->cf;
    for (;;) {
        ins = *cf->ip;
        switch (IGET_OP(ins)) {
        case OP_LDNIL: i_ldnil(vm, ins); break;
        case OP_LDBOOL: i_ldbool(vm, ins); break;
        case OP_LDINT: i_ldint(vm, ins); break;
        case OP_LDCONST: i_ldconst(vm, ins); break;
        case OP_GETGBL: i_getglobal(vm, ins); break;
        case OP_SETGBL: i_setglobal(vm, ins); break;
        case OP_GETUPV: i_getupval(vm, ins); break;
        case OP_SETUPV: i_setupval(vm, ins); break;
        case OP_MOVE: i_move(vm, ins); break;
        case OP_ADD: i_add(vm, ins); break;
        case OP_SUB: i_sub(vm, ins); break;
        case OP_MUL: i_mul(vm, ins); break;
        case OP_DIV: i_div(vm, ins); break;
        case OP_MOD: i_mod(vm, ins); break;
        case OP_LT: i_lt(vm, ins); break;
        case OP_LE: i_le(vm, ins); break;
        case OP_EQ: i_eq(vm, ins); break;
        case OP_NE: i_ne(vm, ins); break;
        case OP_GT: i_gt(vm, ins); break;
        case OP_GE: i_ge(vm, ins); break;
        case OP_NEG: i_neg(vm, ins); break;
        case OP_JMP: i_jump(vm, ins); break;
        case OP_JMPT: i_jumptrue(vm, ins); break;
        case OP_JMPF: i_jumpfalse(vm, ins); break;
        case OP_CALL: i_call(vm, ins); goto newframe;
        case OP_CLOSURE: i_closure(vm, ins); break;
        case OP_GETMBR: i_getfield(vm, ins); break;
        case OP_GETMET: i_getmethod(vm, ins); break;
        case OP_SETMBR: i_setfield(vm, ins); break;
        case OP_GETIDX: i_getindex(vm, ins); break;
        case OP_SETIDX: i_setindex(vm, ins); break;
        case OP_SETSUPER: i_setsuper(vm, ins); break;
        case OP_CLOSE: i_close(vm, ins); break;
        case OP_RET: i_return(vm, ins); goto retpoint;
        default: retpoint:
            if (vm->cf == NULL) {
                bstack *cs = vm->callstack;
                if (!be_stack_isempty(cs)) {
                    vm->cf = be_stack_top(cs);
                }
                return;
            }
            cf = vm->cf;
            break;
        }
        ++cf->ip;
    }
}

void be_dofunc(bvm *vm, bclosure *cl, int argc)
{
    be_gc_setpause(vm, 0);
    do_closure(vm, vm->top, cl, argc);
    be_gc_collect(vm);
}

void be_dontvfunc(bvm *vm, bntvfunc *f, int argc)
{
    do_ntvfunc(vm, vm->top, f, argc);
}

void be_dofuncvar(bvm *vm, bvalue *v, int argc)
{
    do_funcvar(vm, v, argc);
}
