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

static int compile(bvm *vm, char *l, breadline gl, bfreeline fl)
{
    int res = try_return(vm, l);
    if (be_getexcept(vm, res) == BE_SYNTAX_ERROR) {
        be_pop(vm, 2); /* pop exception values */
        be_pushstring(vm, l);
        for (;;) {
            const char *src = be_tostring(vm, -1); /* get source code */
            int idx = be_absindex(vm, -1); /* get the source text absolute index */
            /* compile source line */
            res = be_loadbuffer(vm, "stdin", src, strlen(src));
            if (!res || !is_multline(vm)) {
                be_remove(vm, idx); /* remove source code */
                return res;
            }
            be_pop(vm, 2); /* pop exception values */
            fl(l); /* free line buffer */
            l = gl(">> "); /* read a new input line */
            be_pushfstring(vm, "\n%s", l);
            be_strconcat(vm, -2);
            be_pop(vm, 1); /* pop new line */
        }
    }
    return res;
}

static int call_script(bvm *vm)
{
    int res = be_pcall(vm, 0); /* call the main function */
    switch (res) { 
    case BE_OK: /* execution succeed */
        if (!be_isnil(vm, -1)) { /* output return value when it is not nil */
            be_writestring(be_tostring(vm, -1));
            be_writenewline();
        }
        be_pop(vm, 1); /* pop the result value */
        break;
    case BE_EXCEPTION: /* vm run error */
        be_dumpexcept(vm);
        break;
    default: /* BE_EXIT or BE_MALLOC_FAIL */
        return res;
    }
    return 0;
}

BERRY_API int be_repl(bvm *vm, breadline getline, bfreeline freeline)
{
    char *line;
    be_assert(getline != NULL && freeline != NULL);
    while ((line = getline("> ")) != NULL) {
        if (compile(vm, line, getline, freeline)) {
            be_dumpexcept(vm);
        } else { /* compiled successfully */
            int res = call_script(vm);
            if (res) {
                return res == BE_EXIT ? be_toindex(vm, -1) : res;
            }
        }
        if (freeline) {
            freeline(line);
        }
    }
    be_writenewline();
    return 0;
}

#endif
