#ifndef BE_MEM_H
#define BE_MEM_H

#include "berry.h"

#define be_malloc(vm, size)         be_realloc((vm), NULL, 0, (size))
#define be_free(vm, ptr, size)      be_realloc((vm), (ptr), (size), 0)

void* be_os_malloc(size_t size);
void be_os_free(void *ptr);
void* be_os_realloc(void *ptr, size_t size);
void* be_realloc(bvm *vm, void *ptr, size_t old_size, size_t new_size);
size_t be_memcount(bvm *vm);

#endif
