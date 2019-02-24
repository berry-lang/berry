#ifndef BE_REPL_H
#define BE_REPL_H

#include "berry.h"

typedef const char* (*breadline)(const char *prompt);

int be_repl(bvm *vm, breadline getl);

#endif
