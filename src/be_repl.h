#ifndef BE_REPL_H
#define BE_REPL_H

#include "berry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef const char* (*breadline)(const char *prompt);

BERRY_API int be_repl(bvm *vm, breadline getl);

#ifdef __cplusplus
}
#endif

#endif
