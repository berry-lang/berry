#include "berry.h"
#include "be_repl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* using GNU/readline library */
#if defined(USE_READLINE_LIB)
    #include <readline/readline.h>
    #include <readline/history.h>
#endif

#if !BE_USE_SCRIPT_COMPILER || \
    !BE_USE_BYTECODE_SAVER || !BE_USE_BYTECODE_LOADER
#error incomplete function configuration
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
#define repl_prelude                                                \
    FULL_VERSION " (build in " __DATE__ ", " __TIME__ ")\n"         \
    "[" COMPILER "] on " OS_NAME " (default)\n"                     \

/* command help information */
#define help_information                                            \
    "Usage: berry [options] [script [args]]\n"                      \
    "Avilable options are:\n"                                       \
    "  -i        enter interactive mode after executing 'file'\n"   \
    "  -b        load code from bytecode 'file'\n"                  \
    "  -c <file> compile script 'file' to bytecode file\n"          \
    "  -o <file> save bytecode to 'file'\n"                         \
    "  -v        show version information\n"                        \
    "  -h        show help information\n\n"                         \
    "For more information, please see:\n"                           \
    "  <https://github.com/skiars/berry>.\n"

#define array_count(a) (sizeof(a) / sizeof((a)[0]))

#define arg_i       (1 << 0)
#define arg_b       (1 << 1)
#define arg_c       (1 << 2)
#define arg_o       (1 << 3)
#define arg_h       (1 << 4)
#define arg_v       (1 << 5)
#define arg_err     (1 << 7)

struct arg_opts {
    int idx;
    const char *pattern;
    const char *optarg;
    const char *errarg;
    const char *src;
    const char *dst;
};

/* check if the character is a letter */
static int is_letter(int ch)
{
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

/* matching options
 * pattern: pattern string, the set of vaild options
 * ch: option character to be matched
 * */
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

/* read an option from the arguments
 * opt: option match state
 * argc: the number of arguments
 * argv: the arguments list
 * */
static int arg_getopt(struct arg_opts *opt, int argc, char *argv[])
{
    if (opt->idx < argc) {
        char *arg = argv[opt->idx];
        if (arg[0] == '-' && strlen(arg) == 2) {
            const char *res = match_opt(opt->pattern, arg[1]);
            /* the '?' indicates an optional argument after the option */
            if (++opt->idx < argc && res != NULL
                && res[1] == '?' && *argv[opt->idx] != '-') {
                opt->optarg = argv[opt->idx++]; /* save the argument */
                return *res;
            }
            opt->optarg = NULL;
            opt->errarg = arg;
            return res != NULL ? *res : '?';
        }
    }
    return 0;
}

/* portable readline function package */
static char* get_line(const char *prompt)
{
#if defined(USE_READLINE_LIB)
    char *line = readline(prompt);
    if (line && strlen(line)) {
        add_history(line);
    }
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

static void free_line(char *ptr)
{
#if defined(USE_READLINE_LIB)
    free(ptr);
#else
    (void)ptr;
#endif
}

/* execute a script file and output a result or error */
static int dofile(bvm *vm, const char *name, int args)
{
    int res;
    /* load bytecode file or compile script file */
    if (args & arg_b) { /* load a bytecode file */
        res = be_loadexec(vm, name);
    } else { /* load a script file */
        res = be_loadfile(vm, name);
    }
    if (res == BE_OK) { /* parsing succeeded */
        res = be_pcall(vm, 0); /* execute */
    }
    switch (res) {
    case BE_OK: /* everything is OK */
        return 0;
    case BE_EXCEPTION: /* uncatched exception */
        be_dumpexcept(vm);
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

/* load a Berry file and execute
 * args: the enabled options mask
 * */
static int load_file(bvm *vm, int argc, char *argv[], int args)
{
    int res = 0;
    int repl_mode = args & arg_i || (args == 0 && argc == 0);
    if (repl_mode) { /* enter the REPL mode after executing the script file */
        be_writestring(repl_prelude);
    }
    if (argc > 0) { /* check file path argument */
        res = dofile(vm, argv[0], args);
    }
    if (repl_mode) { /* enter the REPL mode */
        res = be_repl(vm, get_line, free_line);
        if (res == -BE_MALLOC_FAIL) {
            be_writestring("error: memory allocation failed.\n");
        }
    }
    return res;
}

static int build_file(bvm *vm, const char *dst, const char *src)
{
    int res = be_loadfile(vm, src); /* compile script file */
    if (res == BE_OK) {
        if (!dst) dst = "a.out"; /* the default output file name */
        res = be_saveexec(vm, dst); /* execute */
    }
    switch (res) {
    case BE_OK:
        return 0;
    case BE_EXCEPTION: /* uncatched exception */
        be_dumpexcept(vm);
        return 1;
    case BE_MALLOC_FAIL:
        be_writestring("error: memory allocation failed.\n");
        return -1;
    default: /* unkonw result */
        return 2;
    }
}

static int parse_arg(struct arg_opts *opt, int argc, char *argv[])
{
    int ch, args = 0;
    opt->idx = 1;
    while ((ch = arg_getopt(opt, argc, argv)) != '\0') {
        switch (ch) {
        case 'h': args |= arg_h; break;
        case 'v': args |= arg_v; break;
        case 'i': args |= arg_i; break;
        case 'b': args |= arg_b; break;
        case '?': return args | arg_err;
        case 'c':
            args |= arg_c;
            opt->src = opt->optarg;
            break;
        case 'o':
            args |= arg_o;
            opt->dst = opt->optarg;
            break;
        default:
            break;
        }
    }
    return args;
}

/* 
 * command format: berry [options] [script [args]]
 *  command options:
 *   -i: enter interactive mode after executing 'script'
 *   -b: load code from bytecode file
 * command format: berry options
 *  command options:
 *   -v: show version information
 *   -h: show help information
 * command format: berry option file [option file]
 *  command options:
 *   -c: compile script file to bytecode file
 *   -o: set the output file name
 * */
static int analysis_args(bvm *vm, int argc, char *argv[])
{
    int args = 0;
    struct arg_opts opt = { 0 };
    opt.pattern = "vhibc?o?";
    args = parse_arg(&opt, argc, argv);
    if (args & arg_err) {
        be_writestring(be_pushfstring(vm,
            "error: missing argument to '%s'\n", opt.errarg));
        be_pop(vm, 1);
        return -1;
    }
    if (args & arg_v) {
        be_writestring(FULL_VERSION "\n");
    }
    if (args & arg_h) {
        be_writestring(help_information);
    }
    if (args & (arg_c | arg_o)) {
        if (!opt.src && opt.idx < argc) {
            opt.src = argv[opt.idx];
        }
        return build_file(vm, opt.dst, opt.src);
    }
    return load_file(vm, argc - opt.idx, argv + opt.idx, args);
}

/* TODO: more paths & support more OS */
#define BERRY_ROOT "/usr/local"
static const char *module_paths[] = {
    BERRY_ROOT "/lib/berry/packages",
};

static void berry_paths(bvm * vm)
{
    size_t i;
    for (i = 0; i < array_count(module_paths); ++i) {
        be_module_path_set(vm, module_paths[i]);
    }
}

int main(int argc, char *argv[])
{
    int res;
    bvm *vm = be_vm_new(); /* create a virtual machine instance */
    berry_paths(vm);
    res = analysis_args(vm, argc, argv);
    be_vm_delete(vm); /* free all objects and vm */
    return res;
}
