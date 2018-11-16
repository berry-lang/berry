#include "berry.h"
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

static void dofile(const char *fname)
{
    char buffer[4096];
    FILE *fp = fopen(fname, "r");
    if (fp) {
        bvm *vm = be_newvm(64);
        size_t len = fread(buffer, 1, sizeof(buffer), fp);
        buffer[len] = '\0';

        fclose(fp);
        be_loadlibs(vm);
        be_dostring(vm, buffer);
    } else {
        printf("error: can not open file '%s'.\n", fname);
    }
}

static void repl(void)
{
    const char *line;
    bvm *vm = be_newvm(64);
    be_loadlibs(vm);

    while (1) {
        line = readline("> ");
        add_history(line);
        be_dostring(vm, line);
    }
}

int main(int argc, char *argv[])
{
    if (argc >= 2) {
        dofile(argv[1]);
    } else {
        repl();
    }
    return 0;
}
