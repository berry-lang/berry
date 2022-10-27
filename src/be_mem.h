/**
 * @file       be_mem.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_MEM_H
#define BE_MEM_H

#include "berry.h"

#ifdef __cplusplus
extern "C" {
#endif

#define be_malloc(vm, size)         be_realloc((vm), NULL, 0, (size))  /**< be_malloc */
#define be_free(vm, ptr, size)      be_realloc((vm), (ptr), (size), 0) /**< be_free */

BERRY_API void* be_os_malloc(size_t size);
BERRY_API void be_os_free(void *ptr);
BERRY_API void* be_os_realloc(void *ptr, size_t size);
BERRY_API void* be_realloc(bvm_t *vm, void *ptr, size_t old_size, size_t new_size);
BERRY_API void be_gc_memory_pools(bvm_t *vm);
BERRY_API void be_gc_free_memory_pools(bvm_t *vm);
BERRY_API void be_gc_init_memory_pools(bvm_t *vm);
BERRY_API void be_gc_memory_pools_info(bvm_t *vm, size_t* slots_used, size_t* slots_allocated);

/* The following moves a portion of memory to constraint regions with 32-bits read/write acess */
/* Effective only if `BE_USE_MEM_ALIGNED` is set to `1`*/
BERRY_API void* be_move_to_aligned(bvm_t *vm, void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
