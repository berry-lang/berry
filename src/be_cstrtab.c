#include "be_conststr.h"
#include "be_string.h"
#include "be_gc.h"

static const bcstring be_conststr_deinit;
static const bcstring be_conststr_setrange;
static const bcstring be_conststr_insert;
static const bcstring be_conststr_5F5Fnext5F5F;
static const bcstring be_conststr_string;
static const bcstring be_conststr_number;
static const bcstring be_conststr_print;
static const bcstring be_conststr_seek;
static const bcstring be_conststr_open;
static const bcstring be_conststr_resize;
static const bcstring be_conststr_append;
static const bcstring be_conststr_2Edata;
static const bcstring be_conststr_map;
static const bcstring be_conststr_list;
static const bcstring be_conststr_tell;
static const bcstring be_conststr_lower;
static const bcstring be_conststr_5F5Fhasnext5F5F;
static const bcstring be_conststr_range;
static const bcstring be_conststr_close;
static const bcstring be_conststr_5F5Fupper5F5F;
static const bcstring be_conststr_init;
static const bcstring be_conststr_write;
static const bcstring be_conststr_size;
static const bcstring be_conststr_setitem;
static const bcstring be_conststr_classname;
static const bcstring be_conststr_super;
static const bcstring be_conststr_flush;
static const bcstring be_conststr_read;
static const bcstring be_conststr_remove;
static const bcstring be_conststr_tostring;
static const bcstring be_conststr_length;
static const bcstring be_conststr_type;
static const bcstring be_conststr_memcount;
static const bcstring be_conststr_exit;
static const bcstring be_conststr_clock;
static const bcstring be_conststr_5F5Flower5F5F;
static const bcstring be_conststr_item;
static const bcstring be_conststr_readline;
static const bcstring be_conststr_upper;
static const bcstring be_conststr_5F5Fiterator5F5F;
static const bcstring be_conststr_iter;

static const bcstring be_conststr_deinit = {
    .next = (bgcobject *)&be_conststr_setrange,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "deinit"
};

static const bcstring be_conststr_setrange = {
    .next = (bgcobject *)&be_conststr_insert,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "setrange"
};

static const bcstring be_conststr_insert = {
    .next = (bgcobject *)&be_conststr_5F5Fnext5F5F,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "insert"
};

static const bcstring be_conststr_5F5Fnext5F5F = {
    .next = (bgcobject *)&be_conststr_string,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "__next__"
};

static const bcstring be_conststr_string = {
    .next = (bgcobject *)&be_conststr_number,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "string"
};

static const bcstring be_conststr_number = {
    .next = (bgcobject *)&be_conststr_print,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "number"
};

static const bcstring be_conststr_print = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "print"
};

static const bcstring be_conststr_seek = {
    .next = (bgcobject *)&be_conststr_open,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "seek"
};

static const bcstring be_conststr_open = {
    .next = (bgcobject *)&be_conststr_resize,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "open"
};

static const bcstring be_conststr_resize = {
    .next = (bgcobject *)&be_conststr_append,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "resize"
};

static const bcstring be_conststr_append = {
    .next = (bgcobject *)&be_conststr_2Edata,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "append"
};

static const bcstring be_conststr_2Edata = {
    .next = (bgcobject *)&be_conststr_map,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = ".data"
};

static const bcstring be_conststr_map = {
    .next = (bgcobject *)&be_conststr_list,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 3,
    .s = "map"
};

static const bcstring be_conststr_list = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "list"
};

static const bcstring be_conststr_tell = {
    .next = (bgcobject *)&be_conststr_lower,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "tell"
};

static const bcstring be_conststr_lower = {
    .next = (bgcobject *)&be_conststr_5F5Fhasnext5F5F,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "lower"
};

static const bcstring be_conststr_5F5Fhasnext5F5F = {
    .next = (bgcobject *)&be_conststr_range,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 11,
    .s = "__hasnext__"
};

static const bcstring be_conststr_range = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "range"
};

static const bcstring be_conststr_close = {
    .next = (bgcobject *)&be_conststr_5F5Fupper5F5F,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "close"
};

static const bcstring be_conststr_5F5Fupper5F5F = {
    .next = (bgcobject *)&be_conststr_init,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "__upper__"
};

static const bcstring be_conststr_init = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "init"
};

static const bcstring be_conststr_write = {
    .next = (bgcobject *)&be_conststr_size,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "write"
};

static const bcstring be_conststr_size = {
    .next = (bgcobject *)&be_conststr_setitem,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "size"
};

static const bcstring be_conststr_setitem = {
    .next = (bgcobject *)&be_conststr_classname,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 7,
    .s = "setitem"
};

static const bcstring be_conststr_classname = {
    .next = (bgcobject *)&be_conststr_super,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "classname"
};

static const bcstring be_conststr_super = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "super"
};

static const bcstring be_conststr_flush = {
    .next = (bgcobject *)&be_conststr_read,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "flush"
};

static const bcstring be_conststr_read = {
    .next = (bgcobject *)&be_conststr_remove,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "read"
};

static const bcstring be_conststr_remove = {
    .next = (bgcobject *)&be_conststr_tostring,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "remove"
};

static const bcstring be_conststr_tostring = {
    .next = (bgcobject *)&be_conststr_length,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "tostring"
};

static const bcstring be_conststr_length = {
    .next = (bgcobject *)&be_conststr_type,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "length"
};

static const bcstring be_conststr_type = {
    .next = (bgcobject *)&be_conststr_memcount,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "type"
};

static const bcstring be_conststr_memcount = {
    .next = (bgcobject *)&be_conststr_exit,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "memcount"
};

static const bcstring be_conststr_exit = {
    .next = (bgcobject *)&be_conststr_clock,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "exit"
};

static const bcstring be_conststr_clock = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "clock"
};

static const bcstring be_conststr_5F5Flower5F5F = {
    .next = (bgcobject *)&be_conststr_item,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "__lower__"
};

static const bcstring be_conststr_item = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "item"
};

static const bcstring be_conststr_readline = {
    .next = (bgcobject *)&be_conststr_upper,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "readline"
};

static const bcstring be_conststr_upper = {
    .next = (bgcobject *)&be_conststr_5F5Fiterator5F5F,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "upper"
};

static const bcstring be_conststr_5F5Fiterator5F5F = {
    .next = (bgcobject *)&be_conststr_iter,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 12,
    .s = "__iterator__"
};

static const bcstring be_conststr_iter = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "iter"
};

static const bstring* m_string_table[] = {
    (const bstring *)&be_conststr_deinit,
    (const bstring *)&be_conststr_seek,
    (const bstring *)&be_conststr_tell,
    (const bstring *)&be_conststr_close,
    (const bstring *)&be_conststr_write,
    (const bstring *)&be_conststr_flush,
    (const bstring *)&be_conststr_5F5Flower5F5F,
    (const bstring *)&be_conststr_readline
};

const bconststrtab be_const_string_table = {
    .size = 8,
    .count = 41,
    .table = m_string_table
};
