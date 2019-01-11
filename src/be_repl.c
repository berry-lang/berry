#include "berry.h"
#include "be_repl.h"
#include <string.h>

static int try_return(bvm *vm, const char *line)
{
    int res;
    be_pushfstring(vm, "return (%s)", line);
    line = be_tostring(vm, -1);
    res = be_loadbuffer(vm, "stdin", line, strlen(line)); /* compile line */
    be_removeone(vm, -2); /* remove source string */
    return res;
}

static int compile(bvm *vm, const char *line, breadline getl)
{
    int res = try_return(vm, line);
    if (res == BE_SYNTAX_ERROR) {
        be_pop(vm, 1); /* pop error message */
        be_pushstring(vm, line);
        for (;;) {
            const char *src = be_tostring(vm, -1);
            /* compile source line */
            res = be_loadbuffer(vm, "stdin", src, strlen(src));
            if (!res || !strstr(be_tostring(vm, -1), "'EOS'")) {
                be_removeone(vm, -2);
                return res;
            }
            be_pop(vm, 1); /* pop error message */
            line = getl(">> "); /* read a new input line */
            be_pushfstring(vm, " %s", line);
            be_strconcat(vm, -2);
            be_pop(vm, 1); /* pop new line */
        }
    }
    return res;
}

void be_repl(bvm *vm, breadline getl)
{
    const char *line;
    while ((line = getl("> ")) != NULL) {
        if (compile(vm, line, getl)) {
            be_writestring(be_tostring(vm, -1)); /* some error */
            be_writenewline();
            be_pop(vm, 1);
        } else if (be_pcall(vm, 0)) { /* vm run error */
            be_writestring(be_tostring(vm, -1));
            be_writenewline();
            be_pop(vm, 2);
        } else {
            if (!be_isnil(vm, -1)) {
                be_writestring(be_tostring(vm, -1));
                be_writenewline();
            }
            be_pop(vm, 1);
        }
    }
    be_writenewline();
}
