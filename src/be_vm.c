#include "be_vm.h"
#include "be_opcode.h"
#include "be_string.h"
#include "be_strlib.h"
#include "be_class.h"
#include "be_func.h"
#include "be_vector.h"
#include "be_map.h"
#include "be_module.h"
#include "be_mem.h"
#include "be_var.h"
#include "be_gc.h"
#include "be_exec.h"
#include "be_debug.h"

#include <stdio.h>
#include <string.h>

#define NOT_METHOD      BE_NONE

#define vm_error(vm, fmt, ...) \
    be_debug_error(vm, BE_EXEC_ERROR, be_pushfstring(vm, fmt, __VA_ARGS__))

#define RA(i)   (vm->reg + IGET_RA(i))
#define RKB(i)  ((isKB(i) ? curcl(vm)->proto->ktab \
                          : vm->reg) + KR2idx(IGET_RKB(i)))
#define RKC(i)  ((isKC(i) ? curcl(vm)->proto->ktab \
                          : vm->reg) + KR2idx(IGET_RKC(i)))

#define var2real(_v) \
    (var_isreal(_v) ? (_v)->v.r : (breal)(_v)->v.i)

#define val2bool(v)        ((v) ? btrue : bfalse)
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
    } else if (var_isstr(a) && var_isstr(b)) { \
        bstring *s1 = var_tostr(a), *s2 = var_tostr(b); \
        int res = be_strcmp(s1, s2); \
        var_setbool(dst, res op 0); \
    } else if (var_isinstance(a)) { \
        object_binop(vm, #op, ins, a, b); \
    } else { \
        binop_error(vm, #op, a, b); \
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
        int res = var_type(a) op var_type(b); \
        var_setbool(dst, res); \
    } else if (var_isstr(a) && var_isstr(b)) { \
        var_setbool(dst, opstr be_eqstr(a->v.s, b->v.s)); \
    } else if (var_isinstance(a)) { \
        object_binop(vm, #op, ins, a, b); \
    } else { \
        binop_error(vm, #op, a, b); \
    }

/* script closure call */
#define push_closure(_vm, _f, _ns, _t) { \
    bclosure *cl = var_toobj(_f); \
    precall(_vm, _f, _ns, _t); \
    _vm->cf->ip = _vm->ip; \
    _vm->cf->status = NONE_FLAG; \
	_vm->ip = cl->proto->code; \
}

#define push_native(_vm, _f, _ns, _t) { \
    precall(_vm, _f, _ns, _t); \
    _vm->cf->status = PRIM_FUNC; \
}

#define ret_native(_vm) { \
    bcallframe *_cf = _vm->cf; \
    _vm->reg = _cf->reg; \
    _vm->top = _cf->top; \
    be_stack_pop(&_vm->callstack); \
    _vm->cf = be_stack_top(&_vm->callstack); \
}

#define attribute_error(vm, b, c, t) {          \
    const char *attr = var_isstr(c) ?           \
        str(var_tostr(c)) : be_vtype2str(c);    \
    vm_error(vm,                                \
        "'%s' value has no " t " '%s'",         \
        be_vtype2str(b), attr);                 \
}

static void binop_error(bvm *vm, const char *op, bvalue *a, bvalue *b)
{
    vm_error(vm,
        "unsupported operand type(s) for %s: '%s' and '%s'",
        op, be_vtype2str(a), be_vtype2str(b));
}

static void unop_error(bvm *vm, const char *op, bvalue *a)
{
    vm_error(vm,
        "unsupported operand type(s) for %s: '%s'",
        op, be_vtype2str(a));
}

static void precall(bvm *vm, bvalue *func, int nstack, int mode)
{
    bcallframe *cf;
    int expan = nstack + BE_STACK_FREE_MIN;
    if (vm->stacktop < func + expan) {
		size_t fpos = func - vm->stack;
        be_stack_expansion(vm, expan);
		func = vm->stack + fpos;
    }
    be_stack_push(&vm->callstack, NULL);
    cf = be_stack_top(&vm->callstack);
    cf->func = func - mode;
    cf->top = vm->top;
    cf->reg = vm->reg;
    vm->reg = func + 1;
    vm->top = vm->reg + nstack;
    vm->cf = cf;
}

static bbool obj2bool(bvm *vm, bvalue *obj)
{
    bvalue *top = vm->top;
    bstring *tobool = be_newconststr(vm, "tobool");
    /* get operator method */
    if (be_instance_member(obj->v.p, tobool, top)) {
        top[1] = *obj; /* move self to argv[0] */
        be_dofunc(vm, top, 1); /* call method 'tobool' */
        top = vm->top;
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
        return val2bool(v->v.i);
    case BE_REAL:
        return val2bool(v->v.r);
    case BE_INSTANCE:
        return obj2bool(vm, v);
    default:
        return btrue;
    }
}

static void obj_method(bvm *vm, bvalue *o, bstring *attr)
{
    binstance *obj = var_toobj(o);
    int type = be_instance_member(obj, attr, vm->top);
    if (basetype(type) != BE_FUNCTION) {
        vm_error(vm,
            "the class '%s' has no method '%s'",
            str(be_instance_name(obj)), str(attr));
    }
}

static int obj_attribute(bvm *vm, bvalue *o, bstring *attr, bvalue *dst)
{
    binstance *obj = var_toobj(o);
    int type = be_instance_member(obj, attr, dst);
    if (basetype(type) == BE_NIL) {
        vm_error(vm,
            "the class '%s' has no attribute '%s'",
            str(be_instance_name(obj)), str(attr));
    }
    return type;
}

static void object_binop(bvm *vm,
    const char *op, binstruction ins, bvalue *a, bvalue *b)
{
    bvalue *top = vm->top;
    /* get operator method */
    obj_method(vm, a, be_newconststr(vm, op));
    top[1] = *a; /* move self to argv[0] */
    top[2] = *b; /* move other to argv[1] */
    be_incrtop(vm); /* prevent collection results */
    be_dofunc(vm, top, 2); /* call method 'item' */
    be_stackpop(vm, 1);
    *RA(ins) = *vm->top; /* copy result to dst */
}
static void object_unop(bvm *vm,
    const char *op, binstruction ins, bvalue *src)
{
    bvalue *top = vm->top;
    /* get operator method */
    obj_method(vm, src, be_newconststr(vm, op));
    top[1] = *src; /* move self to argv[0] */
    be_dofunc(vm, top, 1); /* call method 'item' */
    *RA(ins) = *vm->top; /* copy result to dst */
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
        vm->ip++;
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
        object_binop(vm, "+", ins, a, b);
    } else {
        binop_error(vm, "+", a, b);
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
        object_binop(vm, "-", ins, a, b);
    } else {
        binop_error(vm, "-", a, b);
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
        object_binop(vm, "*", ins, a, b);
    } else {
        binop_error(vm, "*", a, b);
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
        object_binop(vm, "/", ins, a, b);
    } else {
        binop_error(vm, "/", a, b);
    }
}

static void i_mod(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *a = RKB(ins), *b = RKC(ins);
    if (var_isint(a) && var_isint(b)) {
        var_setint(dst, ibinop(%, a, b));
    } else if (var_isinstance(a)) {
        object_binop(vm, "%", ins, a, b);
    } else {
        binop_error(vm, "%", a, b);
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
        object_unop(vm, "-*", ins, a);
    } else {
        unop_error(vm, "-", a);
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
    bvalue *b = RKB(ins), *c = RKC(ins);
    bvalue *top = vm->top;
    /* get method 'item' */
    int idx = be_globalvar_find(vm, be_newconststr(vm, "range"));
    top[0] = vm->global[idx];
    top[1] = *b; /* move lower to argv[0] */
    top[2] = *c; /* move upper to argv[1] */
    vm->top += 3; /* prevent collection results */
    be_dofunc(vm, top, 2); /* call method 'item' */
    vm->top -= 3;
    *RA(ins) = *vm->top; /* copy result to R(A) */
}

static void i_jump(bvm *vm, binstruction ins)
{
    vm->ip += IGET_sBx(ins);
}

static void i_jumptrue(bvm *vm, binstruction ins)
{
    if (var2bool(vm, RA(ins))) {
		vm->ip += IGET_sBx(ins);
    }
}

static void i_jumpfalse(bvm *vm, binstruction ins)
{
    if (!var2bool(vm, RA(ins))) {
		vm->ip += IGET_sBx(ins);
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
    vm->reg = cf->reg;
    vm->top = cf->top;
	vm->ip = cf->ip;
    be_stack_pop(&vm->callstack); /* pop don't delete */
    if (cf->status & BASE_FRAME) { /* entrance function */
        vm->cf = NULL; /* mainfunction return */
    } else {
        vm->cf = be_stack_top(&vm->callstack);
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
        ++vm->ip; /* to next instruction */
        break;
    case BE_CLOSURE: {
        bvalue *v, *end;
        bproto *proto = var2cl(var)->proto;
        push_closure(vm, var, proto->nstack, mode);
        v = vm->reg + argc;
        end = vm->reg + proto->argc;
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
        ++vm->ip; /* to next instruction */
        break;
    }
    case BE_NTVFUNC: {
        bntvfunc f = var_tontvfunc(var);
        push_native(vm, var, argc, mode);
        f(vm); /* call C primitive function */
        ret_native(vm);
        ++vm->ip; /* to next instruction */
        break;
    }
    default:
        vm_error(vm, "'%s' value is not callable",
            be_vtype2str(var));
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
        obj_attribute(vm, b, var_tostr(c), a);
    } else if (var_ismodule(b) && var_isstr(c)) {
        bstring *attr = var_tostr(c);
        bmodule *module = var_toobj(b);
        bvalue *v = be_module_attr(module, attr);
        if (v) {
            *a = *v;
        } else {
            vm_error(vm, "module '%s' has no attribute '%s'",
                be_module_name(module), str(attr));
        }
    } else {
        attribute_error(vm, b, c, "attribute");
    }
}

static void i_getmethod(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b) && var_isstr(c)) {
        bvalue self = *b;
        bstring *attr = var_tostr(c);
        binstance *obj = var_toobj(b);
        int type = obj_attribute(vm, b, var_tostr(c), a);
        if (type == MT_METHOD || type == MT_PRIMMETHOD) {
            a[1] = self;
        } else if (var_basetype(a) == BE_FUNCTION) {
            a[1] = *a;
            var_settype(a, NOT_METHOD);
        } else {
            vm_error(vm, "class '%s' has no method '%s'",
                str(be_instance_name(obj)), str(attr));
        }
    } else if (var_ismodule(b) && var_isstr(c)) {
        bstring *attr = var_tostr(c);
        bmodule *module = var_toobj(b);
        bvalue *src = be_module_attr(module, attr);
        if (src) {
            var_settype(a, NOT_METHOD);
            a[1] = *src;
        } else {
            vm_error(vm, "module '%s' has no method '%s'",
                be_module_name(module), str(attr));
        }
    } else {
        attribute_error(vm, b, c, "method");
    }
}

static void i_setmember(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(a) && var_isstr(b)) {
        binstance *obj = var_toobj(a);
        bstring *attr = var_tostr(b);
        if (!be_instance_setmember(obj, attr, c)) {
            vm_error(vm, "class '%s' cannot assign to attribute '%s'",
                str(be_instance_name(obj)), str(attr));
        }
    } else {
        attribute_error(vm, b, c, "attribute");
    }
}

static void i_getindex(bvm *vm, binstruction ins)
{
    bvalue *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(b)) {
        bvalue *top = vm->top;
        /* get method 'item' */
        obj_method(vm, b, be_newconststr(vm, "item"));
        top[1] = *b; /* move object to argv[0] */
        top[2] = *c; /* move key to argv[1] */
        vm->top += 3;   /* prevent collection results */
        be_dofunc(vm, top, 2); /* call method 'item' */
        vm->top -= 3;
        *RA(ins) = *vm->top;   /* copy result to R(A) */
    } else {
        vm_error(vm,
            "value '%s' does not support index operation",
            be_vtype2str(b));
    }
}

static void i_setindex(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins), *c = RKC(ins);
    if (var_isinstance(a)) {
        bvalue *top = vm->top;
        /* get method 'setitem' */
        obj_method(vm, a, be_newconststr(vm, "setitem"));
        top[1] = *a; /* move object to argv[0] */
        top[2] = *b; /* move key to argv[1] */
        top[3] = *c; /* move src to argv[2] */
        vm->top += 4;
        be_dofunc(vm, top, 3); /* call method 'setitem' */
        vm->top -= 4;
    } else {
        vm_error(vm,
            "value '%s' does not support index operation",
            be_vtype2str(b));
    }
}

static void i_setsuper(bvm *vm, binstruction ins)
{
    bvalue *a = RA(ins), *b = RKB(ins);
    if (var_isclass(a) && var_isclass(b)) {
        bclass *obj = var_toobj(a);
        be_class_setsuper(obj, var_toobj(b));
    } else {
        vm_error(vm,
            "value '%s' does not support set super",
            be_vtype2str(b));
    }
}

static void i_close(bvm *vm, binstruction ins)
{
    be_upvals_close(vm, RA(ins));
}

static void i_import(bvm *vm, binstruction ins)
{
    bvalue *dst = RA(ins), *b = RKB(ins);
    if (var_isstr(b)) {
        bmodule *m = be_module_load(vm, var_tostr(b), dst);
        if (m == NULL) {
            vm_error(vm, "module '%s' does not found",
                str(var_tostr(b)));
        }
    } else {
        vm_error(vm,
            "import '%s' does not support import",
            be_vtype2str(b));
    }
}

bvm* be_vm_new(void)
{
    bvm *vm = be_malloc(sizeof(bvm));
    be_gc_init(vm);
    be_string_init(vm);
    be_globalvar_init(vm);
    be_stack_init(&vm->callstack, sizeof(bcallframe));
    be_stack_init(&vm->refstack, sizeof(binstance*));
    vm->stack = be_malloc(sizeof(bvalue) * BE_STACK_FREE_MIN);
    vm->stacktop = vm->stack + BE_STACK_FREE_MIN;
    vm->cf = NULL;
	vm->ip = NULL;
    vm->upvalist = NULL;
    vm->reg = vm->stack;
    vm->top = vm->reg;
    vm->errjmp = NULL;
    vm->modulelist = NULL;
    return vm;
}

void be_vm_delete(bvm *vm)
{
    be_gc_deleteall(vm);
    be_string_deleteall(vm);
    be_stack_delete(&vm->callstack);
    be_stack_delete(&vm->refstack);
    be_free(vm->stack);
    be_globalvar_deinit(vm);
    be_free(vm);
}

static void vm_exec(bvm *vm)
{
    binstruction ins;
    vm->cf->status |= BASE_FRAME;
    newframe:
    for (;;) {
        ins = *vm->ip;
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
        case OP_IMPORT: i_import(vm, ins); break;
        case OP_RET: i_return(vm, ins); goto retpoint;
        default: retpoint:
            if (vm->cf == NULL) {
                bstack *cs = &vm->callstack;
                if (!be_stack_isempty(cs)) {
                    vm->cf = be_stack_top(cs);
                }
                return;
            }
            break;
        }
        ++vm->ip;
    }
}

static void do_closure(bvm *vm, bvalue *reg, int argc)
{
    bvalue *v, *end;
    bproto *proto = var2cl(reg)->proto;
    push_closure(vm, reg, proto->nstack, 0);
    v = vm->reg + argc;
    end = vm->reg + proto->argc;
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
    bntvfunc f = var_tontvfunc(reg);
    push_native(vm, reg, argc, 0);
    f(vm); /* call C primitive function */
    ret_native(vm);
}

static void do_class(bvm *vm, bvalue *reg, int argc)
{
    if (be_class_newobj(vm, var_toobj(reg), reg, ++argc)) {
        be_incrtop(vm);
        be_dofunc(vm, reg + 1, argc);
        be_stackpop(vm, 1);
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
}
