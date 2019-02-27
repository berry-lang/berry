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
#include <string.h>

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
    case OP_MOVE: case OP_SETSUPER: case OP_NEG: case OP_IMPORT:
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
        printf("%s\t%d\tR%d\n", be_opcode2str(op), IGET_RA(ins), IGET_RKB(ins));
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
#if BE_RUNTIME_DEBUG_INFO
    blineinfo *lineinfo = proto->lineinfo;
#endif
    for (pc = 0; pc < proto->codesize; pc++) {
#if BE_RUNTIME_DEBUG_INFO
        if (pc == lineinfo->endpc) {
            printf("; line: %d\n", lineinfo->linenumber);
            ++lineinfo;
        }
#endif
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

static const char* sourceinfo(bvm *vm, char *buf, int deepth)
{
#if BE_RUNTIME_DEBUG_INFO
    int size = be_stack_count(&vm->callstack);
    bcallframe *cf = be_vector_at(&vm->callstack, size + deepth);
    bproto *proto = cast(bclosure*, var_toobj(cf->func))->proto;
    blineinfo *start = proto->lineinfo;
    blineinfo *it = start + proto->nlineinfo - 1;
    int pc;
    if (deepth == -1) {
        pc = cast_int(vm->ip - proto->code);
    } else {
        cf = be_vector_at(&vm->callstack, size + deepth + 1);
        pc = cast_int(cf->ip - proto->code);
    }
    while (it > start && it->endpc > pc) {
        --it;
    }
    sprintf(buf, "%s:%d:", str(proto->source), it->linenumber);
    return buf;
#else
    (void)vm; (void)buf; (void)deepth;
    return "<unknow source>:";
#endif
}

void be_debug_ins_info(bvm *vm)
{
    char buf[100];
    bcallframe *cf = vm->cf;
    bproto *proto = cast(bclosure*, var_toobj(cf->func))->proto;
    int pc = cast_int(vm->ip - proto->code);
    const char *srcinfo = sourceinfo(vm, buf, -1);
    size_t len = strlen(srcinfo) + strlen(str(proto->name)) + 1;
    printf("%s %s", srcinfo, str(proto->name));
    for (; len < 40 ; len += 8) {
        printf("\t");
    }
    print_inst(*vm->ip, pc);
}

static void patch_native(bvm *vm, int deepth)
{
    int size = be_stack_count(&vm->callstack);
    bcallframe *cf = be_vector_at(&vm->callstack, size + deepth);
    if (deepth == -1) {
        cf->ip = vm->ip;
    } else {
        cf->ip = cf[1].ip;
    }
}

static void tracestack(bvm *vm)
{
    int deepth, size = be_stack_count(&vm->callstack);
    for (deepth = 1; deepth <= size; ++deepth) {
        bcallframe *cf = be_vector_at(&vm->callstack, size - deepth);
        if (var_isclosure(cf->func)) {
            char buf[100];
            bclosure *cl = var_toobj(cf->func);
            be_pushfstring(vm, "\t%s in function `%s`\n",
                sourceinfo(vm, buf, -deepth), str(cl->proto->name));
        } else {
            patch_native(vm, -deepth);
            be_pushstring(vm, "\t<native>: in native function\n");
        }
        be_strconcat(vm, -2);
        be_stackpop(vm, 1);
    }
    be_pushstring(vm, "\t[C]: in ?");
    be_strconcat(vm, -2);
    be_stackpop(vm, 1);
}

static void addinfo(bvm *vm, const char *msg)
{
    bcallframe *cf = vm->cf;
    if (var_isclosure(cf->func)) {
        char buf[100];
        be_pushfstring(vm, "%s error: %s\nstack traceback:\n",
            sourceinfo(vm, buf, -1), msg);
    } else {
        patch_native(vm, -1);
        be_pushfstring(vm, "native error: %s\n", msg);
    }
    tracestack(vm);
}

void be_debug_error(bvm *vm, int errcode, const char *msg)
{
    addinfo(vm, msg);
    be_throw(vm, errcode);
}
