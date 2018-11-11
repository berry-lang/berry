#include "be_opcode.h"

const char *be_opcode2str(bopcode op)
{
    static const char *opcode_tab[] = {
        "ADD", "SUB", "MUL", "DIV", "MOD",
        "LT", "LE", "EQ", "NE", "GT", "GE",
        "NEG", "LDNIL", "LDBOOL", "LDINT",
        "LDCONST", "MOVE", "GETGBL", "SETGBL",
        "GETUPV", "SETUPV", "JMP", "JMPT",
        "JMPF", "CALL", "RET", "CLOSURE",
        "GETMBR", "GETMET", "SETMBR", "GETIDX",
        "SETIDX", "SETSUPER", "CLOSE"
    };
    return op < sizeof(opcode_tab) / sizeof(opcode_tab[0]) ? opcode_tab[op] : "ERROP";
}
