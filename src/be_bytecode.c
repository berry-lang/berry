#include "be_bytecode.h"
#include "be_string.h"
#include "be_func.h"
#include "be_exec.h"
#include "be_mem.h"
#include "be_sys.h"
#include "be_vm.h"
#include "be_gc.h"

#define BYTECODE_VERSION    0
#define VERIFY_CODE         0x5A
#define MAGIC_NUMBER        0xBECD

static void _save_u8(void *fp, uint8_t value)
{
    be_fwrite(fp, &value, 1);
}

static void _save_u16(void *fp, uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = value & 0xff;
    buffer[1] = value >> 8;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void _save_u32(void *fp, uint32_t value)
{
    uint8_t buffer[4];
    buffer[0] = value & 0xff;
    buffer[1] = (value >> 8) & 0xff;
    buffer[2] = (value >> 16) & 0xff;
    buffer[3] = (value >> 24) & 0xff;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void _save_header(void *fp)
{
    uint8_t buffer[8] = { 0 };
    buffer[0] = MAGIC_NUMBER & 0xff;
    buffer[1] = MAGIC_NUMBER >> 8;
    buffer[2] = BYTECODE_VERSION;
    buffer[3] = VERIFY_CODE;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void _save_string(void *fp, bstring *s)
{
    if (s) {
        uint16_t length = str_len(s);
        const char *data = str(s);
        _save_u16(fp, length);
        be_fwrite(fp, data, length);
    }
}

static void _save_bytecode(void *fp, bproto *proto)
{
    binstruction *code = proto->code, *end = code + proto->codesize;
    _save_u32(fp, (uint32_t)proto->codesize);
    while (code < end) {
        _save_u32(fp, (uint32_t)*code++);
    }
}

static void _save_proto(void *fp, bproto *proto)
{
    if (proto) {
        _save_string(fp, proto->name); /* name */
        _save_u8(fp, proto->argc); /* argc */
        _save_u8(fp, proto->nstack); /* nstack */
        _save_bytecode(fp, proto); /* bytecode */
    }
}

void be_bytecode_save(const char *filename, bproto *proto)
{
    void *fp = be_fopen(filename, "w");
    _save_header(fp);
    _save_proto(fp, proto);
    be_fclose(fp);
}

static uint8_t _load_u8(void *fp)
{
    uint8_t buffer[1];
    be_fread(fp, buffer, 1);
    return buffer[0];
}

static uint16_t _load_u16(void *fp)
{
    uint8_t buffer[2];
    be_fread(fp, buffer, sizeof(buffer));
    return ((uint16_t)buffer[1] << 8) | buffer[0];
}

static uint32_t _load_u32(void *fp)
{
    uint8_t buffer[4];
    be_fread(fp, buffer, sizeof(buffer));
    return ((uint32_t)buffer[3] << 24) | ((uint32_t)buffer[2] << 16) |
        ((uint32_t)buffer[1] << 8) | buffer[0];
}

static void _load_head(void *fp)
{
    uint8_t buffer[8] = { 0 };
    be_fread(fp, buffer, sizeof(buffer));
}

static bstring *_load_string(bvm *vm, void *fp)
{
    uint16_t len = _load_u16(fp);
    char *buf = be_malloc(vm, len);
    be_fread(fp, buf, len);
    return be_newstrn(vm, buf, len);
}

static void _load_bytecode(bvm *vm, void *fp, bproto *proto)
{
    int size = (int)_load_u32(fp);
    binstruction *end, *code = be_malloc(vm, sizeof(binstruction) * size);
    proto->codesize = size;
    proto->code = code;
    for (end = code + size; code < end; ++code) {
        *code = (binstruction)_load_u32(fp);
    }
}

static void _load_proto(bvm *vm, void *fp, bclosure *cl)
{
    bproto *proto = be_newproto(vm);
    cl->proto = proto;
    proto->name = _load_string(vm, fp);
    proto->argc = _load_u8(fp);
    proto->nstack = _load_u8(fp);
    _load_bytecode(vm, fp, proto);
}

bclosure* be_bytecode_load(bvm *vm, const char *filename)
{
    void *fp = be_fopen(filename, "r");
    bclosure *cl = be_newclosure(vm, 0);
    var_setclosure(vm->top, cl);
    be_gc_setpause(vm, 0);
    be_stackpush(vm);
    _load_head(fp);
    _load_proto(vm, fp, cl);
    be_stackpop(vm, 1);
    be_gc_setpause(vm, 1);
    be_fclose(fp);
    return cl;
}
