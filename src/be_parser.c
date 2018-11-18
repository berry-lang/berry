#include "be_parser.h"
#include "be_lexer.h"
#include "be_vector.h"
#include "be_mem.h"
#include "be_vm.h"
#include "be_map.h"
#include "be_list.h"
#include "be_var.h"
#include "be_code.h"
#include "be_string.h"
#include "be_func.h"
#include "be_class.h"
#include "be_opcode.h"
#include "be_debug.h"
#include "be_exec.h"
#include "be_gc.h"

#define OP_NOT_BINARY           TokenNone
#define OP_NOT_UNARY            TokenNone
#define UNARY_OP_PRIO           8

/* get binary operator priority */
#define binary_op_prio(op)      (binary_op_prio_tab[(int)(op) - OptAdd])

#define scan_next_token(parser) (be_lexer_scan_next(&(parser)->lexer))
#define next_token(parser)      ((parser)->lexer.token)
#define array_count(a)          ((int)(sizeof(a) / sizeof((a)[0])))
#define max(a, b)               ((a) > (b) ? (a) : (b))

#define upval_index(v)          ((v) & 0xFF)
#define upval_target(v)         ((bbyte)(((v) >> 8) & 0xFF))
#define upval_instack(v)        ((bbyte)(((v) >> 16) != 0))
#define upval_desc(i, t, s)     (((i) & 0xFF) | (((t) & 0xFF) << 8) \
                                | (((s) != 0) << 16))

typedef struct {
    blexer lexer;
    bvm *vm;
    bvector *global;
    bfuncinfo *finfo;
} bparser;

static void stmtlist(bparser *parser);
static void block(bparser *parser);
static void expr(bparser *parser, bexpdesc *e);

static const int binary_op_prio_tab[] = {
    6, 6, 7, 7, 7, /* + - * / % */
    5, 5, 5, 5, 5, 5, 4, 3 /* < <= == != > >= && || */
};

void parser_error(bparser *parser, const char *msg, const char *tk)
{
    bvm *vm = parser->vm;
    blexer *lexer = &parser->lexer;
    be_pushfstring(vm, "%s:%d: '%s' %s", lexer->fname, lexer->linepos, tk, msg);
    be_throw(parser->vm, 1);
}

void match_token(bparser *parser, btokentype type)
{
    if (next_token(parser).type != type) {
        bvm *vm = parser->vm;
        blexer *lexer = &parser->lexer;
        btoken *token = &next_token(parser);
        const char *s = be_token2str(type);
        switch (token->type) {
        case TokenString:
        case TokenId:
            be_pushfstring(vm, "%s:%d: error: expected '%s' before '%s'",
                lexer->fname, lexer->linepos, s, str(token->u.s));
            break;
        case TokenInteger:
            be_pushfstring(vm, "%s:%d: error: expected '%s' before '%d'",
                lexer->fname, lexer->linepos, s, token->u.i);
            break;
        case TokenReal:
            be_pushfstring(vm, "%s:%d: error: expected '%s' before '%g'",
                lexer->fname, lexer->linepos, s, token->u.r);
            break;
        default:
            be_pushfstring(vm, "%s:%d: error: expected '%s' before '%s'",
                lexer->fname, lexer->linepos, s, be_token2str(token->type));
            break;
        }
        be_throw(parser->vm, 1);
    }
    scan_next_token(parser);
}

void match_notoken(bparser *parser, btokentype type)
{
    if (next_token(parser).type == type) {
        bvm *vm = parser->vm;
        blexer *lexer = &parser->lexer;
        be_pushfstring(vm, "%s:%d: error: expected expression before '%s'",
                lexer->fname, lexer->linepos,
                be_token2str(next_token(parser).type));
        be_throw(parser->vm, 1);
    }
}

static void check_vardefine(bparser *parser, bexpdesc *e)
{
    if (e->type == ETVOID) {
        bstring *s = e->v.s;
        parser_error(parser,
            "undeclared (first use in this function)", str(s));
    }
}

static void begin_block(bfuncinfo *finfo, bblockinfo *binfo, int isloop)
{
    binfo->prev = finfo->binfo;
    finfo->binfo = binfo;
    binfo->isloop = (bbyte)isloop;
    binfo->hasupval = 0;
    binfo->nactlocals = (bbyte)be_list_count(finfo->local);
    if (isloop) {
        binfo->beginpc = finfo->pc;
        binfo->breaklist = NO_JUMP;
        binfo->continuelist = NO_JUMP;
    }
}

static void end_block(bparser *parser)
{
    bfuncinfo *finfo = parser->finfo;
    bblockinfo *binfo = finfo->binfo;
    int nlocal = be_list_count(finfo->local);

    if (binfo->isloop) {
        be_code_patchjump(finfo, binfo->breaklist);
        be_code_patchlist(finfo, binfo->continuelist, binfo->beginpc);
    }
    be_code_close(finfo, 0); /* close upvalues */
    be_list_resize(finfo->local, binfo->nactlocals);
    finfo->nlocal = (bbyte)max(finfo->nlocal, nlocal);
    finfo->freereg = binfo->nactlocals;
    finfo->binfo = binfo->prev;
}

static void begin_func(bparser *parser, bfuncinfo *finfo, bblockinfo *binfo)
{
    bvm *vm = parser->vm;
    bproto *proto = be_newproto(parser->vm);
    proto->ktab = NULL;
    proto->nconst = 0;
    finfo->prev = parser->finfo;
    finfo->code = be_vector_new(sizeof(binstruction));
    finfo->local = be_list_new(vm);
    finfo->upval = be_map_new(vm);
    finfo->kvec = be_vector_new(sizeof(bvalue));
    finfo->pvec = be_vector_new(sizeof(bproto*));
    finfo->proto = proto;
    finfo->global = parser->global;
    finfo->freereg = 0;
    finfo->nlocal = 0;
    finfo->nstack = 0;
    finfo->binfo = NULL;
    finfo->pc = 0;
    finfo->jpc = NO_JUMP;
    parser->finfo = finfo;
    begin_block(finfo, binfo, 0);
    be_gc_fix(vm, gc_object(proto));
    be_gc_fix(vm, gc_object(finfo->local));
    be_gc_fix(vm, gc_object(finfo->upval));
}

static void setupvals(bfuncinfo *finfo)
{
    bproto *proto = finfo->proto;
    int size = be_map_size(finfo->upval);
    int nupvals = be_map_count(finfo->upval);
    bmapentry *slots = be_map_slots(finfo->upval);
    bupvaldesc *upvals = be_malloc(sizeof(bupvaldesc) * nupvals);

    for (; size--; slots++) {
        if (!var_isnil(&slots->key)) {
            uint32_t v = (uint32_t)slots->value.v.i;
            int idx = upval_index(v);
            upvals[idx].idx = upval_target(v);
            upvals[idx].instack = upval_instack(v);
        }
    }
    proto->upvals = upvals;
    proto->nupvals = (bbyte)nupvals;
}

static void end_func(bparser *parser)
{
    bvm *vm = parser->vm;
    bfuncinfo *finfo = parser->finfo;
    bproto *proto = finfo->proto;
    binstruction *ins = be_vector_end(finfo->code);

    /* append a return to last code */
    if (!finfo->pc || IGET_OP(*ins) != OP_RET) {
        be_code_ret(finfo, NULL);
    }
    end_block(parser);
    setupvals(finfo);
    proto->codesize = finfo->pc;
    proto->nlocal = finfo->nlocal;
    proto->nstack = finfo->nstack;
    proto->nproto = be_vector_count(finfo->pvec);
    proto->nconst = be_vector_count(finfo->kvec);
    proto->code = be_vector_swap_delete(finfo->code);
    proto->ktab = be_vector_swap_delete(finfo->kvec);
    proto->ptab = be_vector_swap_delete(finfo->pvec);
    parser->finfo = parser->finfo->prev;
    be_gc_unfix(vm, gc_object(finfo->local));
    be_gc_unfix(vm, gc_object(finfo->upval));
    be_gc_collect(vm);
}

static btokentype get_binop(bparser *parser)
{
    btokentype op = next_token(parser).type;
    if (op >= OptAdd && op <= OptOr) {
        return op;
    }
    return OP_NOT_BINARY;
}

static btokentype get_unary_op(bparser *parser)
{
    btokentype op = next_token(parser).type;

    if (op == OptAdd || op == OptSub || op == OptNot) {
        return op; /* operator 'positive', 'negative' and 'not' */
    }
    return OP_NOT_UNARY;
}

static void init_exp(bexpdesc *e, exptype_t type, int i)
{
    e->type = (bbyte)type;
    e->t = NO_JUMP;
    e->f = NO_JUMP;
    e->not = 0;
    e->v.i = i;
}

static int find_localvar(bfuncinfo *finfo, bstring *s)
{
    int i, count = be_list_count(finfo->local);
    bvalue *var = be_list_data(finfo->local);
    for (i = 0; i < count; i++) {
        if (be_eqstr(var[i].v.s, s)) {
            return i;
        }
    }
    return -1; /* not found */
}

static int new_localvar(bfuncinfo *finfo, bstring *s)
{
    int reg = find_localvar(finfo, s);
    if (reg == -1) {
        bvalue *var;
        reg = be_list_count(finfo->local); /* new local index */
        var = be_list_append(finfo->local, NULL);
        var_setstr(var, s);
        be_code_allocregs(finfo, 1); /* use a register */
    }
    return reg;
}

static int find_upval(bfuncinfo *finfo, bstring *s)
{
    bvalue *desc = be_map_findstr(finfo->upval, s);
    if (desc) {
        return upval_index(desc->v.i);
    }
    return -1;
}

static void mark_upval(bfuncinfo *finfo, int level)
{
    bblockinfo *binfo = finfo->prev->binfo;
    while (binfo->nactlocals > level) {
        binfo = binfo->prev;
    }
    binfo->hasupval = 1;
}

static int new_upval(bfuncinfo *finfo, bstring *name, bexpdesc *var)
{
    int index;
    bvalue *desc;
    int target = var->v.idx;
    int instack = var->type == ETLOCAL;
    if (instack) { /* mark upvalue's block */
        mark_upval(finfo, target);
    }
    index = be_map_count(finfo->upval);
    desc = be_map_insertstr(finfo->upval, name, NULL);
    var_setint(desc, upval_desc(index, target, instack));
    return index;
}

static void new_var(bparser *parser, bstring *name, bexpdesc *var)
{
    bfuncinfo *finfo = parser->finfo;
    if (finfo->prev == NULL && finfo->binfo->prev == NULL) {
        init_exp(var, ETGLOBAL, 0);
        var->v.idx = be_globalvar_new(parser->vm, name);
    } else {
        init_exp(var, ETLOCAL, 0);
        var->v.idx = new_localvar(finfo, name);
    }
}

static void new_class(bparser *parser, bstring *name, bclass *c, bexpdesc *e)
{
    bvalue *var;
    bfuncinfo *finfo = parser->finfo;

    if (finfo->prev == NULL && finfo->binfo->prev == NULL) {
        init_exp(e, ETGLOBAL, 0);
        e->v.idx = be_globalvar_new(parser->vm, name);
        var = be_globalvar(parser->vm, e->v.idx);
    } else {
        init_exp(e, ETLOCAL, 0);
        e->v.idx = new_localvar(finfo, name);
        var = be_code_localobject(finfo, e->v.idx);
    }
    var_settype(var, BE_CLASS);
    var->v.p = c;
}

static int singlevaraux(bvm *vm, bfuncinfo *finfo, bstring *s, bexpdesc *var)
{
    if (finfo == NULL) {
        return ETVOID;
    } else {
        int idx = find_localvar(finfo, s);
        if (idx >= 0) { /* local variable */
            init_exp(var, ETLOCAL, 0);
            var->v.idx = idx;
            return ETLOCAL;
        } else {
            idx = find_upval(finfo, s);
            if (idx < 0) {
                /* find the previous scope  */
                int res = singlevaraux(vm, finfo->prev, s, var);
                if (res == ETUPVAL || res == ETLOCAL) {
                    idx = new_upval(finfo, s, var); /* new upvalue */
                } else if (be_globalvar_find(vm, s) >= 0) {
                    return ETGLOBAL; /* global variable */
                } else {
                    return ETVOID; /* unknow (new variable or error) */
                }
            }
            init_exp(var, ETUPVAL, idx);
            return ETUPVAL;
        }
    }
}

static void singlevar(bparser *parser, bexpdesc *var)
{
    bstring *varname = next_token(parser).u.s;
    int type = singlevaraux(parser->vm, parser->finfo, varname, var);
    switch (type) {
    case ETVOID:
        init_exp(var, ETVOID, 0);
        var->v.s = varname;
        break;
    case ETGLOBAL:
        init_exp(var, ETGLOBAL, 0);
        var->v.idx = be_globalvar_find(parser->vm, varname);
        break;
    default:
        break;
    }
}

static void new_primtype(bparser *parser, const char *type, bexpdesc *e)
{
    int idx;
    bvm *vm = parser->vm;
    bfuncinfo *finfo = parser->finfo;

    idx = be_globalvar_find(vm, be_newstr(vm, type));
    init_exp(e, ETGLOBAL, idx);
    idx = be_code_nextreg(finfo, e);
    be_code_call(finfo, idx, 0);
    e->type = ETREG;
}

static void list_nextfield(bparser *parser, bexpdesc *l)
{
    bexpdesc e, key, v = *l;
    bfuncinfo *finfo = parser->finfo;
    int base, reg = finfo->freereg;

    init_exp(&key, ETSTRING, 0);
    key.v.s = be_newstr(parser->vm, "append");
    /* copy list object to next register */
    be_code_reg(finfo, &v, reg);
    be_code_field(finfo, &v, &key);
    base = be_code_nextreg(finfo, &v);
    be_code_getmethod(finfo);
    /* copy source value to next register */
    expr(parser, &e);
    be_code_nextreg(finfo, &e);
    be_code_call(finfo, base, 2);
    be_code_freeregs(finfo, finfo->freereg - reg);
}

static void list_fields(bparser *parser, bexpdesc *l)
{
    /* expr {',' expr} */
    list_nextfield(parser, l);
    while (next_token(parser).type == OptComma) { /* ',' */
        scan_next_token(parser);
        list_nextfield(parser, l);
    }
}

static void list_expr(bparser *parser, bexpdesc *e)
{
    /* '[' [expr {',' expr}] ']' */
    scan_next_token(parser); /* skip '[' */
    new_primtype(parser, "list", e); /* new list */
    if (next_token(parser).type != OptRSB) { /* ']' */
        list_fields(parser, e);
    }
    match_token(parser, OptRSB); /* skip ']' */
}

static int exprlist(bparser *parser, bexpdesc *e)
{
    bfuncinfo *finfo = parser->finfo;
    int n = 1, reg = finfo->freereg;
    /* expr { ',' expr } */
    expr(parser, e);
    check_vardefine(parser, e);
    be_code_reg(finfo, e, reg++);
    while (next_token(parser).type == OptComma) {
        scan_next_token(parser); /* skip ',' */
        expr(parser, e);
        check_vardefine(parser, e);
        be_code_reg(finfo, e, reg++);
        ++n;
    }
    return n;
}

static void call_expr(bparser *parser, bexpdesc *e)
{
    bexpdesc args;
    bfuncinfo *finfo = parser->finfo;
    int argc = 0, base = finfo->freereg;
    int isfield = e->type == ETMEMBER;

    /* '(' [exprlist] ')' */
    check_vardefine(parser, e);
    /* code function index to next register */
    base = be_code_nextreg(finfo, e);
    if (isfield) {
        be_code_getmethod(finfo);
    }
    scan_next_token(parser); /* skip '(' */
    if (next_token(parser).type != OptRBK) {
        argc = exprlist(parser, &args);
    }
    match_token(parser, OptRBK); /* skip ')' */
    argc += isfield;
    be_code_freeregs(finfo, argc);
    be_code_call(finfo, base, argc);
    if (e->type != ETREG) {
        e->type = ETREG;
        e->v.idx = base;
    }
}

static void field_expr(bparser *parser, bexpdesc *e)
{
    /* . ID */
    check_vardefine(parser, e);
    scan_next_token(parser); /* skip '.' */
    if (next_token(parser).type == TokenId) {
        bexpdesc key;
        init_exp(&key, ETSTRING, 0);
        key.v.s = next_token(parser).u.s;
        be_code_field(parser->finfo, e, &key);
        scan_next_token(parser); /* skip ID */
    }
}

static void index_expr(bparser *parser, bexpdesc *e)
{
    bexpdesc e1;
    /* [expr] */
    check_vardefine(parser, e);
    scan_next_token(parser); /* skip '[' */
    expr(parser, &e1);
    be_code_index(parser->finfo, e, &e1);
    match_token(parser, OptRSB); /* skip ']' */
}

static void primary_expr(bparser *parser, bexpdesc *e)
{
    switch (next_token(parser).type) {
    case OptLBK: /* '(' expr ')' */
        scan_next_token(parser); /* skip '(' */
        expr(parser, e);
        match_token(parser, OptRBK); /* skip '(' */
        break;
    case TokenId:
        singlevar(parser, e);
        scan_next_token(parser);
        break;
    case OptLSB: /* list */
        list_expr(parser, e);
    default: /* unknow expr */
        return;
    }
}

static void suffix_expr(bparser *parser, bexpdesc *e)
{
    primary_expr(parser, e);
    for (;;) {
        switch (next_token(parser).type) {
        case OptLBK: /* '(' function call */
            call_expr(parser, e);
            break;
        case OptDot: /* '.' field */
            field_expr(parser, e);
            break;
        case OptLSB: /* '[' index */
            index_expr(parser, e);
            break;
        default:
            return;
        }
    }
}

static void simple_expr(bparser *parser, bexpdesc *e)
{
    switch (next_token(parser).type) {
    case TokenInteger:
        init_exp(e, ETINT, next_token(parser).u.i);
        break;
    case TokenReal:
        init_exp(e, ETREAL, 0);
        e->v.r = next_token(parser).u.r;
        break;
    case TokenString:
        init_exp(e, ETSTRING, 0);
        e->v.s = next_token(parser).u.s;
        break;
    case KeyTrue:
        init_exp(e, ETBOOL, 1);
        break;
    case KeyFalse:
        init_exp(e, ETBOOL, 0);
        break;
    case KeyNil:
        init_exp(e, ETNIL, 0);
        break;
    default:
        suffix_expr(parser, e);
        return;
    }
    scan_next_token(parser);
}

static void assign_expr(bparser *parser)
{
    int line;
    bexpdesc e;
    expr(parser, &e);
    line = parser->lexer.linepos;
    if (next_token(parser).type == OptAssign) { /* '=' */
        bexpdesc e1;
        scan_next_token(parser);
        expr(parser, &e1);
        if (e.type == ETVOID) { /* new variable */
            new_var(parser, e.v.s, &e);
        }
        if (be_code_setvar(parser->finfo, &e, &e1)) {
            parser->lexer.linepos = line;
            parser_error(parser,
                "try to assign constant expressions.", NULL);
        }
    }
}

/* binary operator: + - * / % && || < <= == != > >=
 * unary operator: + - ! 
 */
static void sub_expr(bparser *parser, bexpdesc *e, int prio)
{
    bfuncinfo *finfo = parser->finfo;
    btokentype op = get_unary_op(parser);
    if (op != OP_NOT_UNARY) {
        scan_next_token(parser);
        sub_expr(parser, e, UNARY_OP_PRIO);
        check_vardefine(parser, e);
        if (be_code_unop(finfo, op, e)) { /* encode unary op */
            parser_error(parser, "neg error", NULL);
        }
    } else {
        simple_expr(parser, e);
    }
    op = get_binop(parser);
    while (op != OP_NOT_BINARY && binary_op_prio(op) > prio) {
        bexpdesc e2;
        scan_next_token(parser);
        check_vardefine(parser, e);
        be_code_prebinop(finfo, op, e); /* and or */
        sub_expr(parser, &e2, binary_op_prio(op));
        check_vardefine(parser, &e2);
        be_code_binop(finfo, op, e, &e2); /* encode binary op */
        op = get_binop(parser);
    }
}

static void expr(bparser *parser, bexpdesc *e)
{
    sub_expr(parser, e, 2);
}

static void expr_stmt(bparser *parser)
{
    assign_expr(parser);
    parser->finfo->freereg = (bbyte)be_vector_count(parser->finfo->local);
}

static int block_follow(bparser *parser)
{
    switch (next_token(parser).type) {
    case KeyElse: case KeyElif:
    case KeyEnd: case TokenEOS:
        return 0;
    default:
        return 1;
    }
}

static int cond_stmt(bparser *parser)
{
    bexpdesc e;
    /* (expr) */
    match_token(parser, OptLBK); /* skip '(' */
    match_notoken(parser, OptRBK);
    expr(parser, &e);
    match_token(parser, OptRBK); /* skip ')' */
    be_code_goiftrue(parser->finfo, &e);
    return e.f;
}

static void condition_block(bparser *parser, int *jmp)
{
    
    bfuncinfo *finfo = parser->finfo;
    int br = cond_stmt(parser);
    block(parser);
    if (next_token(parser).type == KeyElif
            || next_token(parser).type == KeyElse) {
        be_code_conjump(finfo, jmp, be_code_jump(finfo)); /* connect jump */
    }
    be_code_patchjump(finfo, br);
}

static void if_stmt(bparser *parser)
{
    int jl = NO_JUMP; /* jump list */
    /* IF (expr) block {ELSEIF (expr) block}, [ELSE block], end */
    scan_next_token(parser); /* skip 'if' */
    condition_block(parser, &jl);
    while (next_token(parser).type == KeyElif) {
        scan_next_token(parser);
        condition_block(parser, &jl);
    }
    if (next_token(parser).type == KeyElse) {
        scan_next_token(parser);
        block(parser);
    }
    match_token(parser, KeyEnd); /* skip end */
    be_code_patchjump(parser->finfo, jl);
}

static void do_stmt(bparser *parser)
{
    /* DO block END */
    scan_next_token(parser); /* skip 'do' */
    block(parser);
    match_token(parser, KeyEnd); /* skip 'end' */
}

static void while_stmt(bparser *parser)
{
    int end, begin;
    bblockinfo binfo;
    bfuncinfo *finfo = parser->finfo;
    /* WHILE (expr) block END */
    scan_next_token(parser); /* skip 'if' */
    begin_block(parser->finfo, &binfo, 1);
    begin = finfo->pc;
    end = cond_stmt(parser);
    stmtlist(parser);
    be_code_jumpto(finfo, begin);
    end_block(parser);
    be_code_patchjump(finfo, end);
    match_token(parser, KeyEnd); /* skip 'end' */
}

static bblockinfo* get_loop_block(bparser *parser)
{
    bfuncinfo *f = parser->finfo;
    bblockinfo *b = f->binfo;
    /* BREAK | CONTINUE */
    scan_next_token(parser); /* skip 'break' or 'continue' */
    while (b != NULL && !b->isloop) {
        b = b->prev;
    }
    return b;
}

static void break_stmt(bparser *parser)
{
    bfuncinfo *f = parser->finfo;
    bblockinfo *b = get_loop_block(parser);
    if (b != NULL) { /* connect jump */
        be_code_conjump(f, &b->breaklist, be_code_jump(f));
    } else {
        parser_error(parser, "break not loop", NULL);
    }
}

static void continue_stmt(bparser *parser)
{
    bfuncinfo *f = parser->finfo;
    bblockinfo *b = get_loop_block(parser);
    if (b != NULL) { /* connect jump */
        be_code_conjump(f, &b->continuelist, be_code_jump(f));
    } else {
        parser_error(parser, "continue not loop", NULL);
    }
}

static bstring* func_name(bparser *parser, bexpdesc *e, int ismethod)
{
    btokentype type = next_token(parser).type;
    if (type == TokenId) {
        bstring *name = next_token(parser).u.s;
        if (!ismethod) {
            new_var(parser, name, e); /* new variable */
        }
        scan_next_token(parser); /* skip name */
        return name;
    } else if (ismethod && type >= OptAdd && type <= OptGE) {
        scan_next_token(parser); /* skip token */
        /* '-*' neg method */
        if (type == OptSub && next_token(parser).type == OptMul) {
            scan_next_token(parser); /* skip '*' */
            return be_newstr(parser->vm, "-*");
        } else {
            return be_newstr(parser->vm, be_token2str(type));
        }
    }
    parser_error(parser, "token is not identifier.", NULL);
    return NULL;
}

static void func_varlist(bparser *parser)
{
    bexpdesc v;
    /* '(' [ID {',' ID}] ')' */
    match_token(parser, OptLBK); /* skip '(' */
    if (next_token(parser).type == TokenId) {
        bstring *str = next_token(parser).u.s;
        new_var(parser, str, &v); /* new variable */
        scan_next_token(parser);
        while (next_token(parser).type == OptComma) {
            scan_next_token(parser); /* skip ',' */
            str = next_token(parser).u.s;
            /* new local variable */
            if (find_localvar(parser->finfo, str) == -1) {
                new_var(parser, str, &v);
            } else {
                parser_error(parser, "var-mult-define", NULL);
            }
            scan_next_token(parser); /* skip ID */
        }
    }
    match_token(parser, OptRBK); /* skip ')' */
    parser->finfo->proto->argc = parser->finfo->freereg;
}

static bproto* funcbody(bparser *parser, bexpdesc *e, int ismethod)
{
    bstring *name;
    bfuncinfo finfo;
    bblockinfo binfo;

    /* 'def' ID '(' varlist ')' block 'end' */
    scan_next_token(parser); /* skip 'def' */
    name = func_name(parser, e, ismethod);
    begin_func(parser, &finfo, &binfo);
    finfo.proto->name = name;
    if (ismethod) {
        new_localvar(parser->finfo, be_newstr(parser->vm, "self"));
    }
    func_varlist(parser);
    stmtlist(parser);
    end_func(parser);
    match_token(parser, KeyEnd); /* skip 'end' */
    return finfo.proto;
}

static void def_stmt(bparser *parser)
{
    bexpdesc e;
    bproto *proto = funcbody(parser, &e, 0);
    be_code_closure(parser->finfo, &e, proto);
}

static void return_stmt(bparser *parser)
{
    bexpdesc e;
    /* 'return' expr */
    scan_next_token(parser); /* skip 'return' */
    init_exp(&e, ETVOID, 0);
    expr(parser, &e);
    be_code_ret(parser->finfo, &e);
}

static void classvar_stmt(bparser *parser, bclass *c)
{
    /* 'var' ID {',' ID} */
    scan_next_token(parser); /* skip 'var' */
    if (next_token(parser).type == TokenId) {
        be_field_bind(c, next_token(parser).u.s);
        scan_next_token(parser);
        while (next_token(parser).type == OptComma) {
            scan_next_token(parser);
            if (next_token(parser).type == TokenId) {
                be_field_bind(c, next_token(parser).u.s);
                scan_next_token(parser);
            } else {
                parser_error(parser, "class var error", NULL);
            }
        }
    } else {
        parser_error(parser, "class var error", NULL);
    }
}

static void classdef_stmt(bparser *parser, bclass *c)
{
    bexpdesc e;
    bproto *proto = funcbody(parser, &e, 1);
    be_method_bind(parser->vm, c, proto->name, proto);
}

static void class_inherit(bparser *parser, bexpdesc *e)
{
    if (next_token(parser).type == OptColon) { /* ':' */
        bexpdesc e1;
        scan_next_token(parser); /* skip ':' */
        expr(parser, &e1);
        be_code_setsuper(parser->finfo, e, &e1);
    }
}

static void class_block(bparser *parser, bclass *c)
{
    /* { [;] } */
    while (block_follow(parser)) {
        switch (next_token(parser).type) {
        case KeyVar: classvar_stmt(parser, c); break;
        case KeyDef: classdef_stmt(parser, c); break;
        case OptSemic: scan_next_token(parser); break;
        default: parser_error(parser, "class error", NULL);
        }
    }
}

static void class_stmt(bparser *parser)
{
    /* 'class' ID [':' ID] class_block 'end' */
    scan_next_token(parser); /* skip 'class' */
    if (next_token(parser).type == TokenId) {
        bexpdesc e;
        bstring *name = next_token(parser).u.s;
        bclass *c = be_newclass(parser->vm, name, NULL);
        new_class(parser, name, c, &e);
        scan_next_token(parser); /* skip ID */
        class_inherit(parser, &e);
        class_block(parser, c);
        match_token(parser, KeyEnd); /* skip 'end' */
    } else {
        parser_error(parser, "class name error", NULL);
    }
}

static void statement(bparser *parser)
{
    switch (next_token(parser).type) {
    case KeyIf: if_stmt(parser); break;
    case KeyWhile: while_stmt(parser); break;
    case KeyFor: break;
    case KeyDo: do_stmt(parser); break;
    case KeyBreak: break_stmt(parser); break;
    case KeyContinue: continue_stmt(parser); break;
    case KeyDef: def_stmt(parser); break;
    case KeyClass: class_stmt(parser); break;
    case KeyReturn: return_stmt(parser); break;
    case OptSemic: scan_next_token(parser); break; /* empty statement */
    default: expr_stmt(parser); break;
    }
}

static void stmtlist(bparser *parser)
{
    while (block_follow(parser)) {
        statement(parser);
    }
}

static void block(bparser *parser)
{
    bblockinfo binfo;
    begin_block(parser->finfo, &binfo, 0);
    stmtlist(parser);
    end_block(parser);
}

static void mainfunc(bparser *parser, bfuncinfo *finfo)
{
    bblockinfo binfo;

    begin_func(parser, finfo, &binfo);
    finfo->proto->name = be_newstr(parser->vm, "main");
    stmtlist(parser);
    end_func(parser);
}

bclosure* be_parser_source(bvm *vm, const char *fname, const char *text)
{
    bparser parser;
    bfuncinfo finfo;
    bclosure *cl = be_newclosure(vm, 0);

    parser.vm = vm;
    parser.finfo = NULL;
    be_gc_setpause(vm, 0); /* stop auto gc */
    be_gc_fix(vm, gc_object(cl)); /* add main closure to gray list */
    be_lexer_init(&parser.lexer, vm);
    be_lexer_set_source(&parser.lexer, fname, text);
    scan_next_token(&parser); /* scan first token */
    mainfunc(&parser, &finfo);
    scan_next_token(&parser); /* clear lexer */
    cl->proto = finfo.proto;
    cl->proto->argc = 0; /* args */
    be_free(parser.lexer.data);
    be_gc_unfix(vm, gc_object(cl));
    return cl;
}
