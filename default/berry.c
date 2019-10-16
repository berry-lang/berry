#include "berry.h"
#include "be_repl.h"
#include <stdio.h>
#include <string.h>

/* using GNU/readline library */
#if defined(USE_READLINE_LIB)
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

/* detect operating system name */
#if defined(__linux)
    #define OS_NAME   "Linux"
#elif defined(__unix)
    #define OS_NAME   "Unix"
#elif defined(__APPLE__)
    #define OS_NAME   "macOS"
#elif defined(_WIN32)
    #define OS_NAME   "Windows"
#else
    #define OS_NAME   "Unknown OS"
#endif

/* detect compiler name and version */
#if defined(__clang__)
    #define COMPILER  "clang " __clang_version__
#elif defined(__GNUC__)
    #define COMPILER  "GCC " __VERSION__
#elif defined(_MSC_VER)
    #define COMPILER  "MSVC"
#elif defined(__CC_ARM)
    #define COMPILER  "ARMCC"
#elif defined(__ICCARM__)
    #define COMPILER  "IAR"
#else
    #define COMPILER  "Unknown Compiler"
#endif

#if BE_DEBUG
#define FULL_VERSION "Berry " BERRY_VERSION " (debug)"
#else
#define FULL_VERSION "Berry " BERRY_VERSION
#endif

/* prompt message when REPL is loaded */
#define repl_prelude                                            \
    FULL_VERSION " (build in " __DATE__ ", " __TIME__ ")\n"     \
    "[" COMPILER "] on " OS_NAME " (default)\n"                 \

/* command help information */
#define help_information                                        \
    "usage: berry [options] [script [args]]\n"                  \
    "avilable options are:\n"                                   \
    "-i   enter interactive mode after executing 'script'\n"    \
    "-v   show version information\n"                           \
    "-h   show help information\n"

#define arg_i       1
#define arg_v       2
#define arg_h       4

/* portable readline function package */
static const char* get_line(const char *prompt)
{
#if defined(USE_READLINE_LIB)
    const char *line = readline(prompt);
    add_history(line);
    return line;
#else
    static char buffer[1000];
    fputs(prompt, stdout);
    fflush(stdout);
    if (be_readstring(buffer, sizeof(buffer))) {
        buffer[strlen(buffer) - 1] = '\0';
        return buffer;
    }
    return NULL;
#endif
}

/* execute a script file and output a result or error */
static int dofile(bvm *vm)
{
    const char *name = be_tostring(vm, 1); /* get script file path */
    int res = be_loadfile(vm, name); /* compile */
    res = res == BE_OK ? be_pcall(vm, 0) : res; /* execute */
    switch (res) {
    case BE_OK:
        return 0;
    case BE_IO_ERROR: /* IO error */
    case BE_SYNTAX_ERROR: /* syntax error */
    case BE_EXEC_ERROR: /* VM runtime error */
        be_writestring(be_tostring(vm, -1)); /* print the error message */
        be_writenewline();
        return 1;
    case BE_EXIT: /* return exit code */
        return be_toindex(vm, -1);
    case BE_MALLOC_FAIL:
        be_writestring("error: memory allocation failed.\n");
        return -1;
    default: /* unkonw result */
        return 2;
    }
}

/* processing options arguments */
static int check_args(bvm *vm)
{
    int args = 0;
    while (be_top(vm) > 0) {
        const char *arg = be_tostring(vm, 1);
        if (arg[0] != '-') { /* non-option argument */
            return args;
        }
        if (strlen(arg) == 2) {
            switch (arg[1]) { /* update option */
            case 'i': args |= arg_i; break;
            case 'v': args |= arg_v; break;
            case 'h': args |= arg_h; break;
            default: break;
            }
        }
        be_remove(vm, 1); /* remove the processed argument */
    }
    args |= arg_i;
    return args;
}

struct arg_opts {
    int index;
    const char *pattern;
    const char *optarg;
    const char *errarg;
};

static int is_letter(int ch)
{
    return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
}

static const char* match_opt(const char *pattern, int ch)
{
    int c = '\0';
    if (pattern) {
        while ((c = *pattern) != '\0' && c != ch) {
            c = *(++pattern);
            while (c != '\0' && !is_letter(c)) {
                c = *(++pattern); /* skip characters that are not letters */
            }
        }
    }
    return c == ch ? pattern : NULL;
}

static int arg_getopt(struct arg_opts *opt, int argc, char *argv[])
{
    if (opt->index < argc) {
        char *arg = argv[opt->index++];
        if (arg[0] == '-' && strlen(arg) == 2) {
            const char *res = match_opt(opt->pattern, arg[1]);
            if (res != NULL) {
                if (res[1] == ':') { /* get argument */
                    opt->optarg =
                        opt->index < argc ? argv[opt->index++] : NULL;
                } else if (res[1] == '?') {
                    if (opt->index < argc && argv[opt->index][0] != '-') {
                        opt->optarg = argv[opt->index++];
                    } else {
                        opt->optarg = NULL;
                    }
                }
                return *res;
            }
            opt->errarg = arg;
            return '?';
        }
    }
    return 0;
}

/* 
 * command format: berry [options] [script [args]]
 *   command options:
 *   -i: enter interactive mode after executing 'script'
 *   -b: load code from bytecode file
 * command format: berry options
 *   command options:
 *   -v: show version information
 *   -h: show help information
 * command format: berry option file [option file]
 *   -c: compile script file to bytecode file
 *   -o: set the output file name
 * */
static int analysis_args(bvm *vm, int argc, char *argv[])
{
    int ch;
    struct arg_opts opt = { 0 };
    opt.index = 1;
    opt.pattern = "ibvhc:o:";
    while ((ch = arg_getopt(&opt, argc, argv)) != '\0') {
        switch (ch) {
        case 'h':
            be_writestring(help_information);
            break;
        case 'v':
            be_writestring(FULL_VERSION "\n");
            break;
        case 'i':
            break;
        case '?':
            printf("unknown option: %s\n", opt.errarg);
            break;
        }
    }
    return 0;
}
#if 0
static int analysis_args(bvm *vm)
{
    int args = check_args(vm);
    if (args & arg_v) { /* query version information */
        be_writestring(FULL_VERSION "\n");
        return 0;
    }
    if (args & arg_h) { /* query help information */
        be_writestring(help_information);
        return 0;
    }
    if (args & arg_i) { /* enter the REPL mode after executing the script file */
        be_writestring(repl_prelude);
    }
    if (be_top(vm) > 0) { /* check file path argument */
        int res = dofile(vm);
        if (res && !(args & arg_i)) {
            return res;
        }
        be_pop(vm, be_top(vm)); /* clear the stack */
    }
    if (args & arg_i) { /* enter the REPL mode */
        int res = be_repl(vm, get_line);
        if (res == -BE_MALLOC_FAIL) {
            be_writestring("error: memory allocation failed.\n");
        }
        return res;
    }
    return 0;
}
#endif

int main(int argc, char *argv[])
{
    int res;
    bvm *vm = be_vm_new(); /* create a virtual machine instance */
    res = analysis_args(vm, argc, argv);
    be_vm_delete(vm); /* free all objects and vm */
    return res;
}
