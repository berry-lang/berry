#ifndef BE_MEM_H
#define BE_MEM_H

#include "berry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define be_malloc(vm, size)         be_realloc((vm), NULL, 0, (size))
#define be_free(vm, ptr, size)      be_realloc((vm), (ptr), (size), 0)

BERRY_API void* be_os_malloc(size_t size);
BERRY_API void be_os_free(void *ptr);
BERRY_API void* be_os_realloc(void *ptr, size_t size);
BERRY_API void* be_realloc(bvm *vm, void *ptr, size_t old_size, size_t new_size);

#ifdef __cplusplus
}
#endif

#endif
