#include "be_bytecode.h"
#include "be_vector.h"
#include "be_string.h"
#include "be_func.h"
#include "be_exec.h"
#include "be_mem.h"
#include "be_sys.h"
#include "be_var.h"
#include "be_vm.h"
#include "be_gc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define BYTECODE_VERSION    0
#define VERIFY_CODE         0x5A
#define MAGIC_NUMBER        0xBECD

static void save_proto(void *fp, bproto *proto);
static void load_proto(bvm *vm, void *fp, bproto **proto);

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
    binstruction *code = proto->code, *end;
    save_long(fp, (uint32_t)proto->codesize);
    for (end = code + proto->codesize; code < end; ++code) {
        save_long(fp, (uint32_t)*code);
    }
}

static void save_constant(void *fp, bproto *proto)
{
    bvalue *v = proto->ktab, *end;
    save_long(fp, proto->nconst); /* constants count */
    for (end = v + proto->nconst; v < end; ++v) {
        save_value(fp, v);
    }
}

static void save_proto_table(void *fp, bproto *proto)
{
    bproto **p = proto->ptab, **end;
    save_long(fp, proto->nproto); /* proto count */
    for (end = p + proto->nproto; p < end; ++p) {
        save_proto(fp, *p);
    }
}

static void save_upvals(void *fp, bproto *proto)
{
    bupvaldesc *uv = proto->upvals, *end;
    save_byte(fp, proto->nupvals); /* upvals count */
    for (end = uv + proto->nupvals; uv < end; ++uv) {
        save_byte(fp, uv->instack);
        save_byte(fp, uv->idx);
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
        save_proto_table(fp, proto); /* proto table */
        save_upvals(fp, proto); /* upvals description table */
    }
}

void save_global_info(bvm *vm, void *fp)
{
    int count = be_global_count(vm);
    save_long(fp, count);
}

void be_bytecode_save(bvm *vm, const char *filename, bproto *proto)
{
    void *fp = be_fopen(filename, "w");
    save_header(fp);
    save_global_info(vm, fp);
    save_proto(fp, proto);
    be_fclose(fp);
}

static uint8_t load_byte(void *fp)
{
    uint8_t buffer[1];
    if (be_fread(fp, buffer, sizeof(buffer)) == sizeof(buffer)) {
        return buffer[0];
    }
    return 0;
}

static uint16_t load_word(void *fp)
{
    uint8_t buffer[2];
    if (be_fread(fp, buffer, sizeof(buffer)) == sizeof(buffer)) {
        return ((uint16_t)buffer[1] << 8) | buffer[0];
    }
    return 0;
}

static uint32_t load_long(void *fp)
{
    uint8_t buffer[4];
    if (be_fread(fp, buffer, sizeof(buffer)) == sizeof(buffer)) {
        return ((uint32_t)buffer[3] << 24)
            | ((uint32_t)buffer[2] << 16)
            | ((uint32_t)buffer[1] << 8)
            | buffer[0];
    }
    return 0;
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

static bstring* load_string(bvm *vm, void *fp)
{
    uint16_t len = load_word(fp);
    if (len > 0) {
        bstring *str;
        char *buf = be_malloc(vm, len);
        be_fread(fp, buf, len);
        str = be_newstrn(vm, buf, len);
        be_free(vm, buf, len);
        return str;
    }
    return be_newstr(vm, "");
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
    if (size) {
        binstruction *code, *end;
        proto->code = be_malloc(vm, sizeof(binstruction) * size);
        proto->codesize = size;
        code = proto->code;
        for (end = code + size; code < end; ++code) {
            *code = (binstruction)load_long(fp);
        }
    }
}

static void load_constant(bvm *vm, void *fp, bproto *proto)
{
    int size = (int)load_long(fp); /* nconst */
    if (size) {
        bvalue *end, *v = be_malloc(vm, sizeof(bvalue) * size);
        memset(v, 0, sizeof(bvalue) * size);
        proto->ktab = v;
        proto->nconst = size;
        for (end = v + size; v < end; ++v) {
            load_value(vm, fp, v);
        }
    }
}

static void load_proto_table(bvm *vm, void *fp, bproto *proto)
{
    int size = (int)load_long(fp); /* proto count */
    if (size) {
        bproto **p = be_malloc(vm, sizeof(bproto *) * size);
        memset(p, 0, sizeof(bproto *) * size);
        proto->ptab = p;
        proto->nproto = size;
        while (size--) {
            load_proto(vm, fp, p++);
        }
    }
}

static void load_upvals(bvm *vm, void *fp, bproto *proto)
{
    int size = (int)load_byte(fp);
    if (size) {
        bupvaldesc *uv, *end;
        proto->upvals = be_malloc(vm, sizeof(bupvaldesc) * size);
        proto->nupvals = size;
        uv = proto->upvals;
        for (end = uv + size; uv < end; ++uv) {
            uv->instack = load_byte(fp);
            uv->idx = load_byte(fp);
        }
    }
}

static void load_proto(bvm *vm, void *fp, bproto **proto)
{
    *proto = be_newproto(vm);
    (*proto)->name = load_string(vm, fp);
    (*proto)->argc = load_byte(fp);
    (*proto)->nstack = load_byte(fp);
    load_bytecode(vm, fp, *proto);
    load_constant(vm, fp, *proto);
    load_proto_table(vm, fp, *proto);
    load_upvals(vm, fp, *proto);
}

void load_global_info(bvm *vm, void *fp)
{
    int i, count = load_long(fp);
    for (i = 0; i < count; ++i) {
        char buf[16];
        sprintf(buf, "g%d", i);
        be_global_new(vm, be_newstr(vm, buf));
    }
}

bclosure* be_bytecode_load(bvm *vm, const char *filename)
{
    void *fp = be_fopen(filename, "r");
    bclosure *cl = be_newclosure(vm, 0);
    var_setclosure(vm->top, cl);
    be_stackpush(vm);
    load_head(fp);
    load_global_info(vm, fp);
    load_proto(vm, fp, &cl->proto);
    be_stackpop(vm, 1);
    be_fclose(fp);
    return cl;
}
