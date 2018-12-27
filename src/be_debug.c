#include "be_debug.h"
#include "be_func.h"
#include "be_opcode.h"
#include "be_string.h"
#include "be_class.h"
#include "be_api.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_strlib.h"
#include "be_exec.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void be_abort(void)
{
    exit(0);
}

void be_printf(const char *format, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, format);
    vprintf(format, arg_ptr);
    va_end(arg_ptr);
}

static void print_inst(binstruction ins, int pc)
{
    bopcode op = IGET_OP(ins);

    printf("%4d: ", pc);
    switch (op) {
    case OP_ADD: case OP_SUB: case OP_MUL: case OP_DIV:
    case OP_MOD: case OP_LT: case OP_LE: case OP_EQ:
    case OP_NE:  case OP_GT:  case OP_GE:
    case OP_GETMBR: case OP_SETMBR:  case OP_GETMET:
    case OP_GETIDX: case OP_SETIDX:
        printf("%s\tR%d\tR%d\tR%d\n", be_opcode2str(op), IGET_RA(ins), IGET_RKB(ins), IGET_RKC(ins));
        break;
    case OP_GETGBL: case OP_SETGBL:
        printf("%s\tR%d\tG:%d\n", be_opcode2str(op), IGET_RA(ins), IGET_Bx(ins));
        break;
    case OP_MOVE: case OP_SETSUPER: case OP_NEG:
        printf("%s\tR%d\tR%d\n", be_opcode2str(op), IGET_RA(ins), IGET_RKB(ins));
        break;
    case OP_JMP:
        printf("%s\t\t[%d]\n", be_opcode2str(op), IGET_sBx(ins) + pc + 1);
        break;
    case OP_JMPT: case OP_JMPF:
        printf("%s\tR%d\t[%d]\n", be_opcode2str(op), IGET_RA(ins), IGET_sBx(ins) + pc + 1);
        break;
    case OP_LDINT:
        printf("%s\tR%d\t%d\n", be_opcode2str(op), IGET_RA(ins), IGET_sBx(ins));
        break;
    case OP_LDBOOL:
        printf("%s\tR%d\t%d\t%d\n", be_opcode2str(op),  IGET_RA(ins), IGET_RKB(ins), IGET_RKC(ins));
        break;
    case OP_RET:
        printf("%s\t\tR%d\n", be_opcode2str(op), IGET_RKB(ins));
        break;
    case OP_GETUPV: case OP_SETUPV:
        printf("%s\tR%d\tU:%d\n", be_opcode2str(op), IGET_RA(ins), IGET_Bx(ins));
        break;
    case OP_CALL:
        printf("%s\tR%d\t%d\t%d\n", be_opcode2str(op), IGET_RA(ins), IGET_RKB(ins), IGET_RKC(ins));
        break;
    case OP_CLOSURE:
        printf("%s\tR%d\tP:%d\n", be_opcode2str(op), IGET_RA(ins), IGET_Bx(ins));
        break;
    case OP_CLOSE:
        printf("%s\t%d\n", be_opcode2str(op), IGET_RA(ins));
        break;
    default:
        printf("%s\n", be_opcode2str(op));
        break;
    }
}

void print_code(bproto *proto)
{
    int pc;
    binstruction *code = proto->code;
    for (pc = 0; pc < proto->codesize; pc++) {
        print_inst(*code++, pc);
    }
    printf("\n");
}

void printproto(bproto *proto)
{
    int i;
    printf("function '%s':\n", str(proto->name));
    print_code(proto); /* print this proto */
    for (i = 0; i < proto->nproto; ++i) {
        printproto(proto->ptab[i]);
    }
}

void be_dprintcode(bclosure *cl)
{
    printproto(cl->proto);
}

static const char* sourceinfo(char *buf, bcallframe *cf)
{
#if BE_RUNTIME_DEBUG_INFO
    bproto *proto = cast(bclosure*, var_toobj(cf->func))->proto;
    int pc = cf->ip - proto->code;
    blineinfo *start = proto->lineinfo;
    blineinfo *it = start + proto->nlineinfo - 1;
    while (it > start && it->endpc > pc) {
        --it;
    }
    sprintf(buf, "%s:%d:", str(proto->source), it->linenumber);
    return buf;
#else
    (void)buf; (void)cf;
    return "<unknow source>:";
#endif
}

static void tracestack(bvm *vm)
{
    bcallframe *cf = be_stack_top(&vm->callstack);
    bcallframe *base = be_stack_base(&vm->callstack);
    for (; cf >= base; --cf) {
        if (var_isclosure(cf->func)) {
            char buf[100];
            bclosure *cl = var_toobj(cf->func);
            be_pushfstring(vm, "\t%s in function `%s`\n",
                sourceinfo(buf, cf), str(cl->proto->name));
        } else {
            be_pushstring(vm, "\t<native>: in native function\n");
        }
        be_strconcat(vm, -2);
        be_pop(vm, 1);
    }
    be_pushstring(vm, "\t[C]: in ?");
    be_strconcat(vm, -2);
    be_pop(vm, 1);
}

void addinfo(bvm *vm, const char *msg)
{
    bcallframe *cf = vm->cf;
    if (var_isclosure(cf->func)) {
        char buf[100];
        be_pushfstring(vm, "%s error: %s\nstack traceback:\n",
            sourceinfo(buf, cf), msg);
    } else {
        be_pushfstring(vm, "native error: %s\n", msg);
    }
    tracestack(vm);
}

void be_debug_error(bvm *vm, int errcode, const char *format, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, format);
    be_pushvfstr(vm, format, arg_ptr);
    va_end(arg_ptr);
    addinfo(vm, be_tostring(vm, -1));
    be_removeone(vm, -2);
    be_throw(vm, errcode);
}
