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

#if defined(__clang__)
    #define COMPILER  "clange" __clang_version__
#elif defined(__GNUC__)
    #define COMPILER  "GCC " __VERSION__
#elif defined(_MSC_VER)
    #define COMPILER  "MSVC"
#else
    #define COMPILER  "Unknown Compiler"
#endif

#define FULL_VERSION "Berry " BERRY_VERSION

#define repl_prelude                                            \
    FULL_VERSION " (build in " __DATE__ ", " __TIME__ ")\n"     \
    "[" COMPILER "] on " OS_NAME " (default)\n"                 \

#define help_information                                        \
    "usage: berry [options] [script [args]]\n"                  \
    "avilable options are:\n"                                   \
    "-i   enter interactive mode after executing 'script'\n"    \
    "-v   show version information\n"                           \
    "-h   show help information\n"

#define arg_i       1
#define arg_v       2
#define arg_h       4

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

static int dofile(bvm *vm)
{
    const char *name = be_tostring(vm, 1);
    FILE *fp = fopen(name, "r");
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
        res = be_loadbuffer(vm, name, buffer, len) || be_pcall(vm, 0);
        free(buffer);
        if (res) {
            printf("%s\n", be_tostring(vm, -1));
        }
        return 0;
    }
    printf("error: can not open file '%s'.\n", name);
    return 1;
}

int check_args(bvm *vm)
{
    int args = 0;
    while (be_top(vm) > 0) {
        const char *arg = be_tostring(vm, 1);
        if (arg[0] != '-') {
            return args;
        }
        if (strlen(arg) == 2) {
            switch (arg[1]) {
            case 'i': args |= arg_i; break;
            case 'v': args |= arg_v; break;
            case 'h': args |= arg_h; break;
            default: break;
            }
        }
        be_removeone(vm, 1);
    }
    args |= arg_i;
    return args;
}

/* berry [options] [script [args]]
 * command options:
 * -i: enter interactive mode after executing 'script'
 * -v: show version information
 * -h: show help information
 * */
static int analysis_args(bvm *vm)
{
    int args = check_args(vm);
    if (args & arg_v) {
        be_printf(FULL_VERSION "\n");
        return 0;
    }
    if (args & arg_h) {
        be_printf(help_information);
        return 0;
    }
    if (args & arg_i) {
        be_printf(repl_prelude);
    }
    if (be_top(vm) > 0) {
        int res = dofile(vm);
        if (res) {
            return res;
        }
        be_pop(vm, be_top(vm));
    }
    if (args & arg_i) {
        be_repl(vm, get_line);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int i = 0, res;
    bvm *vm = be_vm_new(32);
    be_loadlibs(vm);
    srand((unsigned)time(NULL)); /* Set the random seed to system time */
    for (i = 1; i < argc; ++i) {
        be_pushstring(vm, argv[i]);
    }
    res = analysis_args(vm);
    be_vm_delete(vm);
    return res;
}
