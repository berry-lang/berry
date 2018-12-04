#include "berry.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(__linux) || defined(__unix)
#include <readline/readline.h>
#include <readline/history.h>
#endif

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

static int try_line(bvm *vm, const char *line)
{
    int res = be_loadstring(vm, addreturn(line));
    if (res) {
        be_pop(vm, 1);
        res = be_loadstring(vm, line);
    }
    return res;
}

static const char* getl(void)
{
#if defined(__linux) || defined(__unix)
    const char *line = readline("> ");
    add_history(line);
    return line;
#else
    static char buffer[1000];
    printf("> ");
    if (fgets(buffer, sizeof(buffer), stdin)) {
        return buffer;
    }
    return NULL;
#endif
}

static void repl(bvm *vm)
{
    const char *line;
    while ((line = getl()) != NULL) {
        if (try_line(vm, line)) {
            printf("%s\n", be_tostring(vm, -1)); /* some error */
            be_pop(vm, 1);
        } else if (be_pcall(vm, 0)) { /* vm run error */
            printf("%s\n", be_tostring(vm, -1));
            be_pop(vm, 2);
        } else {
            if (!be_isnil(vm, -1)) {
                be_printvalue(vm, 0, -1);
                printf("\n");
            }
            be_pop(vm, 1);
        }
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    bvm *vm = be_newvm(32);
    be_loadlibs(vm);
    srand((unsigned)time(NULL));
    if (argc >= 2) {
        dofile(vm, argv[1]);
    } else {
        repl(vm);
    }
    return 0;
}
