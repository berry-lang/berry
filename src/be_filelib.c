#include "be_filelib.h"
#include "be_mem.h"

static long int _filesize(be_fstream *fp)
{
    long int size, pos = ftell(fp);
    be_fseek(fp, 0L, SEEK_END);
    size = be_ftell(fp);
    be_fseek(fp, pos, SEEK_SET);
    return size;
}

static int i_write(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if(be_iscomptr(vm, -1) && be_isstring(vm, 2)) {
        be_fstream *fp = be_tocomptr(vm, -1);
        const char *data = be_tostring(vm, 2);
        be_fwrite(fp, data, be_strlen(vm, 2));
    }
    return be_returnnil(vm);
}

static int i_read(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        be_fstream *fp = be_tocomptr(vm, -1);
        long int size = _filesize(fp);
        char *buffer = be_malloc(size);
        size = be_fread(fp, buffer, size);
        be_pushnstring(vm, buffer, size);
        return be_return(vm);
    }
    return be_returnnil(vm);
}

static int i_close(bvm *vm)
{
    be_getmember(vm, 1, ".data");
    if (be_iscomptr(vm, -1)) {
        be_fstream *fp = be_tocomptr(vm, -1);
        be_fclose(fp);
        be_pushnil(vm);
        be_setmember(vm, 1, ".data");
    }
    return be_returnnil(vm);
}

static int m_open(bvm *vm)
{
    static const bmemberinfo members[] = {
        { ".data", NULL },
        { "write", i_write },
        { "read", i_read },
        { "close", i_close },
        { NULL, NULL }
    };
    if (be_isstring(vm, 1) && be_isstring(vm, 2)) {
        const char *fname = be_tostring(vm, 1);
        const char *mode = be_tostring(vm, 2);
        be_fstream *fp = be_fopen(fname, mode);
        be_pushclass(vm, "file", members);
        be_call(vm, 0);
        be_pushcomptr(vm, fp);
        be_setmember(vm, -2, ".data");
        be_pop(vm, 1);
        return be_return(vm);
    }
    return be_returnnil(vm);
}

void be_loadfilelib(bvm *vm)
{
    be_regcfunc(vm, "open", m_open);
}
