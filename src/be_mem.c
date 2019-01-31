#include "be_mem.h"
#include "berry.h"
#include <stdlib.h>

#define os_malloc(size)         malloc(size)
#define os_free(ptr)            free(ptr)
#define os_realloc(ptr, size)   realloc(ptr, size)

#define addr_pos(a, p, o)       ((void*)((size_t)(a) o (p)))
#define addr_region(a, s)       addr_pos((a), sizeof(s), +)
#define addr_base(a, s)         addr_pos((a), sizeof(s), -)

typedef struct {
    size_t use;
} memlist;

typedef struct {
    size_t size;
} mdesc;

static memlist mlist = { .use = 0 };

void* be_malloc(size_t size)
{
    mdesc *obj = os_malloc(size + sizeof(mdesc));

    if (obj != NULL) {
        obj->size = size;
        mlist.use += size;
        return addr_region(obj, mdesc);
    }
    return NULL;
}

void be_free(void *ptr)
{
    if (ptr != NULL) {
        mdesc *obj = addr_base(ptr, mdesc);
        mlist.use -= obj->size;
        os_free(obj);
    }
}

void* be_realloc(void *ptr, size_t size)
{
    if (ptr != NULL) {
        mdesc *obj, *old = addr_base(ptr, mdesc);
        mlist.use = mlist.use + size - old->size;
        obj = (mdesc*)os_realloc(old, size + sizeof(mdesc));
        if (obj != NULL) {
            obj->size = size;
            return addr_region(obj, mdesc);
        }
        os_free(old);
        return NULL;
    }
    return be_malloc(size);
}

size_t be_mcount(void)
{
    return mlist.use;
}
