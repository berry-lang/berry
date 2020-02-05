#ifndef __BE_BYTECODE_H
#define __BE_BYTECODE_H

#include "be_object.h"

void be_bytecode_save(bvm *vm, const char *filename, bproto *proto);
bclosure* be_bytecode_load(bvm *vm, const char *filename);
bbool be_bytecode_check(const char *path);

#endif
