/* this file contains configuration for the file system. */

#include "berry.h"

FILE* be_fopen(const char *filename, const char *modes)
{
    return fopen(filename, modes);
}

int be_fclose(FILE *hfile)
{
    return fclose(hfile);
}

size_t be_fwrite(FILE *hfile, const void *buffer, size_t length)
{
    return fwrite(buffer, 1, length, hfile);
}

size_t be_fread(FILE *hfile, void *buffer, size_t length)
{
    return fread(buffer, 1, length, hfile);
}

char *be_fgets(FILE *hfile, void *buffer, size_t size)
{
    return fgets(buffer, size, hfile);
}

int be_fseek(FILE *hfile, size_t offset)
{
    return fseek(hfile, offset, SEEK_SET);
}

long int be_ftell(FILE *hfile)
{
    return ftell(hfile);
}

long int be_fflush(FILE *hfile)
{
    return fflush(hfile);
}

size_t be_fsize(FILE *hfile)
{
    long int size, offset = be_ftell(hfile);
    fseek(hfile, 0L, SEEK_END);
    size = ftell(hfile);
    fseek(hfile, offset, SEEK_SET);
    return size;
}
