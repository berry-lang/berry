#include "be_cstrtab.h"
#include "be_gc.h"

const bcstring be_conststr_number = {
    .next = (bgcobject *)&be_conststr_print,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "number"
};

const bcstring be_conststr_print = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "print"
};

const bcstring be_conststr_seek = {
    .next = (bgcobject *)&be_conststr_resize,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "seek"
};

const bcstring be_conststr_resize = {
    .next = (bgcobject *)&be_conststr_append,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "resize"
};

const bcstring be_conststr_append = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "append"
};

const bcstring be_conststr_range = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "range"
};

const bcstring be_conststr_close = {
    .next = (bgcobject *)&be_conststr_init,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "close"
};

const bcstring be_conststr_init = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "init"
};

const bcstring be_conststr_classname = {
    .next = (bgcobject *)&be_conststr_super,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "classname"
};

const bcstring be_conststr_super = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "super"
};

const bcstring be_conststr_memcount = {
    .next = (bgcobject *)&be_conststr_clock,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "memcount"
};

const bcstring be_conststr_clock = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "clock"
};

const bcstring be_conststr_readline = {
    .next = (bgcobject *)&be_conststr_iter,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "readline"
};

const bcstring be_conststr_iter = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "iter"
};

const bcstring be_conststr_setrange = {
    .next = (bgcobject *)&be_conststr_insert,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "setrange"
};

const bcstring be_conststr_insert = {
    .next = (bgcobject *)&be_conststr___next__,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "insert"
};

const bcstring be_conststr___next__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "__next__"
};

const bcstring be_conststr_open = {
    .next = (bgcobject *)&be_conststr_map,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "open"
};

const bcstring be_conststr_map = {
    .next = (bgcobject *)&be_conststr_list,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 3,
    .s = "map"
};

const bcstring be_conststr_list = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "list"
};

const bcstring be_conststr_lower = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "lower"
};

const bcstring be_conststr___upper__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "__upper__"
};

const bcstring be_conststr_size = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "size"
};

const bcstring be_conststr_flush = {
    .next = (bgcobject *)&be_conststr_tostring,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "flush"
};

const bcstring be_conststr_tostring = {
    .next = (bgcobject *)&be_conststr_exit,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "tostring"
};

const bcstring be_conststr_exit = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "exit"
};

const bcstring be_conststr___lower__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "__lower__"
};

const bcstring be_conststr_upper = {
    .next = (bgcobject *)&be_conststr___iterator__,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "upper"
};

const bcstring be_conststr___iterator__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 12,
    .s = "__iterator__"
};

const bcstring be_conststr_deinit = {
    .next = (bgcobject *)&be_conststr_string,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "deinit"
};

const bcstring be_conststr_string = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "string"
};

const bcstring be_conststr_2Edata = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = ".data"
};

const bcstring be_conststr_tell = {
    .next = (bgcobject *)&be_conststr___hasnext__,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "tell"
};

const bcstring be_conststr___hasnext__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 11,
    .s = "__hasnext__"
};

const bcstring be_conststr_write = {
    .next = (bgcobject *)&be_conststr_setitem,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "write"
};

const bcstring be_conststr_setitem = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 7,
    .s = "setitem"
};

const bcstring be_conststr_read = {
    .next = (bgcobject *)&be_conststr_remove,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "read"
};

const bcstring be_conststr_remove = {
    .next = (bgcobject *)&be_conststr_length,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "remove"
};

const bcstring be_conststr_length = {
    .next = (bgcobject *)&be_conststr_type,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "length"
};

const bcstring be_conststr_type = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "type"
};

const bcstring be_conststr_item = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "item"
};

static const bstring* m_string_table[] = {
    (const bstring *)&be_conststr_number,
    (const bstring *)&be_conststr_seek,
    (const bstring *)&be_conststr_range,
    (const bstring *)&be_conststr_close,
    (const bstring *)&be_conststr_classname,
    (const bstring *)&be_conststr_memcount,
    NULL,
    (const bstring *)&be_conststr_readline,
    (const bstring *)&be_conststr_setrange,
    (const bstring *)&be_conststr_open,
    (const bstring *)&be_conststr_lower,
    (const bstring *)&be_conststr___upper__,
    (const bstring *)&be_conststr_size,
    (const bstring *)&be_conststr_flush,
    (const bstring *)&be_conststr___lower__,
    (const bstring *)&be_conststr_upper,
    (const bstring *)&be_conststr_deinit,
    (const bstring *)&be_conststr_2Edata,
    (const bstring *)&be_conststr_tell,
    NULL,
    (const bstring *)&be_conststr_write,
    (const bstring *)&be_conststr_read,
    (const bstring *)&be_conststr_item,
    NULL
};

const struct bconststrtab be_const_string_table = {
    .size = 24,
    .count = 41,
    .table = m_string_table
};
