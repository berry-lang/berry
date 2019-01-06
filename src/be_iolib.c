#include "be_iolib.h"
#include "be_mem.h"

#define READLINE_STEP       100

static int l_input(bvm *vm)
{
    size_t pos = 0, size = READLINE_STEP;
    char *buffer = be_malloc(size);
    char *res = be_fgets(stdin, buffer, size);
    while (res) {
        pos += strlen(buffer + pos) - 1;
        if (!pos || buffer[pos] == '\n') {
            break;
        }
        size += READLINE_STEP;
        buffer = be_realloc(buffer, size);
        res = be_fgets(stdin, buffer + pos + 1, READLINE_STEP);
    }
    be_pushstring(vm, buffer);
    return be_return(vm);
}

void be_loadiolib(bvm *vm)
{
    be_regcfunc(vm, "input", l_input);
}
