#ifndef BE_LEXER_H
#define BE_LEXER_H

#include "be_object.h"

typedef enum {
    TokenNone = 0,
    TokenEOS,       /* end of source */
    TokenId,        /* identifier */
    TokenInteger,
    TokenReal,
    TokenString,
    /* operator, don't change order */
    /* assign operator */
    OptAssign,      /* operator, = */
    /* binary operator */
    OptAdd,         /* operator, + */
    OptSub,         /* operator, - */
    OptMul,         /* operator, * */
    OptDiv,         /* operator, / */
    OptMod,         /* operator, % */
    OptLT,          /* operator, < */
    OptLE,          /* operator, <= */
    OptEQ,          /* operator, == */
    OptNE,          /* operator, != */
    OptGT,          /* operator, > */
    OptGE,          /* operator, >= */
    OptRange,       /* operator, .. */
    OptAnd,         /* operator, && */
    OptOr,          /* operator, || */
    /* unary operator */
    OptNot,        /* operator, ! */
    /* postfix operator or bracket */
    OptLBK,         /* operator, ( bracket */
    OptRBK,         /* operator, ) bracket */
    OptLSB,         /* operator, [ square bracket */
    OptRSB,         /* operator, ] square bracket */
    OptLBR,         /* operator, { brace */
    OptRBR,         /* operator, } brace */
    OptDot,         /* operator, . dot */
    /* other symbol */
    OptComma,       /* operator, , */
    OptSemic,       /* operator, ; */
    OptColon,       /* operator, : */
    /* keyword */
    KeyIf,          /* keyword if */
    KeyElif,        /* keyword elif */
    KeyElse,        /* keyword else */
    KeyWhile,       /* keyword while */
    KeyFor,         /* keyword for */
    KeyDef,         /* keyword def */
    KeyEnd,         /* keyword end */
    KeyClass,       /* keyword class */
    KeyBreak,       /* keyword break */
    KeyContinue,    /* keyword continue */
    KeyReturn,      /* keyword return */
    KeyTrue,        /* keyword true */
    KeyFalse,       /* keyword false */
    KeyNil,         /* keyword nil */
    KeyVar,         /* keyword var */
    KeyDo,          /* keyword do */
    KeyImport,      /* keyword import */
    KeyAs           /* keyword as */
} btokentype;

struct blexerbuf {
    char *s;
    size_t len, size;
};

typedef struct btoken {
    btokentype type;
    union {
        bstring *s;
        bint i;
        breal r;
    } u;
} btoken;

typedef struct blexer {
    const char *fname;
    const char *line, *cursor, *endbuf;
    btoken token;
    int linenumber;
    int lastline;
    bvm *vm;
    btokentype cacheType;
    struct blexerbuf buf;
} blexer;

void be_lexer_init(blexer *lexer, bvm *vm);
void be_lexer_deinit(blexer *lexer);
void be_lexerror(blexer *lexer, const char *msg);
void be_lexer_set_source(blexer *lexer,
    const char *fname, const char *text, size_t length);
int be_lexer_scan_next(blexer *lexer);
const char* be_token2str(bvm *vm, btoken *token);
const char* be_tokentype2str(btokentype type);

#endif
