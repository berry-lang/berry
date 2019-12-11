#ifndef BE_REPL_H
#define BE_REPL_H

#include "berry.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef char* (*breadline)(const char *prompt);
typedef void (*bfreeline)(char *ptr);

BERRY_API int be_repl(bvm *vm, breadline getline, bfreeline freeline);

#ifdef __cplusplus
}
#endif

#endif
