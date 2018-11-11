#include <stdio.h>
#include <string.h>
#include "be_debug.h"
#include "be_parser.h"
#include "be_vm.h"
#include "be_libs.h"

static void bparserest(const char *fname, char *buffer)
{
    bclosure *cl;
    bvm *vm = be_vm_new(16);
    be_loadlibs(vm);
    cl = be_parser_source(vm, fname, buffer);
#if 0
    be_dprintcode(cl);
    be_printf("VM Log:\n");
#endif
    be_dofunc(vm, cl, 1);
}

int main(int argc, char *argv[])
{
    char buffer[4096];
    const char *fname = argc < 2 ? "./test.be" : argv[1];
    FILE *fp = fopen(fname, "r");

    if (fp) {
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        buffer[len] = '\0';
        fclose(fp);
        bparserest(fname, buffer);
    } else {
        printf("error: can not open file '%s'.\n", fname);
    }
    return 0;
}
