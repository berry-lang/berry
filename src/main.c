#include "berry.h"
#include "be_repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(__linux) || defined(__unix) || defined(__APPLE__)
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

#if defined(__linux)
    #define OS_NAME   "Linux"
#elif defined(__unix)
    #define OS_NAME   "Unix"
#elif defined(__APPLE__)
    #define OS_NAME   "macOS"
#elif defined(WINVER)
    #define OS_NAME   "Windows"
#else
    #define OS_NAME   "Unknown OS"
#endif

#if defined(__GNUC__)
    #define COMPILER  "GCC " __VERSION__
#elif defined(__clang__)
    #define COMPILER  "clange" __clang_version__
#elif defined(_MSC_VER)
    #define COMPILER  "MSVC"
#else
    #define COMPILER  "Unknown Compiler"
#endif

static void dofile(bvm *vm, const char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (fp) {
        int res;
        size_t len;
        char *buffer;

        fseek(fp,0L,SEEK_END);
        len = ftell(fp);
        buffer = malloc(len);
        fseek(fp, 0L, SEEK_SET);
        len = fread(buffer, 1, len, fp);
        fclose(fp);
        res = be_loadbuffer(vm, fname, buffer, len) || be_pcall(vm, 0);
        free(buffer);
        if (res) {
            printf("%s\n", be_tostring(vm, -1));
        }
    } else {
        printf("error: can not open file '%s'.\n", fname);
    }
}

static const char* get_line(const char *prompt)
{
#if defined(__linux) || defined(__unix) || defined(__APPLE__)
    const char *line = readline(prompt);
    add_history(line);
    return line;
#else
    static char buffer[1000];
    printf(prompt);
    if (fgets(buffer, sizeof(buffer), stdin)) {
        return buffer;
    }
    return NULL;
#endif
}

int main(int argc, char *argv[])
{
    bvm *vm = be_newvm(32);
    be_loadlibs(vm);
    srand((unsigned)time(NULL));
    if (argc >= 2) {
        dofile(vm, argv[1]);
    } else {
        be_printf("Berry " BERRY_VERSION
                  " (build in " __DATE__ ", " __TIME__ ")\n");
        be_printf("[" COMPILER "] on " OS_NAME " (default)\n");
        be_repl(vm, get_line);
    }
    return 0;
}
