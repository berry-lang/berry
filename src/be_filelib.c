#include "be_object.h"
#include "be_mem.h"
#include "be_sys.h"
#include <string.h>

#define READLINE_STEP           100

static int i_write(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if(be_iscomptr(vm, -1) && be_isstring(vm, 2)) {
        void *fh = be_tocomptr(vm, -1);
        const char *data = be_tostring(vm, 2);
        be_fwrite(fh, data, be_strlen(vm, 2));
    }
    be_return_nil(vm);
}

static size_t readsize(bvm *vm, int argc, void *fh)
{
    if (argc >=2 && be_isint(vm, 2)) {
        return be_toindex(vm, 2);
    }
    return be_fsize(fh) - be_ftell(fh);
}

static int i_read(bvm *vm)
{
    int argc = be_top(vm);
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        void *fh = be_tocomptr(vm, -1);
        size_t size = readsize(vm, argc, fh);
        char *buffer = be_malloc(size);
        size = be_fread(fh, buffer, size);
        be_pushnstring(vm, buffer, size);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int i_readline(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        void *fh = be_tocomptr(vm, -1);
        size_t pos = 0, size = READLINE_STEP;
        char *buffer = be_malloc(size);
        char *res = be_fgets(fh, buffer, (int)size);
        while (res) {
            pos += strlen(buffer + pos) - 1;
            if (!pos || buffer[pos] == '\n') {
                break;
            }
            size += READLINE_STEP;
            buffer = be_realloc(buffer, size);
            res = be_fgets(fh, buffer + pos + 1, READLINE_STEP);
        }
        be_pushnstring(vm, buffer, size);
        be_return(vm);
    }
    be_return_nil(vm);
}

static int i_seek(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1) && be_isint(vm, 2)) {
        void *fh = be_tocomptr(vm, -1);
        be_fseek(fh, be_toindex(vm, 2));
    }
    be_return_nil(vm);
}

static int i_tell(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        void *fh = be_tocomptr(vm, -1);
        size_t pos = be_ftell(fh);
        be_pushint(vm, cast(bint, pos));
        be_return(vm);
    }
    be_return_nil(vm);
}

static int i_size(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        void *fh = be_tocomptr(vm, -1);
        size_t pos = be_fsize(fh);
        be_pushint(vm, cast(bint, pos));
        be_return(vm);
    }
    be_return_nil(vm);
}

static int i_flush(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        void *fh = be_tocomptr(vm, -1);
        be_fflush(fh);
    }
    be_return_nil(vm);
}

static int i_close(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        void *fh = be_tocomptr(vm, -1);
        be_fclose(fh);
        be_pushnil(vm);
        be_setmember(vm, 1, ".data");
    }
    be_return_nil(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
static int m_open(bvm *vm)
#else
int be_nfunc_open(bvm *vm)
#endif
{
    int argc = be_top(vm);
    const char *fname, *mode;
    static const bnfuncinfo members[] = {
        { ".data", NULL },
        { "write", i_write },
        { "read", i_read },
        { "readline", i_readline },
        { "seek", i_seek },
        { "tell", i_tell },
        { "size", i_size },
        { "flush", i_flush },
        { "close", i_close },
        { "deinit", i_close },
        { NULL, NULL }
    };
    fname = argc >= 1 && be_isstring(vm, 1) ? be_tostring(vm, 1) : NULL;
    mode = argc >= 2 && be_isstring(vm, 2) ? be_tostring(vm, 2) : "r";
    if (fname) {
        void *fh = be_fopen(fname, mode);
        be_pushclass(vm, "file", members);
        be_call(vm, 0);
        if (fh) {
            be_pushcomptr(vm, fh);
        } else {
            be_pushnil(vm);
        }
        be_setmember(vm, -2, ".data");
        be_pop(vm, 1);
        be_return(vm);
    }
    be_return_nil(vm);
}

#if !BE_USE_PRECOMPILED_OBJECT
void be_load_filelib(bvm *vm)
{
    be_regfunc(vm, "open", m_open);
}
#endif
