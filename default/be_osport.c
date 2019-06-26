/* this file contains configuration for the operating system. */

#include "berry.h"
#include <stdlib.h>

/* standard input and output */

void be_writebuffer(const char *buffer, size_t length)
{
    be_fwrite(stdout, buffer, length);
}

char* be_readstring(char* buffer, size_t size)
{
    return be_fgets(stdin, buffer, (int)size);
}

/* dynamic memory management */

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
