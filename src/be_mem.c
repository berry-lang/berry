#include "be_mem.h"
#include "berry.h"
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

void* be_malloc(size_t size)
{
    return malloc(size);
}

void be_free(void *ptr)
{
    free(ptr);
}

void* be_realloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}
