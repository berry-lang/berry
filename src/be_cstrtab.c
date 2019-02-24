#include "be_cstrtab.h"
#include "be_gc.h"

const bcstring be_const_str_number = {
    .next = (bgcobject *)&be_const_str_print,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "number"
};

const bcstring be_const_str_print = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "print"
};

const bcstring be_const_str_seek = {
    .next = (bgcobject *)&be_const_str_resize,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "seek"
};

const bcstring be_const_str_resize = {
    .next = (bgcobject *)&be_const_str_append,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "resize"
};

const bcstring be_const_str_append = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "append"
};

const bcstring be_const_str_range = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "range"
};

const bcstring be_const_str_close = {
    .next = (bgcobject *)&be_const_str_init,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "close"
};

const bcstring be_const_str_init = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "init"
};

const bcstring be_const_str_classname = {
    .next = (bgcobject *)&be_const_str_super,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "classname"
};

const bcstring be_const_str_super = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "super"
};

const bcstring be_const_str_memcount = {
    .next = (bgcobject *)&be_const_str_clock,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "memcount"
};

const bcstring be_const_str_clock = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "clock"
};

const bcstring be_const_str_readline = {
    .next = (bgcobject *)&be_const_str_iter,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "readline"
};

const bcstring be_const_str_iter = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "iter"
};

const bcstring be_const_str_setrange = {
    .next = (bgcobject *)&be_const_str_insert,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "setrange"
};

const bcstring be_const_str_insert = {
    .next = (bgcobject *)&be_const_str___next__,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "insert"
};

const bcstring be_const_str___next__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "__next__"
};

const bcstring be_const_str_open = {
    .next = (bgcobject *)&be_const_str_map,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "open"
};

const bcstring be_const_str_map = {
    .next = (bgcobject *)&be_const_str_list,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 3,
    .s = "map"
};

const bcstring be_const_str_list = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "list"
};

const bcstring be_const_str_lower = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "lower"
};

const bcstring be_const_str___upper__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "__upper__"
};

const bcstring be_const_str_size = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "size"
};

const bcstring be_const_str_flush = {
    .next = (bgcobject *)&be_const_str_tostring,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "flush"
};

const bcstring be_const_str_tostring = {
    .next = (bgcobject *)&be_const_str_exit,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 8,
    .s = "tostring"
};

const bcstring be_const_str_exit = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "exit"
};

const bcstring be_const_str___lower__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 9,
    .s = "__lower__"
};

const bcstring be_const_str_upper = {
    .next = (bgcobject *)&be_const_str___iterator__,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "upper"
};

const bcstring be_const_str___iterator__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 12,
    .s = "__iterator__"
};

const bcstring be_const_str_deinit = {
    .next = (bgcobject *)&be_const_str_string,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "deinit"
};

const bcstring be_const_str_string = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "string"
};

const bcstring be_const_str_dot_data = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = ".data"
};

const bcstring be_const_str_tell = {
    .next = (bgcobject *)&be_const_str___hasnext__,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "tell"
};

const bcstring be_const_str___hasnext__ = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 11,
    .s = "__hasnext__"
};

const bcstring be_const_str_write = {
    .next = (bgcobject *)&be_const_str_setitem,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 5,
    .s = "write"
};

const bcstring be_const_str_setitem = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 7,
    .s = "setitem"
};

const bcstring be_const_str_read = {
    .next = (bgcobject *)&be_const_str_remove,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "read"
};

const bcstring be_const_str_remove = {
    .next = (bgcobject *)&be_const_str_length,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "remove"
};

const bcstring be_const_str_length = {
    .next = (bgcobject *)&be_const_str_type,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 6,
    .s = "length"
};

const bcstring be_const_str_type = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "type"
};

const bcstring be_const_str_item = {
    .next = NULL,
    .type = BE_STRING,
    .marked = GC_CONST,
    .extra = 0,
    .slen = 4,
    .s = "item"
};

static const bstring* m_string_table[] = {
    (const bstring *)&be_const_str_number,
    (const bstring *)&be_const_str_seek,
    (const bstring *)&be_const_str_range,
    (const bstring *)&be_const_str_close,
    (const bstring *)&be_const_str_classname,
    (const bstring *)&be_const_str_memcount,
    NULL,
    (const bstring *)&be_const_str_readline,
    (const bstring *)&be_const_str_setrange,
    (const bstring *)&be_const_str_open,
    (const bstring *)&be_const_str_lower,
    (const bstring *)&be_const_str___upper__,
    (const bstring *)&be_const_str_size,
    (const bstring *)&be_const_str_flush,
    (const bstring *)&be_const_str___lower__,
    (const bstring *)&be_const_str_upper,
    (const bstring *)&be_const_str_deinit,
    (const bstring *)&be_const_str_dot_data,
    (const bstring *)&be_const_str_tell,
    NULL,
    (const bstring *)&be_const_str_write,
    (const bstring *)&be_const_str_read,
    (const bstring *)&be_const_str_item,
    NULL
};

const struct bconststrtab be_const_string_table = {
    .size = 24,
    .count = 41,
    .table = m_string_table
};
