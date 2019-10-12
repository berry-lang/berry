#include "be_bytecode.h"
#include "be_string.h"
#include "be_func.h"
#include "be_sys.h"
#include "be_vm.h"

#define BYTECODE_VERSION    0
#define VERIFY_CODE         0x5A
#define MAGIC_NUMBER        0xBECD

static void _build_header(void *fp)
{
    uint8_t buffer[8] = { 0 };
    buffer[0] = MAGIC_NUMBER & 0xff;
    buffer[1] = MAGIC_NUMBER >> 8;
    buffer[2] = BYTECODE_VERSION;
    buffer[3] = VERIFY_CODE;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void _build_u8(void *fp, uint8_t value)
{
    be_fwrite(fp, &value, 1);
}

static void _build_u16(void *fp, uint16_t value)
{
    uint8_t buffer[2];
    buffer[0] = value & 0xff;
    buffer[1] = value >> 8;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void _build_u32(void *fp, uint32_t value)
{
    uint8_t buffer[4];
    buffer[0] = value & 0xff;
    buffer[1] = (value >> 8) & 0xff;
    buffer[2] = (value >> 16) & 0xff;
    buffer[3] = (value >> 24) & 0xff;
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void _build_string(void *fp, bstring *s)
{
    if (s) {
        uint16_t length = str_len(s);
        const char *data = str(s);
        _build_u16(fp, length);
        be_fwrite(fp, data, length);
    }
}

static void _build_bytecode(void *fp, bproto *proto)
{
    binstruction *code = proto->code, *end = code + proto->codesize;
    _build_u32(fp, (uint32_t)proto->codesize);
    while (code < end) {
        _build_u32(fp, (uint32_t)*code++);
    }
}

static void _build_proto(void *fp, bproto *proto)
{
    if (proto) {
        _build_string(fp, proto->name); /* name */
        _build_u8(fp, proto->argc); /* argc */
        _build_u8(fp, proto->nstack); /* nstack */
        _build_bytecode(fp, proto); /* bytecode */
    }
}

void be_bytecode_build(const char *filename, bproto *proto)
{
    void *fp = be_fopen(filename, "w");
    _build_header(fp);
    _build_proto(fp, proto);
    be_fclose(fp);
}

static void _load_proto(bvm *vm, void *fp, bclosure *cl)
{
    bproto *proto = be_newproto(vm);
    cl->proto = proto;
}

bclosure* be_bytecode_load(bvm *vm, const char *filename)
{
    void *fp = be_fopen(filename, "r");
    bclosure *cl = be_newclosure(vm, 0);
    var_setclosure(vm->top, cl);
    be_stackpush(vm);
    _load_proto(vm, fp, cl);
    be_stackpop(vm, 1);
    be_fclose(fp);
    return cl;
}
