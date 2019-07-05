#include "be_mem.h"
#include "be_exec.h"
#include "be_vm.h"
#include "be_gc.h"
#include <stdlib.h>

#ifdef BE_EXPLICIT_MALLOC
  #define malloc                BE_EXPLICIT_MALLOC
#endif

#ifdef BE_EXPLICIT_FREE
  #define free                  BE_EXPLICIT_FREE
#endif

#ifdef BE_EXPLICIT_REALLOC
  #define realloc               BE_EXPLICIT_REALLOC
#endif

void* be_os_malloc(size_t size)
{
    return malloc(size);
}

void be_os_free(void *ptr)
{
    free(ptr);
}

void* be_os_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

static void* _realloc(void *ptr, size_t old_size, size_t new_size)
{
    if (old_size == new_size) { /* the block unchanged */
        return ptr;
    }
    if (ptr && new_size) { /* realloc block */
        return realloc(ptr, new_size);
    }
    if (new_size) { /* alloc a new block */
        be_assert(ptr == NULL && old_size == 0);
        return malloc(new_size);
    }
    be_assert(new_size == 0);
    free(ptr);
    return NULL;
}

void* be_realloc(bvm *vm, void *ptr, size_t old_size, size_t new_size)
{
    void *block = _realloc(ptr, old_size, new_size);
    if (!block && new_size) { /* allocation failure */
        be_gc_collect(vm); /* try to allocate again after GC */
        block = _realloc(ptr, old_size, new_size);
        if (!block) { /* lack of heap space */
            be_throw(vm, BE_MALLOC_FAIL);
        }
    }
    vm->gc.usage = vm->gc.usage + new_size - old_size; /* update allocated count */
    return block;
}

size_t be_memcount(bvm *vm)
{
    return vm->gc.usage;
}
