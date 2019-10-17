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

#define OP_NOT_BINARY           TokenNone
#define OP_NOT_UNARY            TokenNone
#define OP_NOT_ASSIGN           TokenNone
#define UNARY_OP_PRIO           3
#define ASSIGN_OP_PRIO          16

#define FUNC_METHOD             1
#define FUNC_ANONYMOUS          2

/* get binary operator priority */
#define binary_op_prio(op)      (binary_op_prio_tab[cast_int(op) - OptAdd])

#define scan_next_token(parser) (be_lexer_scan_next(&(parser)->lexer))
#define next_token(parser)      ((parser)->lexer.token)
#define next_type(parser)       (next_token(parser).type)
#define max(a, b)               ((a) > (b) ? (a) : (b))
#define token2str(parser)       be_token2str((parser)->vm, &next_token(parser))

#define upval_index(v)          ((v) & 0xFF)
#define upval_target(v)         ((bbyte)(((v) >> 8) & 0xFF))
#define upval_instack(v)        ((bbyte)(((v) >> 16) != 0))
#define upval_desc(i, t, s)     (((i) & 0xFF) | (((t) & 0xFF) << 8) \
                                | (((s) != 0) << 16))

#define parser_newstr(p, str)   be_lexer_newstr(&(p)->lexer, (str))

#define parser_error(p, msg)    be_lexerror(&(p)->lexer, msg)

#define push_error(parser, ...) \
    parser_error(parser, be_pushfstring(parser->vm, __VA_ARGS__))

typedef struct {
    blexer lexer;
    bvm *vm;
    bfuncinfo *finfo;
    bclosure *cl;
} bparser;

#if BE_USE_SCRIPT_COMPILER

static void stmtlist(bparser *parser);
static void block(bparser *parser);
static void expr(bparser *parser, bexpdesc *e);
static void sub_expr(bparser *parser, bexpdesc *e, int prio);

static const int binary_op_prio_tab[] = {
    5, 5, 4, 4, 4, /* + - * / % */
    11, 11, 12, 12, 11, 11, /* < <= == != > >= */
    7, 9, 8, 6, 6, 10, 13, 14 /*  & | ^ << >> .. && || */
};

static void match_token(bparser *parser, btokentype type)
{
    if (next_type(parser) != type) { /* error when token is no match */
        btoken *token = &next_token(parser);
        const char *s1 = be_tokentype2str(type);
        const char *s2 = be_token2str(parser->vm, token);
        push_error(parser, "expected '%s' before '%s'", s1, s2);
    }
    scan_next_token(parser); /* skip this token */
}

static void match_notoken(bparser *parser, btokentype type)
{
    if (next_type(parser) == type) { /* error when token is match */
        push_error(parser,
            "expected statement before '%s'", token2str(parser));
    }
}

static void check_symbol(bparser *parser, bexpdesc *e)
{
    if (e->type == ETVOID && e->v.s == NULL) { /* error when token is not a symbol */
        push_error(parser,
            "unexpected symbol near '%s'", token2str(parser));
    }
}

static void check_var(bparser *parser, bexpdesc *e)
{
    check_symbol(parser, e); /* check the token is a symbol */
    if (e->type == ETVOID) { /* error when symbol is undefined */
        bstring *s = e->v.s;
        push_error(parser,
            "'%s' undeclared (first use in this function)", str(s));
    }
}

static int match_skip(bparser *parser, btokentype type)
{
    if (next_type(parser) == type) { /* match */
        scan_next_token(parser); /* skip this token */
        return btrue;
    }
    return bfalse;
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

static void block_setloop(bfuncinfo *finfo)
{
    bblockinfo *binfo = finfo->binfo;
    binfo->isloop = 1;
    binfo->beginpc = finfo->pc;
    binfo->breaklist = NO_JUMP;
    binfo->continuelist = NO_JUMP;
}

static void end_block(bparser *parser)
{
    bfuncinfo *finfo = parser->finfo;
    bblockinfo *binfo = finfo->binfo;

    be_code_close(finfo, 0); /* close upvalues */
    if (binfo->isloop) {
        be_code_jumpto(finfo, binfo->beginpc);
        be_code_patchjump(finfo, binfo->breaklist);
        be_code_patchlist(finfo, binfo->continuelist, binfo->beginpc);
    }
    be_list_resize(parser->vm, finfo->local, binfo->nactlocals);
    finfo->freereg = binfo->nactlocals;
    finfo->binfo = binfo->prev;
}

static void begin_func(bparser *parser, bfuncinfo *finfo, bblockinfo *binfo)
{
    bvm *vm = parser->vm;
    bproto *proto = be_newproto(vm);
    var_setproto(vm->top, proto);
    be_stackpush(vm);
    be_vector_init(vm, &finfo->code, sizeof(binstruction));
    proto->code = be_vector_data(&finfo->code);
    proto->codesize = be_vector_capacity(&finfo->code);
    be_vector_init(vm, &finfo->kvec, sizeof(bvalue));
    proto->ktab = be_vector_data(&finfo->kvec);
    proto->nconst = be_vector_capacity(&finfo->kvec);
    be_vector_init(vm, &finfo->pvec, sizeof(bproto*));
    proto->ptab = be_vector_data(&finfo->pvec);
    proto->nproto = be_vector_capacity(&finfo->pvec);
    finfo->local = be_list_new(vm);
    var_setlist(vm->top, finfo->local);
    be_stackpush(vm);
    finfo->upval = be_map_new(vm);
    var_setmap(vm->top, finfo->upval);
    be_stackpush(vm);
    finfo->prev = parser->finfo;
    finfo->lexer = &parser->lexer;
    finfo->proto = proto;
    finfo->freereg = 0;
    finfo->binfo = NULL;
    finfo->pc = 0;
    finfo->jpc = NO_JUMP;
    finfo->flag = 0;
    parser->finfo = finfo;
#if BE_DEBUG_RUNTIME_INFO
    be_vector_init(vm, &finfo->linevec, sizeof(blineinfo));
    proto->source = be_newstr(vm, parser->lexer.fname);
    proto->lineinfo = be_vector_data(&finfo->linevec);
    proto->nlineinfo = be_vector_capacity(&finfo->linevec);
#endif
    begin_block(finfo, binfo, 0);
}

static void setupvals(bfuncinfo *finfo)
{
    bproto *proto = finfo->proto;
    int nupvals = be_map_count(finfo->upval);
    if (nupvals) {
        bmapnode *node;
        bmap *map = finfo->upval;
        bmapiter iter = be_map_iter();
        bupvaldesc *upvals = be_malloc(
                finfo->lexer->vm, sizeof(bupvaldesc) * nupvals);
        while ((node = be_map_next(map, &iter)) != NULL) {
            uint32_t v = (uint32_t)node->value.v.i;
            int idx = upval_index(v);
            upvals[idx].idx = upval_target(v);
            upvals[idx].instack = upval_instack(v);
        }
        proto->upvals = upvals;
        proto->nupvals = (bbyte)nupvals;
    }
}

static void end_func(bparser *parser)
{
    bvm *vm = parser->vm;
    bfuncinfo *finfo = parser->finfo;
    bproto *proto = finfo->proto;

    be_code_ret(finfo, NULL); /* append a return to last code */
    end_block(parser);
    setupvals(finfo);
    proto->code = be_vector_release(vm, &finfo->code);
    proto->codesize = finfo->pc;
    proto->ktab = be_vector_release(vm, &finfo->kvec);
    proto->nconst = be_vector_count(&finfo->kvec);
    proto->ptab = be_vector_release(vm, &finfo->pvec);
    proto->nproto = be_vector_count(&finfo->pvec);
#if BE_DEBUG_RUNTIME_INFO
    proto->lineinfo = be_vector_release(vm, &finfo->linevec);
    proto->nlineinfo = be_vector_count(&finfo->linevec);
#endif
    parser->finfo = parser->finfo->prev;
    be_stackpop(vm, 2); /* pop upval and local */
}

static btokentype get_binop(bparser *parser)
{
    btokentype op = next_type(parser);
    if (op >= OptAdd && op <= OptOr) {
        return op;
    }
    return OP_NOT_BINARY;
}

static btokentype get_unary_op(bparser *parser)
{
    btokentype op = next_type(parser);
    if (op == OptSub || op == OptNot || op == OptFlip) {
        return op; /* operator 'negative' 'not' and 'flip' */
    }
    return OP_NOT_UNARY;
}

static btokentype get_assign_op(bparser *parser)
{
    btokentype op = next_type(parser);
    if (op >= OptAssign && op <= OptRsfAssign) {
        return op;
    }
    return OP_NOT_ASSIGN;
}

static void init_exp(bexpdesc *e, exptype_t type, bint i)
{
    e->type = (bbyte)type;
    e->t = NO_JUMP;
    e->f = NO_JUMP;
    e->not = 0;
    e->v.s = NULL;
    e->v.i = i;
}

static int find_localvar(bfuncinfo *finfo, bstring *s, int begin)
{
    int i, count = be_list_count(finfo->local);
    bvalue *var = be_list_data(finfo->local);
    for (i = count - 1; i >= begin; --i) {
        if (be_eqstr(var[i].v.s, s)) {
            return i;
        }
    }
    return -1; /* not found */
}

static int new_localvar(bparser *parser, bstring *s)
{
    bfuncinfo *finfo = parser->finfo;
    int reg = find_localvar(finfo, s, finfo->binfo->nactlocals);
    if (reg == -1) {
        bvalue *var;
        reg = be_list_count(finfo->local); /* new local index */
        var = be_list_append(parser->vm, finfo->local, NULL);
        var_setstr(var, s);
        if (reg >= finfo->freereg) {
            be_code_allocregs(finfo, 1); /* use a register */
        }
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

static int new_upval(bvm *vm, bfuncinfo *finfo, bstring *name, bexpdesc *var)
{
    int index;
    bvalue *desc;
    int target = var->v.idx;
    int instack = var->type == ETLOCAL;
    if (instack) { /* mark upvalue's block */
        mark_upval(finfo, target);
    }
    index = be_map_count(finfo->upval);
    desc = be_map_insertstr(vm, finfo->upval, name, NULL);
    var_setint(desc, upval_desc(index, target, instack));
    return index;
}

static void new_var(bparser *parser, bstring *name, bexpdesc *var)
{
    bfuncinfo *finfo = parser->finfo;
    if (finfo->prev || finfo->binfo->prev) {
        init_exp(var, ETLOCAL, 0);
        var->v.idx = new_localvar(parser, name);
    } else {
        init_exp(var, ETGLOBAL, 0);
        var->v.idx = be_global_new(parser->vm, name);
    }
}

static void new_class(bparser *parser, bstring *name, bclass *c, bexpdesc *e)
{
    bvalue *var;
    bfuncinfo *finfo = parser->finfo;
    if (finfo->prev || finfo->binfo->prev) {
        init_exp(e, ETLOCAL, 0);
        e->v.idx = new_localvar(parser, name);
        var = be_code_localobject(finfo, e->v.idx);
    } else {
        init_exp(e, ETGLOBAL, 0);
        e->v.idx = be_global_new(parser->vm, name);
        var = be_code_globalobject(finfo, e->v.idx);
    }
    var_settype(var, BE_CLASS);
    var->v.p = c;
}

static int singlevaraux(bvm *vm, bfuncinfo *finfo, bstring *s, bexpdesc *var)
{
    if (finfo == NULL) {
        return ETVOID;
    } else {
        int idx = find_localvar(finfo, s, 0);
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
                    idx = new_upval(vm, finfo, s, var); /* new upvalue */
                } else if (be_global_find(vm, s) >= 0) {
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
        var->v.idx = be_global_find(parser->vm, varname);
        break;
    default:
        break;
    }
}

static void func_varlist(bparser *parser)
{
    bexpdesc v;
    /* '(' [ID {',' ID}] ')' */
    match_token(parser, OptLBK); /* skip '(' */
    if (next_type(parser) == TokenId) {
        bstring *str = next_token(parser).u.s;
        new_var(parser, str, &v); /* new variable */
        scan_next_token(parser);
        while (next_type(parser) == OptComma) {
            scan_next_token(parser); /* skip ',' */
            str = next_token(parser).u.s;
            match_token(parser, TokenId); /* match and skip ID */
            /* new local variable */
            if (find_localvar(parser->finfo, str, 0) == -1) {
                new_var(parser, str, &v);
            } else {
                push_error(parser, "redefinition of '%s'", str(str));
            }
        }
    }
    match_token(parser, OptRBK); /* skip ')' */
    parser->finfo->proto->argc = parser->finfo->freereg;
}

static bproto* funcbody(bparser *parser, bstring *name, int type)
{
    bfuncinfo finfo;
    bblockinfo binfo;

    /* '(' varlist ')' block 'end' */
    begin_func(parser, &finfo, &binfo);
    finfo.proto->name = name;
    finfo.flag = (bbyte)type;
    if (type & FUNC_METHOD) {
        new_localvar(parser, parser_newstr(parser, "self"));
    }
    func_varlist(parser);
    stmtlist(parser);
    end_func(parser);
    match_token(parser, KeyEnd); /* skip 'end' */
    return finfo.proto;
}

/* anonymous function */
static void anon_func(bparser *parser, bexpdesc *e)
{
    bproto *proto;
    bstring *name = parser_newstr(parser, "<anonymous>");
    /* 'def' ID '(' varlist ')' block 'end' */
    scan_next_token(parser); /* skip 'def' */
    proto = funcbody(parser, name, FUNC_ANONYMOUS);
    init_exp(e, ETPROTO, be_code_proto(parser->finfo, proto));
    be_stackpop(parser->vm, 1);
}

static void lambda_varlist(bparser *parser)
{
    bexpdesc v;
    /* [ID {',' ID}] | {ID}] */
    if (next_type(parser) == TokenId) {
        bbool comma;
        bstring *str = next_token(parser).u.s;
        new_var(parser, str, &v); /* new variable */
        scan_next_token(parser);
        comma = next_type(parser) == OptComma;
        while (next_type(parser) != OptArrow) {
            if (comma) {
                match_token(parser, OptComma); /* match and skip ',' */
            }
            str = next_token(parser).u.s;
            match_token(parser, TokenId); /* match and skip ID */
            /* new local variable */
            if (find_localvar(parser->finfo, str, 0) == -1) {
                new_var(parser, str, &v);
            } else {
                push_error(parser, "redefinition of '%s'", str(str));
            }
        }
    }
    match_token(parser, OptArrow); /* skip '->' */
    parser->finfo->proto->argc = parser->finfo->freereg;
}

/* lambda expression */
static void lambda_expr(bparser *parser, bexpdesc *e)
{
    bexpdesc e1;
    bfuncinfo finfo;
    bblockinfo binfo;
    bstring *name = parser_newstr(parser, "<lambda>");
    /* '/' ID {[',' ID]} '->' expr */
    scan_next_token(parser); /* skip '/' */
    begin_func(parser, &finfo, &binfo);
    finfo.proto->name = name;
    finfo.flag = (bbyte)FUNC_ANONYMOUS;
    lambda_varlist(parser);
    expr(parser, &e1);
    check_var(parser, &e1);
    be_code_ret(parser->finfo, &e1);
    end_func(parser);
    init_exp(e, ETPROTO, be_code_proto(parser->finfo, finfo.proto));
    be_stackpop(parser->vm, 1);
}

static void new_primtype(bparser *parser, const char *type, bexpdesc *e)
{
    int idx;
    bvm *vm = parser->vm;
    bfuncinfo *finfo = parser->finfo;

    scan_next_token(parser);
    idx = be_builtin_find(vm, parser_newstr(parser, type));
    init_exp(e, ETGLOBAL, idx);
    idx = be_code_nextreg(finfo, e);
    be_code_call(finfo, idx, 0);
    e->type = ETLOCAL;
}

static void list_nextmember(bparser *parser, bexpdesc *l)
{
    bexpdesc e, key, v = *l;
    bfuncinfo *finfo = parser->finfo;
    int base;

    init_exp(&key, ETSTRING, 0);
    key.v.s = parser_newstr(parser, "append");
    be_code_member(finfo, &v, &key);
    base = be_code_getmethod(finfo, &v);
    /* copy source value to next register */
    expr(parser, &e);
    check_var(parser, &e);
    be_code_nextreg(finfo, &e);
    be_code_call(finfo, base, 2);
    be_code_freeregs(finfo, 1); /* free return value */
}

static void map_nextmember(bparser *parser, bexpdesc *l)
{
    bexpdesc e, key, v = *l;
    bfuncinfo *finfo = parser->finfo;
    int base;

    init_exp(&key, ETSTRING, 0);
    key.v.s = parser_newstr(parser, "insert");
    be_code_member(finfo, &v, &key);
    base = be_code_getmethod(finfo, &v);
    /* copy source value to next register */
    expr(parser, &e); /* key */
    check_var(parser, &e);
    be_code_nextreg(finfo, &e);
    match_token(parser, OptColon); /* ':' */
    expr(parser, &e); /* value */
    check_var(parser, &e);
    be_code_nextreg(finfo, &e);
    be_code_call(finfo, base, 3);
    be_code_freeregs(finfo, 1); /* free return value */
}

static void list_expr(bparser *parser, bexpdesc *e)
{
    /* '[' {expr ','} [expr] ']' */
    new_primtype(parser, "list", e); /* new list */
    while (next_type(parser) != OptRSB) {
        list_nextmember(parser, e);
        if (!match_skip(parser, OptComma)) { /* ',' */
            break;
        }
    }
    e->type = ETREG;
    match_token(parser, OptRSB); /* skip ']' */
}

static void map_expr(bparser *parser, bexpdesc *e)
{
    /* '{' {expr ':' expr ','} [expr ':' expr] '}' */
    new_primtype(parser, "map", e); /* new map */
    while (next_type(parser) != OptRBR) {
        map_nextmember(parser, e);
        if (!match_skip(parser, OptComma)) { /* ',' */
            break;
        }
    }
    e->type = ETREG;
    match_token(parser, OptRBR); /* skip '}' */
}

static int exprlist(bparser *parser, bexpdesc *e)
{
    bfuncinfo *finfo = parser->finfo;
    int n = 1;
    /* expr { ',' expr } */
    expr(parser, e);
    check_var(parser, e);
    be_code_nextreg(finfo, e);
    while (next_type(parser) == OptComma) {
        scan_next_token(parser); /* skip ',' */
        expr(parser, e);
        check_var(parser, e);
        be_code_nextreg(finfo, e);
        ++n;
    }
    return n;
}

static void call_expr(bparser *parser, bexpdesc *e)
{
    bexpdesc args;
    bfuncinfo *finfo = parser->finfo;
    int argc = 0, base = finfo->freereg;
    int ismember = e->type == ETMEMBER;

    /* func '(' [exprlist] ')' */
    check_var(parser, e);
    /* code function index to next register */
    if (ismember) {
        base = be_code_getmethod(finfo, e);
    } else {
        base = be_code_nextreg(finfo, e);
    }
    scan_next_token(parser); /* skip '(' */
    if (next_type(parser) != OptRBK) {
        argc = exprlist(parser, &args);
    }
    match_token(parser, OptRBK); /* skip ')' */
    argc += ismember;
    be_code_call(finfo, base, argc);
    if (e->type != ETREG) {
        e->type = ETREG;
        e->v.idx = base;
    }
}

static void member_expr(bparser *parser, bexpdesc *e)
{
    /* . ID */
    check_var(parser, e);
    scan_next_token(parser); /* skip '.' */
    if (next_type(parser) == TokenId) {
        bexpdesc key;
        init_exp(&key, ETSTRING, 0);
        key.v.s = next_token(parser).u.s;
        be_code_member(parser->finfo, e, &key);
        scan_next_token(parser); /* skip ID */
    } else {
        push_error(parser, "invalid syntax near '%s'",
            be_token2str(parser->vm, &next_token(parser)));
    }
}

static void index_expr(bparser *parser, bexpdesc *e)
{
    bexpdesc e1;
    /* [expr] */
    check_var(parser, e);
    scan_next_token(parser); /* skip '[' */
    expr(parser, &e1);
    check_var(parser, &e1);
    be_code_index(parser->finfo, e, &e1);
    match_token(parser, OptRSB); /* skip ']' */
}

static void simple_expr(bparser *parser, bexpdesc *e)
{
    switch (next_type(parser)) {
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
    case TokenId:
        singlevar(parser, e);
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
    default: /* unknow expr */
        return;
    }
    scan_next_token(parser);
}

static void primary_expr(bparser *parser, bexpdesc *e)
{
    switch (next_type(parser)) {
    case OptLBK: /* '(' expr ')' */
        scan_next_token(parser); /* skip '(' */
        /* sub_expr() is more efficient because there is no need to initialize e. */
        sub_expr(parser, e, ASSIGN_OP_PRIO);
        check_var(parser, e);
        match_token(parser, OptRBK); /* skip ')' */
        break;
    case OptLSB: /* list */
        list_expr(parser, e);
        break;
    case OptLBR: /* map */
        map_expr(parser, e);
        break;
    case KeyDef: /* anonymous function */
        anon_func(parser, e);
        break;
    case OptDiv: /* lambda expression */
        lambda_expr(parser, e);
        break;
    default: /* simple expr */
        simple_expr(parser, e);
        break;
    }
}

static void suffix_expr(bparser *parser, bexpdesc *e)
{
    primary_expr(parser, e);
    for (;;) {
        switch (next_type(parser)) {
        case OptLBK: /* '(' function call */
            call_expr(parser, e);
            break;
        case OptDot: /* '.' member */
            member_expr(parser, e);
            break;
        case OptLSB: /* '[' index */
            index_expr(parser, e);
            break;
        default:
            return;
        }
    }
}

/* compound assignment */
static void compound_assign(bparser *parser, int op, bexpdesc *l, bexpdesc *r)
{
    if (op != OptAssign) { /* check left variable */
        check_var(parser, l);
    }
    expr(parser, r); /* right expression */
    check_var(parser, r);
    if (op != OptAssign) { /* compound assignment */
        bexpdesc e = *l;
        op = op < OptAndAssign ? op - OptAddAssign + OptAdd
                : op - OptAndAssign + OptBitAnd;
        be_code_binop(parser->finfo, op, &e, r); /* coding operation */
        *r = e;
    }
}

static int check_newvar(bparser *parser, bexpdesc *e)
{
    if (e->type == ETGLOBAL) {
        if (e->v.idx < be_builtin_count(parser->vm)) {
            e->v.s = be_builtin_name(parser->vm, e->v.idx);
            return btrue;
        }
        return bfalse;
    }
    return e->type == ETVOID;
}

static void assign_expr(bparser *parser)
{
    bexpdesc e;
    btokentype op;
    int line = parser->lexer.linenumber;
    expr(parser, &e); /* left expression */
    check_symbol(parser, &e);
    op = get_assign_op(parser);
    if (op != OP_NOT_ASSIGN) { /* assign operator */
        bexpdesc e1;
        scan_next_token(parser);
        compound_assign(parser, op, &e, &e1);
        if (check_newvar(parser, &e)) { /* new variable */
            new_var(parser, e.v.s, &e);
        }
        if (be_code_setvar(parser->finfo, &e, &e1)) {
            parser->lexer.linenumber = line;
            parser_error(parser,
                "try to assign constant expressions.");
        }
    } else if (e.type == ETREG) {
        be_code_freeregs(parser->finfo, 1);
    } else if (e.type == ETVOID) { /* not assign expression */
        /* undeclared symbol */
        parser->lexer.linenumber = line;
        check_var(parser, &e);
    } 
}

/* conditional expression */
static void cond_expr(bparser *parser, bexpdesc *e)
{
    /* expr '?' expr ':' expr */
    if (next_type(parser) == OptQuestion) {
        int jf, jl = NO_JUMP; /* jump list */
        bfuncinfo *finfo = parser->finfo;
        scan_next_token(parser); /* skip '?' */
        be_code_goiftrue(finfo, e);
        jf = e->f;
        expr(parser, e);
        check_var(parser, e);
        be_code_nextreg(finfo, e);
        be_code_freeregs(finfo, 1);
        be_code_conjump(finfo, &jl, be_code_jump(finfo)); /* connect jump */
        be_code_patchjump(finfo, jf);
        match_token(parser, OptColon); /* match and skip ':' */
        expr(parser, e);
        check_var(parser, e);
        e->v.idx = be_code_nextreg(finfo, e);
        be_code_patchjump(finfo, jl);
        e->type = ETREG;
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
        int line, res;
        scan_next_token(parser);
        line = parser->lexer.linenumber;
        sub_expr(parser, e, UNARY_OP_PRIO);
        check_var(parser, e);
        res = be_code_unop(finfo, op, e);
        if (res) { /* encode unary op */
            parser->lexer.linenumber = line;
            push_error(parser, "wrong type argument to unary '%s'",
                res == 1 ? "negative" : "bit-flip");
        }
    } else {
        suffix_expr(parser, e);
    }
    op = get_binop(parser);
    while (op != OP_NOT_BINARY && prio > binary_op_prio(op)) {
        bexpdesc e2;
        check_var(parser, e);
        scan_next_token(parser);
        be_code_prebinop(finfo, op, e); /* and or */
        init_exp(&e2, ETVOID, 0);
        sub_expr(parser, &e2, binary_op_prio(op));
        check_var(parser, &e2);
        be_code_binop(finfo, op, e, &e2); /* encode binary op */
        op = get_binop(parser);
    }
    if (prio == ASSIGN_OP_PRIO) {
        cond_expr(parser, e);
    }
}

static void expr(bparser *parser, bexpdesc *e)
{
    init_exp(e, ETVOID, 0);
    sub_expr(parser, e, ASSIGN_OP_PRIO);
}

static void expr_stmt(bparser *parser)
{
    assign_expr(parser);
}

static int block_follow(bparser *parser)
{
    switch (next_type(parser)) {
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
    if (next_type(parser) == KeyElif
            || next_type(parser) == KeyElse) {
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
    while (next_type(parser) == KeyElif) {
        scan_next_token(parser);
        condition_block(parser, &jl);
    }
    if (next_type(parser) == KeyElse) {
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
    int brk;
    bblockinfo binfo;
    bfuncinfo *finfo = parser->finfo;
    /* WHILE (expr) block END */
    scan_next_token(parser); /* skip 'while' */
    begin_block(parser->finfo, &binfo, 1);
    brk = cond_stmt(parser);
    stmtlist(parser);
    end_block(parser);
    be_code_patchjump(finfo, brk);
    match_token(parser, KeyEnd); /* skip 'end' */
}

static void for_itvar(bparser *parser, bexpdesc *e)
{
    if (next_type(parser) == TokenId) {
        bstring *s = next_token(parser).u.s;
        int idx = new_localvar(parser, s);
        init_exp(e, ETLOCAL, idx);
        scan_next_token(parser);
    } else {
        push_error(parser,
            "missing iteration variable before '%s'",
            token2str(parser));
    }
}

static void for_init(bparser *parser, bexpdesc *v)
{
    bexpdesc e;
    bstring *s;
    bfuncinfo *finfo = parser->finfo;

    /* .it = __iterator__(expr) */
    s = parser_newstr(parser, "__iterator__");
    init_exp(&e, ETGLOBAL, be_builtin_find(parser->vm, s));
    be_code_nextreg(finfo, &e); /* code function '__iterator__' */
    expr(parser, v);
    check_var(parser, v);
    be_code_nextreg(finfo, v);
    be_code_call(finfo, e.v.idx, 1); /* call __iterator__(expr) */
    be_code_freeregs(finfo, 1); /* free register of __iterator__ */
    s = parser_newstr(parser, ".it");
    init_exp(v, ETLOCAL, new_localvar(parser, s));
}

/*
 * while (__hasnext__(.it))
 *     var = __next__(.it)
 *     stmtlist
 * end
 */
static void for_iter(bparser *parser, bexpdesc *v, bexpdesc *it)
{
    bexpdesc e;
    bstring *s;
    int brk;
    bfuncinfo *finfo = parser->finfo;

    block_setloop(finfo);
    /* __hasnext__(.it) */
    s = parser_newstr(parser, "__hasnext__");
    init_exp(&e, ETGLOBAL, be_builtin_find(parser->vm, s));
    be_code_nextreg(finfo, &e); /* code function '__hasnext__' */
    be_code_nextreg(finfo, it); /* code argv[0]: '.it' */
    be_code_call(finfo, e.v.idx, 1); /* call __hasnext__(.it) */
    be_code_goiftrue(finfo, &e);
    brk = e.f;
    /* var = __next__(.it) */
    s = parser_newstr(parser, "__next__");
    init_exp(&e, ETGLOBAL, be_builtin_find(parser->vm, s));
    be_code_nextreg(finfo, &e); /* code function '__next__' */
    be_code_nextreg(finfo, it); /* code argv[0]: '.it' */
    be_code_call(finfo, e.v.idx, 1); /* call __next__(.it) */
    be_code_setvar(finfo, v, &e); /* code var = __next__(.it) */
    stmtlist(parser);
    end_block(parser);
    be_code_patchjump(finfo, brk);
}

static void for_stmt(bparser *parser)
{
    bblockinfo binfo;
    bexpdesc var, iter;
    /* FOR (ID : expr) block END */
    scan_next_token(parser); /* skip 'for' */
    match_token(parser, OptLBK); /* skip '(' */
    begin_block(parser->finfo, &binfo, 0);
    for_itvar(parser, &var);
    match_token(parser, OptColon); /* skip ':' */
    for_init(parser, &iter);
    match_token(parser, OptRBK); /* skip ')' */
    for_iter(parser, &var, &iter);
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
        parser_error(parser, "break not loop");
    }
}

static void continue_stmt(bparser *parser)
{
    bfuncinfo *f = parser->finfo;
    bblockinfo *b = get_loop_block(parser);
    if (b != NULL) { /* connect jump */
        be_code_conjump(f, &b->continuelist, be_code_jump(f));
    } else {
        parser_error(parser, "continue not loop");
    }
}

static bstring* func_name(bparser *parser, bexpdesc *e, int ismethod)
{
    btokentype type = next_type(parser);
    if (type == TokenId) {
        bstring *name = next_token(parser).u.s;
        if (!ismethod) {
            new_var(parser, name, e); /* new variable */
        }
        scan_next_token(parser); /* skip name */
        return name;
    } else if (ismethod && type >= OptAdd && type <= OptShiftR) {
        scan_next_token(parser); /* skip token */
        /* '-*' neg method */
        if (type == OptFlip || (type == OptSub
            && next_type(parser) == OptMul)) {
            scan_next_token(parser); /* skip '*' */
            return parser_newstr(parser, "-*");
        }
        return parser_newstr(parser, be_tokentype2str(type));
    }
    push_error(parser,
        "the token '%s' is not a valid function name.",
        token2str(parser));
    return NULL;
}

static void def_stmt(bparser *parser)
{
    bexpdesc e;
    bproto *proto;
    bfuncinfo *finfo = parser->finfo;
    /* 'def' ID '(' varlist ')' block 'end' */
    scan_next_token(parser); /* skip 'def' */
    proto = funcbody(parser, func_name(parser, &e, 0), 0);
    be_code_closure(finfo, &e, be_code_proto(finfo, proto));
    be_stackpop(parser->vm, 1);
}

static void return_stmt(bparser *parser)
{
    bexpdesc e;
    /* 'return' expr */
    scan_next_token(parser); /* skip 'return' */
    expr(parser, &e);
    if (e.v.s) { /* expression is not empty */
        check_var(parser, &e);
    }
    be_code_ret(parser->finfo, &e);
}

static void check_class_attr(bparser *parser, bclass *c, bstring *attr)
{
    if (be_class_attribute(c, attr) != BE_NIL) {
        push_error(parser,
            "redefinition of the attribute '%s'", str(attr));
    }
}

static void classvar_stmt(bparser *parser, bclass *c)
{
    /* 'var' ID {',' ID} */
    scan_next_token(parser); /* skip 'var' */
    if (next_type(parser) == TokenId) {
        bstring *name = next_token(parser).u.s;
        check_class_attr(parser, c, name);
        be_member_bind(parser->vm, c, name);
        scan_next_token(parser);
        while (next_type(parser) == OptComma) {
            scan_next_token(parser);
            if (next_type(parser) == TokenId) {
                name = next_token(parser).u.s;
                check_class_attr(parser, c, name);
                be_member_bind(parser->vm, c, name);
                scan_next_token(parser);
            } else {
                parser_error(parser, "class var error");
            }
        }
    } else {
        parser_error(parser, "class var error");
    }
}

static void classdef_stmt(bparser *parser, bclass *c)
{
    bexpdesc e;
    bstring *name;
    bproto *proto;
    /* 'def' ID '(' varlist ')' block 'end' */
    scan_next_token(parser); /* skip 'def' */
    name = func_name(parser, &e, 1);
    check_class_attr(parser, c, name);
    proto = funcbody(parser, name, FUNC_METHOD);
    be_method_bind(parser->vm, c, proto->name, proto);
    be_stackpop(parser->vm, 1);
}

static void class_inherit(bparser *parser, bexpdesc *e)
{
    if (next_type(parser) == OptColon) { /* ':' */
        bexpdesc e1;
        scan_next_token(parser); /* skip ':' */
        expr(parser, &e1);
        check_var(parser, &e1);
        be_code_setsuper(parser->finfo, e, &e1);
    }
}

static void class_block(bparser *parser, bclass *c)
{
    /* { [;] } */
    while (block_follow(parser)) {
        switch (next_type(parser)) {
        case KeyVar: classvar_stmt(parser, c); break;
        case KeyDef: classdef_stmt(parser, c); break;
        case OptSemic: scan_next_token(parser); break;
        default: push_error(parser,
                "unexpected token '%s'", token2str(parser));
        }
    }
}

static void class_stmt(bparser *parser)
{
    /* 'class' ID [':' ID] class_block 'end' */
    scan_next_token(parser); /* skip 'class' */
    if (next_type(parser) == TokenId) {
        bexpdesc e;
        bstring *name = next_token(parser).u.s;
        bclass *c = be_newclass(parser->vm, name, NULL);
        new_class(parser, name, c, &e);
        scan_next_token(parser); /* skip ID */
        class_inherit(parser, &e);
        class_block(parser, c);
        be_map_release(parser->vm, c->members); /* clear space */
        match_token(parser, KeyEnd); /* skip 'end' */
    } else {
        parser_error(parser, "class name error");
    }
}

static void import_stmt(bparser *parser)
{
    bstring *name;
    bexpdesc m, v;
    /* 'import' ID ['as' ID] */
    scan_next_token(parser); /* skip 'import' */
    name = next_token(parser).u.s;
    match_token(parser, TokenId); /* match and skip ID */
    init_exp(&m, ETSTRING, 0);
    m.v.s = name;
    if (next_type(parser) == KeyAs) {
        scan_next_token(parser); /* skip 'as' */
        name = next_token(parser).u.s;
        match_token(parser, TokenId);  /* match and skip ID */
    }
    new_var(parser, name, &v);
    be_code_import(parser->finfo, &m, &v);
}

static void var_field(bparser *parser)
{
    /* ID ['=' expr] */
    bexpdesc e1, e2;
    bstring *name;
    name = next_token(parser).u.s;
    match_token(parser, TokenId); /* match and skip ID */
    if (next_type(parser) == OptAssign) { /* '=' */
        scan_next_token(parser); /* skip '=' */
        expr(parser, &e2);
        check_var(parser, &e2);
    } else {
        init_exp(&e2, ETNIL, 0);
    }
    new_var(parser, name, &e1); /* new local variable */
    be_code_setvar(parser->finfo, &e1, &e2);
}

static void var_stmt(bparser *parser)
{
    /* 'var' ID ['=' expr] {',' ID ['=' expr]} */
    scan_next_token(parser); /* skip 'var' */
    var_field(parser);
    while (next_type(parser) == OptComma) {
        scan_next_token(parser); /* skip ',' */
        var_field(parser);
    }
}

static void statement(bparser *parser)
{
    switch (next_type(parser)) {
    case KeyIf: if_stmt(parser); break;
    case KeyWhile: while_stmt(parser); break;
    case KeyFor: for_stmt(parser); break;
    case KeyDo: do_stmt(parser); break;
    case KeyBreak: break_stmt(parser); break;
    case KeyContinue: continue_stmt(parser); break;
    case KeyDef: def_stmt(parser); break;
    case KeyClass: class_stmt(parser); break;
    case KeyReturn: return_stmt(parser); break;
    case KeyImport: import_stmt(parser); break;
    case KeyVar: var_stmt(parser); break;
    case OptSemic: scan_next_token(parser); break; /* empty statement */
    default: expr_stmt(parser); break;
    }
    be_assert(parser->finfo->freereg == be_list_count(parser->finfo->local));
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

static void mainfunc(bparser *parser, bclosure *cl)
{
    bblockinfo binfo;
    bfuncinfo finfo;
    begin_func(parser, &finfo, &binfo);
    finfo.proto->argc = 0; /* args */
    finfo.proto->name = be_newstr(parser->vm, "main");
    cl->proto = finfo.proto;
    be_remove(parser->vm, -3);  /* pop proto from stack */
    stmtlist(parser);
    end_func(parser);
    match_token(parser, TokenEOS); /* skip EOS */
}

bclosure* be_parser_source(bvm *vm,
    const char *fname, breader reader, void *data)
{
    bparser parser;
    bclosure *cl = be_newclosure(vm, 0);

    parser.vm = vm;
    parser.finfo = NULL;
    parser.cl = cl;
    var_setclosure(vm->top, cl);
    be_stackpush(vm);
    be_lexer_init(&parser.lexer, vm, fname, reader, data);
    scan_next_token(&parser); /* scan first token */
    mainfunc(&parser, cl);
    be_lexer_deinit(&parser.lexer);
    be_global_release_space(vm); /* clear global space */
    be_stackpop(vm, 1);
    scan_next_token(&parser); /* clear lexer */
    return cl;
}

#endif
