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

#define RA(i)   (vm->cf->reg + IGET_RA(i))
#define RKB(i)  ((isKB(i) ? vm->cf->s.uf.cl->proto->ktab \
                          : vm->cf->reg) + KR2idx(IGET_RKB(i)))
#define RKC(i)  ((isKC(i) ? vm->cf->s.uf.cl->proto->ktab \
                          : vm->cf->reg) + KR2idx(IGET_RKC(i)))

#define isnil(val)      var_isnil(val)
#define isbool(val)     var_isbool(val)
#define isint(val)      (type(val) == BE_INT)
#define isreal(val)     (type(val) == BE_REAL)
#define isstr(val)      (type(val) == BE_STRING)
#define isnumber(val)   (isint(val) || isreal(val))
#define isinstance(val) (type(val) == BE_INSTANCE)
#define toreal(val)     (isreal(val) ? (val)->v.r : (breal)(val)->v.i)
#define setnil(val)     set_type((val), BE_NIL)
#define setint(val, x)  { set_type((val), BE_INT); (val)->v.i = (x); }
#define setreal(val, x) { set_type((val), BE_REAL); (val)->v.r = (x); }
#define setbool(val, x) { set_type((val), BE_BOOL); (val)->v.b = cast_bool(x); }

#define ibinop(op, a, b)    ((a)->v.i op (b)->v.i)

#define topreg(vm)      ((vm)->cf->reg + \
    (vm)->cf->s.uf.cl->proto->nlocal + MAX_TMPREG)

#define define_function(name, block) \
    static void name(bvm *vm, binstruction ins) { \
        block \
    }

#define relop_block(op) \
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins); \
    if (isint(a) && isint(b)) { \
        setbool(dst, ibinop(op, a, b)); \
    } else if (isnumber(a) && isnumber(b)) { \
        breal x = toreal(a), y = toreal(b); \
        setbool(dst, x op y); \
    } else if (isinstance(a)) { \
        object_binop(vm, #op, dst, a, b); \
    } else { \
        vm_error(vm, "a " #op " b param error."); \
    }

#define equal_block(op, opstr) \
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins); \
    if (isint(a) && isint(b)) { \
        setbool(dst, ibinop(op, a, b)); \
    } else if (isnumber(a) && isnumber(b)) { \
        breal x = toreal(a), y = toreal(b); \
        setbool(dst, x op y); \
    } else if (isnil(a) || isnil(b)) { \
        bbool res = type(a) op type(b); \
        setbool(dst, res); \
    } else if (isstr(a) && isstr(b)) { \
        setbool(dst, opstr be_eqstr(a->v.s, b->v.s)); \
    } else if (isinstance(a)) { \
        object_binop(vm, #op, dst, a, b); \
    } else { \
        vm_error(vm, "a " #op " b param error."); \
    }

/* script closure call */
#define push_closure(_vm, _cl, _reg) { \
    bcallframe *_c; \
    be_stack_push(_vm->callstack, NULL); \
    _c = be_stack_top(_vm->callstack); \
    _c->reg = _reg + 1; \
    _c->s.ur.ip = _cl->proto->code; \
    _c->s.uf.cl = _cl; \
    _c->status = NONE_FLAG; \
    _vm->cf = _c; \
}

#define push_ntvfunc(_vm, _f, _reg, _argc) { \
    bcallframe *_c; \
    be_stack_push(_vm->callstack, NULL); \
    _c = be_stack_top(_vm->callstack); \
    _c->reg = _reg + 1; \
    _c->s.ur.top = _c->reg + _argc; \
    _c->s.uf.f = _f; \
    _c->status = PRIM_FUNC; \
    _vm->cf = _c; \
}

#define ret_cfunction(vm) { \
    be_stack_pop(vm->callstack); \
    vm->cf = be_stack_top(vm->callstack); \
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
    push_closure(vm, cl, reg);
    be_exec(vm);
}

void do_ntvfunc(bvm *vm, bvalue *reg, bntvfunc *f, int argc)
{
    if (argc != f->argc && f->argc != -1) {
        vm_error(vm, "function argc error");
    }
    push_ntvfunc(vm, f, reg, argc);
    f->f(vm); /* call C primitive function */
    ret_cfunction(vm);
}

void do_funcvar(bvm *vm, bvalue *reg, bvalue *v, int argc)
{
    switch (v->type) {
    case BE_CLOSURE:
        do_closure(vm, reg, var_toobj(v), argc);
        break;
    case BE_NTVFUNC: {
        do_ntvfunc(vm, reg, var_toobj(v), argc);
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
    do_funcvar(vm, top, top, 1); /* call method 'item' */
    return isbool(top) ? top->v.b : btrue;
}

static bbool var2bool(bvm *vm, bvalue *v)
{
    switch (basetype(v)) {
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
    do_funcvar(vm, top, top, 2); /* call method 'item' */
    *dst = *top; /* copy result to dst */
}

static void object_unop(bvm *vm, const char *op,
                        bvalue *dst, bvalue *src)
{
    bvalue *top = topreg(vm);
    /* get operator method */
    be_instance_field(src->v.p, be_newstr(vm, op), top);
    top[1] = *src; /* move self to argv[0] */
    do_funcvar(vm, top, top, 1); /* call method 'item' */
    *dst = *top; /* copy result to dst */
}

static void i_ldnil(bvm *vm, binstruction ins)
{
    setnil(RA(ins));
}

static void i_ldbool(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    setbool(v, IGET_RKB(ins));
    if (IGET_RKC(ins)) { /* skip next instruction */
        vm->cf->s.ur.ip++;
    }
}

static void i_ldint(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);

    set_type(v, BE_INT);
    v->v.i = IGET_sBx(ins);
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
    *v = *vm->cf->s.uf.cl->upvals[idx]->value;
}

static void i_setupval(bvm *vm, binstruction ins)
{
    bvalue *v = RA(ins);
    int idx = IGET_Bx(ins);
    *vm->cf->s.uf.cl->upvals[idx]->value = *v;
}

static void i_ldconst(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins);
    *dst = vm->cf->s.uf.cl->proto->ktab[IGET_Bx(ins)];
}

static void i_move(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins);
    *dst = *RKB(ins);
}

static void i_add(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (isint(a) && isint(b)) {
        setint(dst, ibinop(+, a, b));
    } else if (isnumber(a) && isnumber(b)) {
        breal x = toreal(a), y = toreal(b);
        setreal(dst, x + y);
    } else if (isinstance(a)) {
        object_binop(vm, "+", dst, a, b);
    } else {
        vm_error(vm, "ADD param error.");
    }
}

static void i_sub(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (isint(a) && isint(b)) {
        setint(dst, ibinop(-, a, b));
    } else if (isnumber(a) && isnumber(b)) {
        breal x = toreal(a), y = toreal(b);
        setreal(dst, x - y);
    } else if (isinstance(a)) {
        object_binop(vm, "-", dst, a, b);
    } else {
        vm_error(vm, "SUB param error.");
    }
}

static void i_mul(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (isint(a) && isint(b)) {
        setint(dst, ibinop(*, a, b));
    } else if (isnumber(a) && isnumber(b)) {
        breal x = toreal(a), y = toreal(b);
        setreal(dst, x * y);
    } else if (isinstance(a)) {
        object_binop(vm, "*", dst, a, b);
    } else {
        vm_error(vm, "MUL param error.");
    }
}

static void i_div(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (isint(a) && isint(b)) {
        setint(dst, ibinop(/, a, b));
    } else if (isnumber(a) && isnumber(b)) {
        breal x = toreal(a), y = toreal(b);
        setreal(dst, x / y);
    } else if (isinstance(a)) {
        object_binop(vm, "/", dst, a, b);
    } else {
        vm_error(vm, "DIV param error.");
    }
}

static void i_mod(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (isint(a) && isint(b)) {
        setint(dst, ibinop(%, a, b));
    } else if (isinstance(a)) {
        object_binop(vm, "%", dst, a, b);
    } else {
        vm_error(vm, "MOD param error.");
    }
}

static void i_neg(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins);
    if (isint(a)) {
        setint(dst, -a->v.i);
    } else if (isreal(a)) {
        setreal(dst, -a->v.r);
    } else if (isinstance(a)) {
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
    cf->s.ur.ip += IGET_sBx(ins);
}

static void i_jumptrue(bvm *vm, binstruction ins)
{
    bcallframe *cf = vm->cf;
    if (var2bool(vm, RA(ins))) {
        cf->s.ur.ip += IGET_sBx(ins);
    }
}

static void i_jumpfalse(bvm *vm, binstruction ins)
{
    bcallframe *cf = vm->cf;
    if (!var2bool(vm, RA(ins))) {
        cf->s.ur.ip += IGET_sBx(ins);
    }
}

static void i_return(bvm *vm, binstruction ins)
{
    bcallframe *cf = be_stack_top(vm->callstack);
    bvalue *ret = cf->reg - 1, *src = RKB(ins);
    be_stack_pop(vm->callstack); /* pop don't delete */
    if (cf->status & BASE_FRAME) { /* entrance function */
        *ret = *src;
        vm->cf = NULL; /* mainfunction return */
    } else {
        ret[type(ret - 1) != BE_NOTMETHOD ? 0 : -1] = *src;
        vm->cf = be_stack_top(vm->callstack);
    }
}

static void i_call(bvm *vm, binstruction ins)
{
    bvalue *var = RA(ins);
    int argc = IGET_RKB(ins);

    recall: /* goto: instantiation class and call constructor */
    switch (type(var)) {
    case BE_NOTMETHOD:
        ++var; --argc;
        goto recall;
    case BE_CLASS:
        if (be_class_newobj(vm, var_toobj(var), var, ++argc)) {
            ++var; /* to next register */
            goto recall; /* call constructor */
        }
        ++vm->cf->s.ur.ip; /* to next instruction */
        break;
    case BE_CLOSURE: {
        bclosure *cl = var->v.p;
        if (argc != cl->proto->argc) {
            vm_error(vm, "function argc error");
        }
        push_closure(vm, cl, var);
        break;
    }
    case BE_NTVFUNC: {
        bntvfunc *f = var->v.p;
        if (argc != f->argc && f->argc != -1) {
            vm_error(vm, "function argc error");
        }
        push_ntvfunc(vm, f, var, argc);
        f->f(vm); /* call C primitive function */
        ret_cfunction(vm);
        ++vm->cf->s.ur.ip; /* to next instruction */
        break;
    }
    default:
        vm_error(vm, "value is not function");
    }
}

static void i_closure(bvm *vm, binstruction ins)
{
    bvalue *reg = RA(ins);
    bproto *p = vm->cf->s.uf.cl->proto->ptab[IGET_Bx(ins)];
    bclosure *cl = be_newclosure(vm, p->nupvals);
    cl->proto = p;
    be_initupvals(vm, cl);
    set_type(reg, BE_CLOSURE);
    reg->v.p = cl;
}

static void i_getfield(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (type(b) == BE_INSTANCE && type(c) == BE_STRING) {
        be_instance_field(b->v.p, c->v.s, a);
    } else {
        vm_error(vm, "get field: object error\n");
    }
}

static void i_getmethod(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (type(b) == BE_INSTANCE && type(c) == BE_STRING) {
        bvalue self = *b;
        bvalue *m = be_instance_field(b->v.p, c->v.s, a);
        if (m && m->type != MT_VARIABLE) {
            a[1] = self;
        } else if (basetype(a) == BE_FUNCTION) {
            a[1] = *a;
            set_type(a, BE_NOTMETHOD);
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
    if (type(a) == BE_INSTANCE && type(b) == BE_STRING) {
        be_instance_setfield(a->v.p, b->v.s, c);
    } else {
        vm_error(vm, "set field: object error\n");
    }
}

static void i_getindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (type(b) == BE_INSTANCE) {
        bvalue *top = topreg(vm);
        /* get method 'item' */
        be_instance_field(b->v.p, be_newstr(vm, "item"), top);
        top[1] = *b; /* move object to argv[0] */
        top[2] = *c; /* move key to argv[1] */
        do_funcvar(vm, top, top, 2); /* call method 'item' */
        *a = *top; /* copy result to R(A) */
    } else {
        vm_error(vm, "get index: object error\n");
    }
}

static void i_setindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (type(a) == BE_INSTANCE) {
        bvalue *top = topreg(vm);
        /* get method 'item' */
        be_instance_field(a->v.p, be_newstr(vm, "setitem"), top);
        top[1] = *a; /* move object to argv[0] */
        top[2] = *b; /* move key to argv[1] */
        top[3] = *c; /* move src to argv[2] */
        do_funcvar(vm, top, top, 3); /* call method 'setitem' */
    } else {
        vm_error(vm, "set index: object error\n");
    }
}

static void i_setsuper(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins);
    if (type(a) == BE_CLASS && type(b) == BE_CLASS) {
        be_class_setsuper((bclass*)a->v.p, b->v.p);
    } else {
        vm_error(vm, "set super: class error\n");
    }
}

static void i_close(bvm *vm, binstruction ins)
{
    be_upvals_close(vm, RA(ins));
}

bvm* be_vm_new(int nstack)
{
    bvm *vm = be_malloc(sizeof(bvm));
    be_gc_init(vm);
    be_string_init(vm);
    be_globalvar_init(vm);
    vm->stack = be_malloc(sizeof(bvalue) * nstack);
    vm->callstack = be_stack_new(sizeof(bcallframe));
    vm->cf = NULL;
    vm->upvalist = NULL;
    vm->spos = 0;
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
        ins = *cf->s.ur.ip;
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
        ++cf->s.ur.ip;
    }
}

void be_dofunc(bvm *vm, bclosure *cl, int argc)
{
    bvalue *reg = vm->cf ? vm->cf->s.ur.top : vm->stack;
    be_gc_setpause(vm, 0);
    do_closure(vm, reg, cl, argc);
    be_gc_collect(vm);
}

void be_dontvfunc(bvm *vm, bntvfunc *f, int argc)
{
    bvalue *reg = vm->cf ? vm->cf->s.ur.top : vm->stack;
    do_ntvfunc(vm, reg, f, argc);
}

void be_dofuncvar(bvm *vm, bvalue *v, int argc)
{
    bvalue *reg = vm->cf ? vm->cf->s.ur.top : vm->stack;
    do_funcvar(vm, reg, v, argc);
}
