#include "be_bytecode.h"
#include "be_string.h"
#include "be_func.h"
#include "be_exec.h"
#include "be_mem.h"
#include "be_sys.h"
#include "be_vm.h"
#include "be_gc.h"
#include <string.h>
#include <stdio.h>

#define BYTECODE_VERSION    0
#define VERIFY_CODE         0x5A
#define MAGIC_NUMBER        0xBECD

static void save_byte(void *fp, uint8_t value)
{
    be_fwrite(fp, &value, 1);
}

static void save_word(void *fp, uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = value & 0xff;
    buffer[1] = value >> 8;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void save_long(void *fp, uint32_t value)
{
    uint8_t buffer[4];
    buffer[0] = value & 0xff;
    buffer[1] = (value >> 8) & 0xff;
    buffer[2] = (value >> 16) & 0xff;
    buffer[3] = (value >> 24) & 0xff;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void save_header(void *fp)
{
    uint8_t buffer[8] = { 0 };
    buffer[0] = MAGIC_NUMBER & 0xff;
    buffer[1] = MAGIC_NUMBER >> 8;
    buffer[2] = BYTECODE_VERSION;
    buffer[3] = VERIFY_CODE;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void save_int(void *fp, bint i)
{
    save_long(fp, i & 0xffffffff);
    save_long(fp, (i >> 32) & 0xffffffff);
}

static void save_real(void *fp, breal r)
{
    union {
        breal r;
        uint64_t i;
    } u;
    u.r = r;
    save_long(fp, u.i & 0xffffffff);
    save_long(fp, (u.i >> 32) & 0xffffffff);
}

static void save_string(void *fp, bstring *s)
{
    if (s) {
        uint16_t length = str_len(s);
        const char *data = str(s);
        save_word(fp, length);
        be_fwrite(fp, data, length);
    }
}

static void save_value(void *fp, bvalue *v)
{
    save_byte(fp, var_type(v)); /* type */
    switch (var_type(v)) {
    case BE_INT: save_int(fp, var_toint(v)); break;
    case BE_REAL: save_real(fp, var_toreal(v)); break;
    case BE_STRING: save_string(fp, var_tostr(v)); break;
    case BE_CLASS: break; /* TODO */
    default: break;
    }
}

static void save_bytecode(void *fp, bproto *proto)
{
    binstruction *code = proto->code, *end = code + proto->codesize;
    save_long(fp, (uint32_t)proto->codesize);
    while (code < end) {
        save_long(fp, (uint32_t)*code++);
    }
}

static void save_constant(void *fp, bproto *proto)
{
    bvalue *v = proto->ktab, *end = v + proto->nconst;
    save_long(fp, proto->nconst); /* nconst */
    while (v < end) {
        save_value(fp, v++);
    }
}

static void save_proto(void *fp, bproto *proto)
{
    if (proto) {
        save_string(fp, proto->name); /* name */
        save_byte(fp, proto->argc); /* argc */
        save_byte(fp, proto->nstack); /* nstack */
        save_bytecode(fp, proto); /* bytecode */
        save_constant(fp, proto); /* constant */
    }
}

void be_bytecode_save(const char *filename, bproto *proto)
{
    void *fp = be_fopen(filename, "w");
    save_header(fp);
    save_proto(fp, proto);
    be_fclose(fp);
}

static uint8_t load_byte(void *fp)
{
    uint8_t buffer[1];
    be_fread(fp, buffer, 1);
    return buffer[0];
}

static uint16_t load_word(void *fp)
{
    uint8_t buffer[2];
    be_fread(fp, buffer, sizeof(buffer));
    return ((uint16_t)buffer[1] << 8) | buffer[0];
}

static uint32_t load_long(void *fp)
{
    uint8_t buffer[4];
    be_fread(fp, buffer, sizeof(buffer));
    return ((uint32_t)buffer[3] << 24) | ((uint32_t)buffer[2] << 16) |
        ((uint32_t)buffer[1] << 8) | buffer[0];
}

static void load_head(void *fp)
{
    uint8_t buffer[8] = { 0 };
    be_fread(fp, buffer, sizeof(buffer));
}

static bint load_int(void *fp)
{
    bint i;
    i = load_long(fp);
    i |= (bint)load_long(fp) << 32;
    return i;
}

static breal load_real(void *fp)
{
    union {
        breal r;
        uint64_t i;
    } u;
    u.i = load_long(fp);
    u.i |= (uint64_t)load_long(fp) << 32;
    return u.r;
}

static bstring *load_string(bvm *vm, void *fp)
{
    uint16_t len = load_word(fp);
    char *buf = be_malloc(vm, len);
    be_fread(fp, buf, len);
    return be_newstrn(vm, buf, len);
}

static void load_value(bvm *vm, void *fp, bvalue *v)
{
    switch (load_byte(fp)) {
    case BE_INT: var_setint(v, load_int(fp)); break;
    case BE_REAL: var_setreal(v, load_real(fp)); break;
    case BE_STRING: var_setstr(v, load_string(vm, fp)); break;
    case BE_CLASS: break; /* TODO */
    default: break;
    }
}

static void load_bytecode(bvm *vm, void *fp, bproto *proto)
{
    int size = (int)load_long(fp);
    binstruction *end, *code = be_malloc(vm, sizeof(binstruction) * size);
    proto->codesize = size;
    proto->code = code;
    for (end = code + size; code < end; ++code) {
        *code = (binstruction)load_long(fp);
    }
}

static void load_constant(bvm *vm, void *fp, bproto *proto)
{
    int nconst = (int)load_long(fp); /* nconst */
    bvalue *v = be_malloc(vm, sizeof(bvalue) * nconst), *end = v + nconst;
    memset(v, 0, sizeof(bvalue) * nconst);
    proto->ktab = v;
    proto->nconst = nconst;
    while (v < end) {
        load_value(vm, fp, v++);
    }
}

static void load_proto(bvm *vm, void *fp, bclosure *cl)
{
    bproto *proto = be_newproto(vm);
    cl->proto = proto;
    proto->name = load_string(vm, fp);
    proto->argc = load_byte(fp);
    proto->nstack = load_byte(fp);
    load_bytecode(vm, fp, proto);
    load_constant(vm, fp, proto);
}

bclosure* be_bytecode_load(bvm *vm, const char *filename)
{
    void *fp = be_fopen(filename, "r");
    bclosure *cl = be_newclosure(vm, 0);
    var_setclosure(vm->top, cl);
    be_gc_setpause(vm, 0);
    be_stackpush(vm);
    load_head(fp);
    load_proto(vm, fp, cl);
    be_stackpop(vm, 1);
    be_gc_setpause(vm, 1);
    be_fclose(fp);
    return cl;
}
