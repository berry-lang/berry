#include "be_object.h"

const char* be_vtype2str(bvalue *v)
{
    switch(var_type(v)) {
    case BE_NIL: return "nil";
    case BE_INT: return "int";
    case BE_REAL: return "real";
    case BE_BOOL: return "bool";
    case BE_CLOSURE: case BE_NTVCLOS:
    case BE_NTVFUNC: return "function";
    case BE_PROTO: return "proto";
    case BE_CLASS: return "class";
    case BE_STRING: return "string";
    case BE_LIST: return "list";
    case BE_MAP: return "map";
    case BE_INSTANCE: return "instance";
    default: return "invalid type";
    }
}
