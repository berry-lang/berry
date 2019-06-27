#ifndef BE_SYS_H
#define BE_SYS_H

#include <stddef.h>

/* directory information for directory traversal */
typedef struct {
    void *dir;
    void *file;
    const char *name;
} bdirinfo;

void* be_fopen(const char *filename, const char *modes);
int be_fclose(void *hfile);
size_t be_fwrite(void *hfile, const void *buffer, size_t length);
size_t be_fread(void *hfile, void *buffer, size_t length);
char* be_fgets(void *hfile, void *buffer, int size);
int be_fseek(void *hfile, long offset);
long int be_ftell(void *hfile);
long int be_fflush(void *hfile);
size_t be_fsize(void *hfile);
int be_isdir(const char *path);
int be_isfile(const char *path);
int be_isexist(const char *path);
char* be_getcwd(char *buf, size_t size);
int be_chdir(const char *path);
int be_mkdir(const char *path);
int be_unlink(const char *filename);
int be_dirfirst(bdirinfo *info, const char *path);
int be_dirnext(bdirinfo *info);
int be_dirclose(bdirinfo *info);

#endif
