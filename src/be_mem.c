#include "be_mem.h"
#include <stdlib.h>

typedef struct {
    size_t size;
} mnode;

static size_t m_mem_count = 0;

void* be_malloc(size_t size)
{
    mnode *obj = malloc(size + sizeof(mnode));

    if (obj != NULL) {
        obj->size = size;
        m_mem_count += size;
        return (char*)obj + sizeof(mnode);
    }
    return NULL;
}

void be_free(void *p)
{
    if (p != NULL) {
        mnode *obj = (mnode*)((char*)p - sizeof(mnode));
        m_mem_count -= obj->size;
        free(obj);
    }
}

void* be_realloc(void *p, size_t size)
{
    if (p != NULL) {
        mnode *obj, *old = (mnode*)((char*)p - sizeof(mnode));
        m_mem_count = m_mem_count + size - old->size;
        obj = (mnode*)realloc(old, size + sizeof(mnode));
        if (obj != NULL) {
            obj->size = size;
            return (char*)obj + sizeof(mnode);
        }
        free(old);
        return NULL;
    }
    return be_malloc(size);
}

size_t be_mcount(void)
{
    return m_mem_count;
}
