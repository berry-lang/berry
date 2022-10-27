/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_bytecode.h"
#include "be_decoder.h"
#include "be_vector.h"
#include "be_string.h"
#include "be_class.h"
#include "be_func.h"
#include "be_exec.h"
#include "be_list.h"
#include "be_map.h"
#include "be_mem.h"
#include "be_sys.h"
#include "be_var.h"
#include "be_vm.h"
#include <string.h>

#define MAGIC_NUMBER1       0xBE
#define MAGIC_NUMBER2       0xCD
#define MAGIC_NUMBER3       0xFE
#define BYTECODE_VERSION    4

#define USE_64BIT_INT       (BE_INTGER_TYPE == 2 \
    || BE_INTGER_TYPE == 1 && LONG_MAX == 9223372036854775807L)

#if !BE_USE_SCRIPT_COMPILER && BE_USE_BYTECODE_SAVER
#error bytecode generation dependent compiler (require BE_USE_SCRIPT_COMPILER != 0)
#endif

#if BE_USE_BYTECODE_SAVER || BE_USE_BYTECODE_LOADER
static void bytecode_error(bvm_t *vm, const char *msg)
{
    be_raise(vm, "io_error", msg);
}

static uint8_t vm_sizeinfo(void)
{
    uint8_t res = sizeof(bint_t) == 8;
    res |= (sizeof(breal_t) == 8) << 1;
    return res;
}
#endif

#if BE_USE_BYTECODE_SAVER
static void save_proto(bvm_t *vm, void *fp, bproto_t *proto);

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
    buffer[0] = MAGIC_NUMBER1;
    buffer[1] = MAGIC_NUMBER2;
    buffer[2] = MAGIC_NUMBER3;
    buffer[3] = BYTECODE_VERSION;
    buffer[4] = vm_sizeinfo();
    be_fwrite(fp, buffer, sizeof(buffer));
}

static void save_int(void *fp, bint_t i)
{
#if USE_64BIT_INT
    save_long(fp, i & 0xffffffff);
    save_long(fp, (i >> 32) & 0xffffffff);
#else
    save_long(fp, (uint32_t)i);
#endif
}

static void save_real(void *fp, breal_t r)
{
#if BE_USE_SINGLE_FLOAT
    union { breal_t r; uint32_t i; } u;
    u.r = r;
    save_long(fp, u.i);
#else
    union { breal_t r; uint64_t i; } u;
    u.r = r;
    save_long(fp, u.i & 0xffffffff);
    save_long(fp, (u.i >> 32) & 0xffffffff);
#endif
}

static void save_string(void *fp, bstring_t *s)
{
    if (s) {
        uint16_t length = (uint16_t)str_len(s);
        const char *data = str(s);
        save_word(fp, length);
        be_fwrite(fp, data, length);
    }
}

static bstring_t** save_members(bvm_t *vm, void *fp, bclass_t *c, int nvar)
{
    bmapnode_t *node;
    bstring_t **vars = NULL;
    bmap_t *members = c->members;
    bmapiter_t iter = be_map_iter();
    if (nvar) {
        /* allocate the member-variable name cache */
        vars = be_malloc(vm, sizeof(bstring_t *) * nvar);
    }
    while ((node = be_map_next(members, &iter)) != NULL) {
        be_assert(var_isstr(&node->key));
        if (var_isindex(&node->value)) { /* cache member name */
            if (vars == NULL) {
                return NULL; /* should never be executed */
            }
            vars[var_toidx(&node->value)] = var_tostr(&node->key);
        } else { /* save method's name and function */
            bproto_t *proto;
            bvalue_t *value = &node->value;
            be_assert(var_isclosure(value) || var_isproto(value) || var_isnil(value));
            save_string(fp, var_tostr(&node->key)); /* save method name */
            if (var_isproto(value)) { /* the method is a prototype */
                proto = var_toobj(value);
                save_proto(vm, fp, proto); /* only save prototype */
            } else if (var_isclosure(value)) { /* the method is a closure */
                proto = cast(bclosure_t *, var_toobj(value))->proto;            
                save_proto(vm, fp, proto); /* only save prototype */
            } else if (var_isnil(value)) {
                /* this is a static member (nil default) */
                save_word(fp, 0);  /* store a zero byte that will be seen as a zero length method name which is invalid */
            } else {
                be_raise(vm, "internal_error", "unsupported member in class");
                return NULL; /* should never be executed */
            }
        }
    }
    return vars;
}

static void save_class(bvm_t *vm, void *fp, bclass_t *c)
{
    bstring_t **vars;
    int i, count = be_map_count(c->members);
    int nvar = c->nvar - be_class_closure_count(c);
    save_string(fp, c->name);
    save_long(fp, nvar); /* member variables count */
    save_long(fp, count - nvar); /* method count */
    vars = save_members(vm, fp, c, nvar);
    if (vars != NULL) {
        for (i = 0; i < nvar; ++i) {
            save_string(fp, vars[i]);
        }
        be_free(vm, vars, sizeof(bstring_t *) * nvar);
    }
}

static void save_value(bvm_t *vm, void *fp, bvalue_t *v)
{
    save_byte(fp, (uint8_t)var_primetype(v)); /* type */
    switch (var_primetype(v)) {
    case BE_INT: save_int(fp, var_toint(v)); break;
    case BE_REAL: save_real(fp, var_toreal(v)); break;
    case BE_STRING: save_string(fp, var_tostr(v)); break;
    case BE_CLASS: save_class(vm, fp, var_toobj(v)); break;
    default: break;
    }
}

static void save_bytecode(bvm_t *vm, void *fp, bproto_t *proto)
{
    int forbid_gbl = comp_is_named_gbl(vm);
    binstruction_t *code = proto->code, *end;
    save_long(fp, (uint32_t)proto->codesize);
    for (end = code + proto->codesize; code < end; ++code) {
        save_long(fp, (uint32_t)*code);
        if (forbid_gbl) {   /* we are saving only named globals, so make sure we don't save OP_GETGBL or OP_SETGBL */
            if ((uint32_t)*code == OP_GETGBL || (uint32_t)*code == OP_SETGBL) {
                be_raise(vm, "internal_error", "GETGBL/SETGBL found when saving with named globals");
            }
        }
    }
}

static void save_constants(bvm_t *vm, void *fp, bproto_t *proto)
{
    bvalue_t *v = proto->ktab, *end;
    save_long(fp, proto->nconst); /* constants count */
    for (end = v + proto->nconst; v < end; ++v) {
        save_value(vm, fp, v);
    }
}

static void save_proto_table(bvm_t *vm, void *fp, bproto_t *proto)
{
    bproto_t **p = proto->ptab, **end;
    save_long(fp, proto->nproto); /* proto count */
    if (p) {
        for (end = p + proto->nproto; p < end; ++p) {
            save_proto(vm, fp, *p);
        }
    }
}

static void save_upvals(void *fp, bproto_t *proto)
{
    bupvaldesc_t *uv = proto->upvals, *end;
    save_byte(fp, proto->nupvals); /* upvals count */
    if (uv) {
        for (end = uv + proto->nupvals; uv < end; ++uv) {
            save_byte(fp, uv->instack);
            save_byte(fp, uv->idx);
        }
    }
}

static void save_proto(bvm_t *vm, void *fp, bproto_t *proto)
{
    if (proto) {
        save_string(fp, proto->name); /* name */
        save_string(fp, proto->source); /* source */
        save_byte(fp, proto->argc); /* argc */
        save_byte(fp, proto->nstack); /* nstack */
        save_byte(fp, proto->varg); /* varg */
        save_byte(fp, 0x00); /* reserved */
        save_bytecode(vm, fp, proto); /* bytecode */
        save_constants(vm, fp, proto); /* constant */
        save_proto_table(vm, fp, proto); /* proto table */
        save_upvals(fp, proto); /* upvals description table */
    }
}

static void save_globals(bvm_t *vm, void *fp)
{
    bmapnode_t *node;
    bmapiter_t iter = be_map_iter();
    bmap_t *map = vm->gbldesc.global.vtab;
    int i, count = be_global_count(vm);
    bstring_t **list = be_malloc(vm, sizeof(bstring_t*) * count);
    while ((node = be_map_next(map, &iter)) != NULL) {
        if (var_isstr(&node->key)) {
            int idx = var_toidx(&node->value);
            be_assert(idx < count);
            list[idx] = var_tostr(&node->key);
        }
    }
    for (i = 0; i < count; ++i) {
        save_string(fp, list[i]);
    }
    be_free(vm, list, sizeof(bstring_t*) * count);
}

static void save_global_info(bvm_t *vm, void *fp)
{
    save_long(fp, be_builtin_count(vm));
    if (comp_is_named_gbl(vm)) {
        /* named globals only so no need to save current globals */
        save_long(fp, 0);
    } else {
        save_long(fp, be_global_count(vm));
        save_globals(vm, fp);
    }
}

void be_bytecode_save(bvm_t *vm, const char *filename, bproto_t *proto)
{
    void *fp = be_fopen(filename, "wb");
    if (fp == NULL) {
        bytecode_error(vm, be_pushfstring(vm,
            "can not open file '%s'.", filename));
    } else {
        save_header(fp);
        save_global_info(vm, fp);
        save_proto(vm, fp, proto);
        be_fclose(fp);
    }
}
#endif /* BE_USE_BYTECODE_SAVER */

#if BE_USE_BYTECODE_LOADER
static bbool load_proto(bvm_t *vm, void *fp, bproto_t **proto, int info, int version);

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

static int load_head(void *fp)
{
    int res;
    uint8_t buffer[8] = { 0 };
    be_fread(fp, buffer, sizeof(buffer));
    res = buffer[0] == MAGIC_NUMBER1 &&
          buffer[1] == MAGIC_NUMBER2 &&
          buffer[2] == MAGIC_NUMBER3 &&
          buffer[4] == vm_sizeinfo();
    if (res) {
        return buffer[3];
    } else {
        return 0;
    }
}

bbool be_bytecode_check(const char *path)
{
    void *fp = be_fopen(path, "r");
    if (fp) {
        uint8_t buffer[3], rb;
        rb = (uint8_t)be_fread(fp, buffer, 3);
        be_fclose(fp);
        /* check magic number */
        return rb == 3 &&
            buffer[0] == MAGIC_NUMBER1 &&
            buffer[1] == MAGIC_NUMBER2 &&
            buffer[2] == MAGIC_NUMBER3;
    }
    return bfalse;
}

static bint_t load_int(void *fp)
{
#if USE_64BIT_INT
    bint_t i;
    i = load_long(fp);
    i |= (bint_t)load_long(fp) << 32;
    return i;
#else
    return load_long(fp);
#endif
}

static breal_t load_real(void *fp)
{
#if BE_USE_SINGLE_FLOAT
    union { breal_t r; uint32_t i; } u;
    u.i = load_long(fp);
    return u.r;
#else
    union {
        breal_t r;
        uint64_t i;
    } u;
    u.i = load_long(fp);
    u.i |= (uint64_t)load_long(fp) << 32;
    return u.r;
#endif
}

static bstring_t* load_string(bvm_t *vm, void *fp)
{
    uint16_t len = load_word(fp);
    if (len > 0) {
        bstring_t *str;
        char *buf = be_malloc(vm, len);
        be_fread(fp, buf, len);
        str = be_newstrn(vm, buf, len);
        be_free(vm, buf, len);
        return str;
    }
    return str_literal(vm, "");
}

static bstring_t* cache_string(bvm_t *vm, void *fp)
{
    bstring_t *str = load_string(vm, fp);
    var_setstr(vm->top, str);
    be_incrtop(vm);
    return str;
}

static void load_class(bvm_t *vm, void *fp, bvalue_t *v, int version)
{
    int nvar, count;
    bclass_t *c = be_newclass(vm, NULL, NULL);
    var_setclass(v, c);
    c->name = load_string(vm, fp);
    nvar = load_long(fp);
    count = load_long(fp);
    while (count--) { /* load method table */
        bvalue_t *value;
        bstring_t *name = cache_string(vm, fp);
        value = vm->top;
        var_setproto(value, NULL);
        be_incrtop(vm);
        if (load_proto(vm, fp, (bproto_t**)&var_toobj(value), -3, version)) {
            /* actual method */
            bbool is_method = ((bproto_t*)var_toobj(value))->varg & BE_VA_METHOD;
            be_class_method_bind(vm, c, name, var_toobj(value), !is_method);
        } else {
            /* no proto, static member set to nil */
            be_class_member_bind(vm, c, name, bfalse);
        }
        be_stackpop(vm, 2); /* pop the cached string and proto */
    }
    for (count = 0; count < nvar; ++count) { /* load member-variable table */
        bstring_t *name = cache_string(vm, fp);
        be_class_member_bind(vm, c, name, btrue);
        be_stackpop(vm, 1); /* pop the cached string */
    }
}

static void load_value(bvm_t *vm, void *fp, bvalue_t *v, int version)
{
    switch (load_byte(fp)) {
    case BE_INT: var_setint(v, load_int(fp)); break;
    case BE_REAL: var_setreal(v, load_real(fp)); break;
    case BE_STRING: var_setstr(v, load_string(vm, fp)); break;
    case BE_CLASS: load_class(vm, fp, v, version); break;
    default: break;
    }
}

static void load_bytecode(bvm_t *vm, void *fp, bproto_t *proto, int info)
{
    int size = (int)load_long(fp);
    if (size) {
        binstruction_t *code, *end;
        int bcnt = be_builtin_count(vm);
        blist_t *list = var_toobj(be_indexof(vm, info));
        be_assert(be_islist(vm, info));
        proto->code = be_malloc(vm, sizeof(binstruction_t) * size);
        proto->codesize = size;
        code = proto->code;
        for (end = code + size; code < end; ++code) {
            binstruction_t ins = (binstruction_t)load_long(fp);
            binstruction_t op = IGET_OP(ins);
            /* fix global variable index */
            if (op == OP_GETGBL || op == OP_SETGBL) {
                int idx = IGET_Bx(ins);
                if (idx >= bcnt) { /* does not fix builtin index */
                    bvalue_t *name = be_list_at(list, idx - bcnt);
                    idx = be_global_find(vm, var_tostr(name));
                    ins = (ins & ~IBx_MASK) | ISET_Bx(idx);
                }
            }
            *code = ins;
        }
    }
}

static void load_constant(bvm_t *vm, void *fp, bproto_t *proto, int version)
{
    int size = (int)load_long(fp); /* nconst */
    if (size) {
        bvalue_t *end, *v = be_malloc(vm, sizeof(bvalue_t) * size);
        memset(v, 0, sizeof(bvalue_t) * size);
        proto->ktab = v;
        proto->nconst = size;
        for (end = v + size; v < end; ++v) {
            load_value(vm, fp, v, version);
        }
    }
}

static void load_proto_table(bvm_t *vm, void *fp, bproto_t *proto, int info, int version)
{
    int size = (int)load_long(fp); /* proto count */
    if (size) {
        bproto_t **p = be_malloc(vm, sizeof(bproto_t *) * size);
        memset(p, 0, sizeof(bproto_t *) * size);
        proto->ptab = p;
        proto->nproto = size;
        while (size--) {
            load_proto(vm, fp, p++, info, version);
        }
    }
}

static void load_upvals(bvm_t *vm, void *fp, bproto_t *proto)
{
    int size = (int)load_byte(fp);
    if (size) {
        bupvaldesc_t *uv, *end;
        proto->upvals = be_malloc(vm, sizeof(bupvaldesc_t) * size);
        proto->nupvals = (bbyte_t)size;
        uv = proto->upvals;
        for (end = uv + size; uv < end; ++uv) {
            uv->instack = load_byte(fp);
            uv->idx = load_byte(fp);
        }
    }
}

static bbool load_proto(bvm_t *vm, void *fp, bproto_t **proto, int info, int version)
{
    /* first load the name */
    /* if empty, it's a static member so don't allocate an actual proto */
    bstring_t *name = load_string(vm, fp);
    if (str_len(name)) {
        *proto = be_newproto(vm);
        (*proto)->name = name;
        (*proto)->source = load_string(vm, fp);
        (*proto)->argc = load_byte(fp);
        (*proto)->nstack = load_byte(fp);
        if (version > 1) {
            (*proto)->varg = load_byte(fp);
            load_byte(fp); /* discard reserved byte */
        }
        load_bytecode(vm, fp, *proto, info);
        load_constant(vm, fp, *proto, version);
        load_proto_table(vm, fp, *proto, info, version);
        load_upvals(vm, fp, *proto);
        return btrue;
    }
    return bfalse;  /* no proto read */
}

void load_global_info(bvm_t *vm, void *fp)
{
    int i;
    int bcnt = (int)load_long(fp); /* builtin count */
    int gcnt = (int)load_long(fp); /* global count */
    if (bcnt > be_builtin_count(vm)) {
        bytecode_error(vm, be_pushfstring(vm,
            "inconsistent number of builtin objects."));
    }
    be_newlist(vm);
    for (i = 0; i < gcnt; ++i) {
        bstring_t *name = cache_string(vm, fp);
        be_global_new(vm, name);
        be_data_push(vm, -2); /* push the variable name to list */
        be_stackpop(vm, 1); /* pop the cached string */
    }
    be_global_release_space(vm);
}

bclosure_t* be_bytecode_load(bvm_t *vm, const char *filename)
{
    void *fp = be_fopen(filename, "rb");
    if (fp == NULL) {
        bytecode_error(vm, be_pushfstring(vm,
            "can not open file '%s'.", filename));
    } else {
        int version = load_head(fp);
        if (version == BYTECODE_VERSION) {
            bclosure_t *cl = be_newclosure(vm, 0);
            var_setclosure(vm->top, cl);
            be_stackpush(vm);
            load_global_info(vm, fp);
            load_proto(vm, fp, &cl->proto, -1, version);
            be_stackpop(vm, 2); /* pop the closure and list */
            be_fclose(fp);
            return cl;
        }
        bytecode_error(vm, be_pushfstring(vm,
            "invalid bytecode version '%s'.", filename));
    }
    bytecode_error(vm, be_pushfstring(vm,
        "invalid bytecode file '%s'.", filename));
    return NULL;
}
#endif /* BE_USE_BYTECODE_LOADER */
