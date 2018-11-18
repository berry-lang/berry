#include "berry.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

static void dofile(bvm *vm, const char *fname)
{
    char buffer[4096];
    FILE *fp = fopen(fname, "r");
    if (fp) {
        int res;
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        
        buffer[len] = '\0';
        fclose(fp);
        res = be_loadstring(vm, buffer) || be_pcall(vm, 0);
        if (res) {
            printf("%s\n", be_tostring(vm, -1));
        }
    } else {
        printf("error: can not open file '%s'.\n", fname);
    }
}

static const char* addreturn(const char *line)
{
    static char buffer[1024];
    sprintf(buffer, "return (%s)", line);
    return buffer;
}

static void repl(bvm *vm)
{
    while (1) {
        const char *line = readline("> ");
        add_history(line);
        if (be_loadstring(vm, addreturn(line))) {
            be_pop(vm, 1);
            if (be_loadstring(vm, line)) {
                printf("%s\n", be_tostring(vm, -1));
                be_pop(vm, 1);
                continue; /* error */
            }
        }
        if (be_pcall(vm, 0)) { /* run error */
            printf("%s\n", be_tostring(vm, -1));
            be_pop(vm, 2);
        } else {
            be_pop(vm, 1);
            if (!be_isnil(vm, 1)) {
                be_printvalue(vm, 0, 1);
                printf("\n");
            }
        }
    }
}

int main(int argc, char *argv[])
{
    bvm *vm = be_newvm(128);
    be_loadlibs(vm);
    if (argc >= 2) {
        dofile(vm, argv[1]);
    } else {
        repl(vm);
    }
    return 0;
}
