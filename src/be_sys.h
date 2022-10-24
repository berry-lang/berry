/**
 * @file       berry.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_SYS_H
#define BE_SYS_H

#include <stddef.h>

typedef struct {
    void *dir;        /**< dir */
    void *file;       /**< file */
    const char *name; /**< name */
} bdirinfo;           /**< directory information for directory traversal */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @fn void be_fopen*(const char*, const char*)
 * @brief (???)
 *
 * @param filename (???)
 * @param modes (???)
 */
void* be_fopen(const char *filename, const char *modes);

/**
 * @fn int be_fclose(void*)
 * @brief (???)
 *
 * @param hfile (???)
 * @return (???)
 */
int be_fclose(void *hfile);

/**
 * @fn size_t be_fwrite(void*, const void*, size_t)
 * @brief (???)
 *
 * @param hfile (???)
 * @param buffer (???)
 * @param length (???)
 * @return (???)
 */
size_t be_fwrite(void *hfile, const void *buffer, size_t length);

/**
 * @fn size_t be_fread(void*, void*, size_t)
 * @brief (???)
 *
 * @param hfile (???)
 * @param buffer (???)
 * @param length (???)
 * @return (???)
 */
size_t be_fread(void *hfile, void *buffer, size_t length);

/**
 * @fn char be_fgets*(void*, void*, int)
 * @brief (???)
 *
 * @param hfile (???)
 * @param buffer (???)
 * @param size (???)
 * @return (???)
 */
char* be_fgets(void *hfile, void *buffer, int size);

/**
 * @fn int be_fseek(void*, long)
 * @brief (???)
 *
 * @param hfile (???)
 * @param offset (???)
 * @return (???)
 */
int be_fseek(void *hfile, long offset);

/**
 * @fn long int be_ftell(void*)
 * @brief (???)
 *
 * @param hfile (???)
 * @return (???)
 */
long int be_ftell(void *hfile);

/**
 * @fn long int be_fflush(void*)
 * @brief (???)
 *
 * @param hfile (???)
 * @return (???)
 */
long int be_fflush(void *hfile);

/**
 * @fn size_t be_fsize(void*)
 * @brief (???)
 *
 * @param hfile (???)
 * @return (???)
 */
size_t be_fsize(void *hfile);

/**
 * @fn int be_isdir(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
int be_isdir(const char *path);

/**
 * @fn int be_isfile(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
int be_isfile(const char *path);

/**
 * @fn int be_isexist(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
int be_isexist(const char *path);

/**
 * @fn char be_getcwd*(char*, size_t)
 * @brief (???)
 *
 * @param buf (???)
 * @param size (???)
 * @return (???)
 */
char* be_getcwd(char *buf, size_t size);

/**
 * @fn int be_chdir(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
int be_chdir(const char *path);

/**
 * @fn int be_mkdir(const char*)
 * @brief (???)
 *
 * @param path (???)
 * @return (???)
 */
int be_mkdir(const char *path);

/**
 * @fn int be_unlink(const char*)
 * @brief (???)
 *
 * @param filename (???)
 * @return (???)
 */
int be_unlink(const char *filename);

/**
 * @fn int be_dirfirst(bdirinfo*, const char*)
 * @brief (???)
 *
 * @param info (???)
 * @param path (???)
 * @return (???)
 */
int be_dirfirst(bdirinfo *info, const char *path);

/**
 * @fn int be_dirnext(bdirinfo*)
 * @brief (???)
 *
 * @param info (???)
 * @return (???)
 */
int be_dirnext(bdirinfo *info);

/**
 * @fn int be_dirclose(bdirinfo*)
 * @brief (???)
 *
 * @param info (???)
 * @return (???)
 */
int be_dirclose(bdirinfo *info);

#ifdef __cplusplus
}
#endif

#endif
