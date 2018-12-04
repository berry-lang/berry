#include "be_vm.h"
#include "be_opcode.h"
#include "be_string.h"
#include "be_strlib.h"
#include "be_class.h"
#include "be_func.h"
#include "be_vector.h"
#include "be_map.h"
#include "be_mem.h"
#include "be_var.h"
#include "be_gc.h"
#include "be_exec.h"
#include "be_debug.h"

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
    } else if (var_isbool(a) || var_isbool(b)) { \
        var_setbool(dst, var_tobool(a) op var_tobool(b)); \
    } else if (var_isnil(a) || var_isnil(b)) { \
        bbool res = var_type(a) op var_type(b); \
        var_setbool(dst, res); \
    } else if (var_isstr(a) && var_isstr(b)) { \
        var_setbool(dst, opstr be_eqstr(a->v.s, b->v.s)); \
    } else if (var_isinstance(a)) { \
        object_binop(vm, #op, dst, a, b); \
    } else { \
        vm_error(vm, "a " #op " b param error."); \
    }

/* script closure call */
#define push_closure(_vm, _f, _ns, _t) { \
    bcallframe *_cf; \
    be_stack_push(_vm->callstack, NULL); \
    _cf = be_stack_top(_vm->callstack); \
    _cf->func = _t ? _f - 1 : _f; \
    _cf->top = _vm->top; \
    _cf->reg = _vm->reg; \
    _cf->ip = var2cl(_f)->proto->code; \
    _cf->status = NONE_FLAG; \
    _vm->reg = _f + 1; \
    _vm->top = _f + 1 + _ns; \
    _vm->cf = _cf; \
}

#define push_native(_vm, _f, _ns, _t) { \
    bcallframe *_cf; \
    be_stack_push(_vm->callstack, NULL); \
    _cf = be_stack_top(_vm->callstack); \
    _cf->func = _t ? _f - 1 : _f; \
    _cf->top = _vm->top; \
    _cf->reg = _vm->reg; \
    _cf->status = PRIM_FUNC; \
    _vm->reg = _f + 1; \
    _vm->top = _f + 1 + _ns; \
    _vm->cf = _cf; \
}

#define ret_native(_vm) { \
    bcallframe *_cf = _vm->cf; \
    be_stack_pop(_vm->callstack); \
    _vm->cf = be_stack_top(_vm->callstack); \
    _vm->reg = _cf->reg; \
    _vm->top = _cf->top; \
}

static void vm_error(bvm *vm, const char *msg)
{
    be_pushfstring(vm, "%s", msg);
    be_throw(vm, 1);
}

static bbool obj2bool(bvm *vm, bvalue *obj)
{
    bvalue *top = vm->top;
    bstring *tobool = be_newconststr(vm, "tobool");
    /* get operator method */
    if (be_instance_member(obj->v.p, tobool, top)) {
        top[1] = *obj; /* move self to argv[0] */
        be_dofunc(vm, top, 1); /* call method 'tobool' */
        return var_isbool(top) ? var_tobool(top) : btrue;
    }
    return btrue;
}

static bbool var2bool(bvm *vm, bvalue *v)
{
    switch (var_basetype(v)) {
    case BE_NIL:
        return bfalse;
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
    bvalue *top = vm->top;
    /* get operator method */
    be_instance_member(a->v.p, be_newconststr(vm, op), top);
    top[1] = *a; /* move self to argv[0] */
    top[2] = *b; /* move other to argv[1] */
    vm->top++;   /* prevent collection results */
    be_dofunc(vm, top, 2); /* call method 'item' */
    vm->top--;
    *dst = *top; /* copy result to dst */
}
static void object_unop(bvm *vm, const char *op,
                        bvalue *dst, bvalue *src)
{
    bvalue *top = vm->top;
    /* get operator method */
    be_instance_member(src->v.p, be_newconststr(vm, op), top);
    top[1] = *src; /* move self to argv[0] */
    be_dofunc(vm, top, 1); /* call method 'item' */
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
    } else if (var_isstr(a) && var_isstr(b)) { /* strcat */
        bstring *s = be_strcat(vm, var_tostr(a), var_tostr(b));
        var_setstr(dst, s);
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

static void i_range(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    bvalue *top = vm->top;
    /* get method 'item' */
    int idx = be_globalvar_find(vm, be_newconststr(vm, "range"));
    top[0] = vm->global[idx];
    top[1] = *b; /* move lower to argv[0] */
    top[2] = *c; /* move upper to argv[1] */
    vm->top += 3; /* prevent collection results */
    be_dofunc(vm, top, 2); /* call method 'item' */
    vm->top -= 3;
    *a = *top; /* copy result to R(A) */
}

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
    bvalue *ret = vm->cf->func;
    /* copy return value */
    if (IGET_RA(ins)) {
        *ret = *RKB(ins);
    } else {
        var_setnil(ret);
    }
    be_stack_pop(vm->callstack); /* pop don't delete */
    vm->reg = cf->reg;
    vm->top = cf->top;
    if (cf->status & BASE_FRAME) { /* entrance function */
        vm->cf = NULL; /* mainfunction return */
    } else {
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
        bproto *proto = var2cl(var)->proto;
        bvalue *v = var + argc + 1, *end = var + proto->argc + 1;
        push_closure(vm, var, proto->nstack, mode);
        for (; v <= end; ++v) {
            var_setnil(v);
        }
        break;
    }
    case BE_NTVCLOS: {
        bntvclos *f = var_toobj(var);
        push_native(vm, var, argc, mode);
        f->f(vm); /* call C primitive function */
        ret_native(vm);
        ++vm->cf->ip; /* to next instruction */
        break;
    }
    case BE_NTVFUNC: {
        bcfunction f = cast(bcfunction, var_toobj(var));
        push_native(vm, var, argc, mode);
        f(vm); /* call C primitive function */
        ret_native(vm);
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

static void i_getmember(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b) && var_isstr(c)) {
        be_instance_member(var_toobj(b), var_tostr(c), a);
    } else {
        vm_error(vm, "get member: object error");
    }
}

static void i_getmethod(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b) && var_isstr(c)) {
        bvalue self = *b;
        bvalue *m = be_instance_member(var_toobj(b), var_tostr(c), a);
        if (m && m->type != MT_VARIABLE) {
            a[1] = self;
        } else if (var_basetype(a) == BE_FUNCTION) {
            a[1] = *a;
            var_settype(a, NOT_METHOD);
        } else {
            vm_error(vm, "member is not function");
        }
    } else {
        vm_error(vm, "get method: error");
    }
}

static void i_setmember(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(a) && var_isstr(b)) {
        be_instance_setmember(var_toobj(a), var_tostr(b), c);
    } else {
        vm_error(vm, "set member: object error");
    }
}

static void i_getindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b)) {
        bvalue *top = vm->top;
        /* get method 'item' */
        be_instance_member(var_toobj(b), be_newconststr(vm, "item"), top);
        top[1] = *b; /* move object to argv[0] */
        top[2] = *c; /* move key to argv[1] */
        vm->top += 3;   /* prevent collection results */
        be_dofunc(vm, top, 2); /* call method 'item' */
        vm->top -= 3;
        *a = *top;   /* copy result to R(A) */
    } else {
        vm_error(vm, "get index: object error");
    }
}

static void i_setindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(a)) {
        bvalue *top = vm->top;
        /* get method 'item' */
        be_instance_member(var_toobj(a), be_newconststr(vm, "setitem"), top);
        top[1] = *a; /* move object to argv[0] */
        top[2] = *b; /* move key to argv[1] */
        top[3] = *c; /* move src to argv[2] */
        vm->top += 4;
        be_dofunc(vm, top, 3); /* call method 'setitem' */
        vm->top -= 4;
    } else {
        vm_error(vm, "set index: object error");
    }
}

static void i_setsuper(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins);
    if (var_isclass(a) && var_isclass(b)) {
        be_class_setsuper(cast(bclass*, var_toobj(a)), var_toobj(b));
    } else {
        vm_error(vm, "set super: class error");
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
    vm->stacksize = nstack;
    vm->cf = NULL;
    vm->upvalist = NULL;
    vm->reg = vm->stack;
    vm->top = vm->reg;
    vm->errjmp = NULL;
    return vm;
}

static void vm_exec(bvm *vm)
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
        case OP_RANGE: i_range(vm, ins); break;
        case OP_NEG: i_neg(vm, ins); break;
        case OP_JMP: i_jump(vm, ins); break;
        case OP_JMPT: i_jumptrue(vm, ins); break;
        case OP_JMPF: i_jumpfalse(vm, ins); break;
        case OP_CALL: i_call(vm, ins); goto newframe;
        case OP_CLOSURE: i_closure(vm, ins); break;
        case OP_GETMBR: i_getmember(vm, ins); break;
        case OP_GETMET: i_getmethod(vm, ins); break;
        case OP_SETMBR: i_setmember(vm, ins); break;
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

static void do_closure(bvm *vm, bvalue *reg, int argc)
{
    bproto *proto = var2cl(reg)->proto;
    bvalue *v = reg + argc + 1, *end = reg + proto->argc + 1;
    push_closure(vm, reg, proto->nstack, 0);
    for (; v <= end; ++v) {
        var_setnil(v);
    }
    vm_exec(vm);
}

static void do_ntvclos(bvm *vm, bvalue *reg, int argc)
{
    bntvclos *f = var_toobj(reg);
    push_native(vm, reg, argc, 0);
    f->f(vm); /* call C primitive function */
    ret_native(vm);
}

static void do_ntvfunc(bvm *vm, bvalue *reg, int argc)
{
    bcfunction f = cast(bcfunction, var_toobj(reg));
    push_native(vm, reg, argc, 0);
    f(vm); /* call C primitive function */
    ret_native(vm);
}

static void do_class(bvm *vm, bvalue *reg, int argc)
{
    if (be_class_newobj(vm, var_toobj(reg), reg, ++argc)) {
        vm->top++;
        be_dofunc(vm, reg + 1, argc);
        vm->top--;
    }
}

void be_dofunc(bvm *vm, bvalue *v, int argc)
{
    be_gc_setpause(vm, 1);
    switch (var_type(v)) {
    case BE_CLASS: do_class(vm, v, argc); break;
    case BE_CLOSURE: do_closure(vm, v, argc); break;
    case BE_NTVCLOS: do_ntvclos(vm, v, argc); break;
    case BE_NTVFUNC: do_ntvfunc(vm, v, argc); break;
    default: break;
    }
    be_gc_collect(vm);
}
