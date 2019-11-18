#include "be_opcode.h"
#include "be_object.h"

const char *be_opcode2str(bopcode op)
{
    static const char* const opcode_tab[] = {
        "ADD", "SUB", "MUL", "DIV", "MOD",
        "LT", "LE", "EQ", "NE", "GT", "GE",
        "AND", "OR", "XOR", "SHL", "SHR",
        "CONNECT", "NEG", "FLIP", "LDNIL",
        "LDBOOL", "LDINT", "LDCONST", "MOVE",
        "GETGBL", "SETGBL", "GETUPV", "SETUPV",
        "JMP", "JMPT", "JMPF", "CALL", "RET",
        "CLOSURE", "GETMBR", "GETMET", "SETMBR",
        "GETIDX", "SETIDX", "SETSUPER", "CLOSE",
        "IMPORT", "EXBLK", "CATCH", "RAISE"
    };
    return op < array_count(opcode_tab) ? opcode_tab[op] : "ERROP";
}
