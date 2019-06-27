/* this file contains configuration for the operating system. */

#include "berry.h"
#include <stdio.h>

/* standard input and output */

void be_writebuffer(const char *buffer, size_t length)
{
    be_fwrite(stdout, buffer, length);
}

char* be_readstring(char* buffer, size_t size)
{
    return be_fgets(stdin, buffer, (int)size);
}
