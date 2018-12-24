#ifndef __BE_REPL_H
#define __BE_REPL_H

#include "berry.h"

typedef const char* (*breadline)(const char *prompt);

void be_repl(bvm *vm, breadline getl);

#endif
