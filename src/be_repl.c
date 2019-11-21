#include "berry.h"
#include "be_repl.h"
#include <string.h>

#if BE_USE_SCRIPT_COMPILER

static int try_return(bvm *vm, const char *line)
{
    int res, idx;
    line = be_pushfstring(vm, "return (%s)", line);
    idx = be_absindex(vm, -1); /* get the source text absolute index */
    res = be_loadbuffer(vm, "stdin", line, strlen(line)); /* compile line */
    be_remove(vm, idx); /* remove source string */
    return res;
}

static int is_multline(bvm *vm)
{
    const char *msg = be_tostring(vm, -1);
    size_t len = strlen(msg);
    if (len > 5) { /* multi-line text if the error message is 'EOS' at the end */
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
                be_remove(vm, -2);
                return res;
            }
            be_pop(vm, 1); /* pop error message */
            line = getl(">> "); /* read a new input line */
            be_pushfstring(vm, "\n%s", line);
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
            be_writestring(be_tostring(vm, -2)); /* some error */
            be_writestring(": ");
            be_writestring(be_tostring(vm, -1)); /* some error */
            be_writenewline();
            be_pop(vm, 2);
        } else { /* compiled successfully */
            switch (be_pcall(vm, 0)) { /* call the main function */
            case BE_OK: /* execution succeed */
                if (!be_isnil(vm, -1)) { /* output return value when it is not nil */
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
                return be_toindex(vm, -1);
            case BE_MALLOC_FAIL:
                return -BE_MALLOC_FAIL;
            default:
                break;
            }
        }
    }
    be_writenewline();
    return 0;
}

#endif
