#include "be_lexer.h"
#include "be_string.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_debug.h"
#include <string.h>
#include <stdlib.h>

#define SHORT_STR_LEN       32

#define next(lex)           (++(lex)->cursor)
#define prev(lex)           (--(lex)->cursor)
#define lgetc(lex)          (*(lex)->cursor)
#define match(lex, pattern) while (pattern(lgetc(lex))) { next(lex); }
#define setstr(lex, v)      ((lex)->token.u.s = (v))
#define setint(lex, v)      ((lex)->token.u.i = (v))
#define setreal(lex, v)     ((lex)->token.u.r = (v))

static const char* kwords_tab[] = {
        "NONE", "EOS", "ID", "INT", "REAL", "STR",
        "=", "+", "-", "*", "/", "%", "<", "<=",
        "==", "!=", ">", ">=", "..", "&&", "||", "!",
        "(", ")", "[", "]", "{", "}", ".", ",", ";",
        ":", "if", "elif", "else", "while", "for",
        "def", "end", "class", "break", "continue",
        "return", "true", "false", "nil", "var", "do"
};

static void keyword_registe(bvm *vm)
{
    int i, n = (int)(sizeof(kwords_tab) / sizeof(kwords_tab[0]));
    for (i = KeyIf; i < n; ++i) {
        bstring *s = be_newconststr(vm, kwords_tab[i]);
        be_gc_fix(vm, gc_object(s));
        str_setextra(s, i);
    }
}

static void keyword_unregiste(bvm *vm)
{
    int i, n = (int)(sizeof(kwords_tab) / sizeof(kwords_tab[0]));
    for (i = KeyIf; i < n; ++i) {
        bstring *s = be_newconststr(vm, kwords_tab[i]);
        be_gc_unfix(vm, gc_object(s));
    }
}

static int is_newline(int c)
{
    return c == '\r' || c == '\n';
}

static int is_digit(int c)
{
    return c >= '0' && c <= '9';
}

static int is_letter(int c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_');
}

static int is_word(int c)
{
    return is_digit(c) || is_letter(c);
}

static int check_next(blexer *lexer, int c)
{
    if (lgetc(lexer) == c) {
        next(lexer);
        return 1;
    }
    return 0;
}

static void lexer_error(blexer *lexer, const char *err)
{
    be_printf("%s:%d: %s\n", lexer->fname, lexer->linepos, err);
    be_abort();
}

static char* lexer_resize_data(blexer *lexer, int len)
{
    int size = len > SHORT_STR_LEN ? len : SHORT_STR_LEN;

    size += 1; /* include '\0' */
    if (size > lexer->size) {
        lexer->size = size;
        /* don't use be_realloc() no need for copy. */
        be_free(lexer->data);
        lexer->data = be_malloc(size);
    }
    return lexer->data;
}

static int char2hex(int c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 0x0A;
    } else if (c > 'A' && c <= 'F') {
        return c - 'A' + 0x0A;
    }
    return -1;
}

static int check2hex(blexer *lexer, int c)
{
    c = char2hex(c);
    if (c < 0) {
        lexer_error(lexer, "hexadecimal digit expected.");
    }
    return c;
}

static int read_hex(blexer *lexer, const char *src)
{
    int c = check2hex(lexer, *src++);

    return (c << 4) + check2hex(lexer, *src);
}

static int read_oct(blexer *lexer, const char *src)
{
    int i, c = 0;

    for (i = 0; i < 3 && is_digit(*src); ++i) {
        c = 8 * c + *src++ - '0';
    }
    if (i < 3 && !(c == '\0' && i == 1)) {
        lexer_error(lexer, "octal escape too few.");
    }
    return c;
}

static void tr_string(blexer *lexer, const char *src, const char *end)
{
    char *dst = lexer_resize_data(lexer, (int)(end - src));

    while (src < end) {
        int c = *src++;
        switch (c) {
        case '\n': case '\r':
            lexer_error(lexer, "unfinished string.");
            break;
        case '\\':
            switch (*src) {
            case 'a': c = '\a'; break;
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'n': c = '\n'; break;
            case 'r': c = '\r'; break;
            case 't': c = '\t'; break;
            case 'v': c = '\v'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            case '"': c = '"'; break;
            case '?': c = '?'; break;
            case 'x': c = read_hex(lexer, ++src); ++src; break;
            default:
                c = read_oct(lexer, src);
                if (c != '\0') {
                    src += 2;
                }
                break;
            }
            ++src;
            break;
        default:
            break;
        }
        *dst++ = (char)c;
    }
    *dst = '\0';
}

static const char* skip_newline(blexer *lexer)
{
    char lc = lgetc(lexer);

    next(lexer);
    if (is_newline(lgetc(lexer)) && lgetc(lexer) != lc) {
        next(lexer); /* skip "\n\r" or "\r\n" */
    }
    lexer->linepos++;
    return lexer->line = lexer->cursor;
}

static void skip_comment(blexer *lexer)
{
    next(lexer); /* skip '#' */
    if (lgetc(lexer) == '-') { /* mult-line comment */
        int mark, c = 'x'; /* skip first '-' (#- ... -#) */
        do {
            mark = c == '-';
            if (is_newline(c)) {
                c = *skip_newline(lexer);
                continue;
            }
            c = *next(lexer);
        } while (!(mark && c == '#') && c != '\0');
        next(lexer); /* skip '#' */
    } else { /* line comment */
        while (!is_newline(lgetc(lexer)) && lgetc(lexer)) {
            next(lexer);
        }
    }
}

static int scan_realexp(blexer *lexer)
{
    char c = lgetc(lexer);

    if (c == 'e' || c == 'E') {
        c = *next(lexer);
        if (c == '+' || c == '-') {
            c = *next(lexer);
        }
        if (!is_digit(c)) {
            lexer_error(lexer, "number error.");
        }
        match(lexer, is_digit);
        return 1;
    }
    return 0;
}

static btokentype scan_dot_real(blexer *lexer)
{
    const char *begin = lexer->cursor;
    if (*next(lexer) == '.') { /* is '..' */
        next(lexer);
        return OptRange;
    }
    if (is_digit(lgetc(lexer))) {
        match(lexer, is_digit);
        scan_realexp(lexer);
        setreal(lexer, (breal)atof(begin));
        return TokenReal;
    }
    return OptDot;
}

static int scan_hex(blexer *lexer)
{
    int dig, res = 0, cnt = 0;

    while ((dig = char2hex(lgetc(lexer))) >= 0) {
        res = (res << 4) + dig;
        next(lexer);
        ++cnt;
    }
    if (cnt == 0 || cnt > 8) {
        lexer_error(lexer, "hexadecimal number error.");
    }
    return res;
}

static btokentype scan_numeral(blexer *lexer)
{
    const char *begin = lexer->cursor;
    btokentype type = TokenInteger;
    int c = *next(lexer);

    if (c == 'x' || c == 'X') { /* hex 0[xX][0-9a-fA-F]+ */
        next(lexer);
        setint(lexer, scan_hex(lexer));
    } else {
        match(lexer, is_digit);
        if (lgetc(lexer) == '.') { /* '..' or real */
            if (*next(lexer) == '.') { /* '..' */
                prev(lexer);
            } else { /* real */
                match(lexer, is_digit);
                type = TokenReal;
            }
        }
        if (scan_realexp(lexer)) {
            type = TokenReal;
        }
        if (type == TokenReal) {
            setreal(lexer, atof(begin));
        } else {
            setint(lexer, atoi(begin));
        }
    }
    return type;
}

static btokentype scan_identifier(blexer *lexer)
{
    bstring *s;
    const char *begin = lexer->cursor;

    next(lexer);
    match(lexer, is_word);
    s = be_newstrn(lexer->vm, begin, (int)(lexer->cursor - begin));
    if (str_extra(s) != 0) {
        lexer->token.type = (btokentype)str_extra(s);
        return lexer->token.type;
    }
    setstr(lexer, s); /* set identifier name */
    return TokenId;
}

static btokentype scan_string(blexer *lexer)
{
    char c, end = lgetc(lexer);
    const char *begin = lexer->cursor + 1;

    next(lexer);
    while ((c = lgetc(lexer)) != '\0' && (c != end)) {
        next(lexer);
        if (c == '\\') {
            next(lexer); /* skip '\\.' */
        }
    }
    tr_string(lexer, begin, lexer->cursor);
    setstr(lexer, be_newstr(lexer->vm, lexer->data));
    next(lexer); /* skip '"' or '\'' */
    return TokenString;
}

static btokentype opt_and(blexer *lexer)
{
    next(lexer);
    if (!check_next(lexer, '&')) {
        lexer_error(lexer, "operator '&&' spelling mistakes");
    }
    return OptAnd;
}

static btokentype opt_or(blexer *lexer)
{
    next(lexer);
    if (!check_next(lexer, '|')) {
        lexer_error(lexer, "operator '||' spelling mistakes");
    }
    return OptOr;
}

static btokentype lexer_next(blexer *lexer)
{
    for (;;) {
        switch (lgetc(lexer)) {
        case '\r': case '\n': /* newline */
            skip_newline(lexer);
            break;
        case ' ': case '\t': case '\f': case '\v': /* spaces */
            next(lexer);
            break;
        case '#': /* comment */
            skip_comment(lexer);
            break;
        /* operator */
        case '+': next(lexer); return OptAdd;
        case '-': next(lexer); return OptSub;
        case '*': next(lexer); return OptMul;
        case '/': next(lexer); return OptDiv;
        case '%': next(lexer); return OptMod;
        case '(': next(lexer); return OptLBK;
        case ')': next(lexer); return OptRBK;
        case '[': next(lexer); return OptLSB;
        case ']': next(lexer); return OptRSB;
        case '{': next(lexer); return OptLBR;
        case '}': next(lexer); return OptRBR;
        case ',': next(lexer); return OptComma;
        case ';': next(lexer); return OptSemic;
        case ':': next(lexer); return OptColon;
        case '&': return opt_and(lexer);
        case '|': return opt_or(lexer);
        case '<':
            next(lexer);
            return check_next(lexer, '=') ? OptLE : OptLT;
        case '=':
            next(lexer);
            return check_next(lexer, '=') ? OptEQ : OptAssign;
        case '>':
            next(lexer);
            return check_next(lexer, '=') ? OptGE : OptGT;
        case '!':
            next(lexer);
            return check_next(lexer, '=') ? OptNE : OptNot;
        case '\'': case '"':
            return scan_string(lexer);
        case '.':
            return scan_dot_real(lexer);
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return scan_numeral(lexer);
        default:
            if (is_letter(lgetc(lexer))) {
                return scan_identifier(lexer);;
            }
            return TokenNone; /* error */
        }
    }
}

void be_lexer_init(blexer *lexer, bvm *vm)
{
    lexer->vm = vm;
    lexer->line = NULL;
    lexer->cursor = NULL;
    lexer->data = NULL;
    lexer->size = 0;
    keyword_registe(vm);
}

void be_lexer_deinit(blexer *lexer)
{
    be_free(lexer->data);
    keyword_unregiste(lexer->vm);
}

void be_lexer_set_source(blexer *lexer, const char *fname, const char *text)
{
    lexer->fname = fname;
    lexer->line = text;
    lexer->cursor = text;
    lexer->linepos = 1;
}

int be_lexer_scan_next(blexer *lexer)
{
    btokentype type;

    if (lgetc(lexer) == '\0') { /* clear lexer */
        lexer->cursor = lexer->line;
        be_free(lexer->data);
        lexer->data = NULL;
        lexer->size = 0;
        lexer->token.type = TokenEOS;
        return 0;
    }
    type = lexer_next(lexer);
    if (type != TokenNone) {
        lexer->token.type = type;
    } else {
        lexer->token.type = TokenEOS;
        return 0;
    }
    return 1;
}

const char* be_token2str(btokentype type)
{
    return kwords_tab[type];
}
