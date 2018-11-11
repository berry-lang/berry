#ifndef __BE_MEM_H
#define __BE_MEM_H

#include <stddef.h>

void* be_malloc(size_t size);
void be_free(void *p);
void* be_realloc(void *p, size_t size);
size_t be_mem_usage(void);

#endif
