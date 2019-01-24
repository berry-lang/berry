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

static int is_multline(bvm *vm)
{
    const char *msg = be_tostring(vm, -1);
    size_t len = strlen(msg);
    if (len > 5) {
        return !strcmp(msg + len - 5, "'EOS'");
    }
    return 0;
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
            if (!res || !is_multline(vm)) {
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

int be_repl(bvm *vm, breadline getl)
{
    const char *line;
    while ((line = getl("> ")) != NULL) {
        if (compile(vm, line, getl)) {
            be_writestring(be_tostring(vm, -1)); /* some error */
            be_writenewline();
            be_pop(vm, 1);
        } else {
            switch (be_pcall(vm, 0)) {
            case BE_OK:
                if (!be_isnil(vm, -1)) {
                    be_writestring(be_tostring(vm, -1));
                    be_writenewline();
                }
                be_pop(vm, 1);
                break;
            case BE_EXEC_ERROR: /* vm run error */
                be_writestring(be_tostring(vm, -1));
                be_writenewline();
                be_pop(vm, 2);
                break;
            case BE_EXIT:
                return be_toint(vm, -1);
            case BE_MALLOC_FAIL:
                return -1;
            default:
                break;
            }
        }
    }
    be_writenewline();
    return 0;
}
