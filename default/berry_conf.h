#ifndef __BERRY_CONF_H
#define __BERRY_CONF_H

#include <stdio.h>
#include <assert.h>

/* Macro: BE_DEBUG
 * Berry interpreter debug switch.
 * default: 0
 **/
#ifndef BE_DEBUG
#define BE_DEBUG                        0
#endif

/* Macro: BE_SINGLE_FLOAT
 * Select floating point precision.
 * Use double-precision floating-point numbers when the value
 * is 0 (default), otherwise use single-precision floating-point
 * numbers.
 * default: 0
 **/
#define BE_SINGLE_FLOAT                 0

/* Macro: BE_RUNTIME_DEBUG_INFO
 * Set runtime error debugging information.
 * 0: unable to output source file and line number at runtime.
 * 1: output source file and line number information at runtime.
 * 2: the information use uint16_t type (save space)
 * default: 1
 **/
#define BE_RUNTIME_DEBUG_INFO           1

/* Macro: BE_STACK_TOTAL_MAX
 * Set the maximum total stack size.
 * default: 200
 **/
#define BE_STACK_TOTAL_MAX              200

/* Macro: BE_STACK_FREE_MIN
 * Set the minimum free count of the stack. The stack idles will
 * be checked when a function is called, and the stack will be
 * expanded if the number of free is less than BE_STACK_FREE_MIN.
 * default: 10
 **/
#define BE_STACK_FREE_MIN               10

/* Macro: BE_USE_XXX_MODULE
 * These macros control whether the related module is compiled.
 * When they are true, they will enable related modules. At this
 * point you can use the import statement to import the module.
 * They will not compile related modules when they are false.
 **/
#define BE_USE_IO_MODULE                1
#define BE_USE_STRING_MODULE            1
#define BE_USE_JSON_MODULE              1
#define BE_USE_MATH_MODULE              1
#define BE_USE_TIME_MODULE              1
#define BE_USE_OS_MODULE                1

#define BE_MEM_HEAP_SIZE                4096
#define BE_MEM_SMALLOBJ_SIZE            512

/* Macro: be_assert
 * Berry debug assertion. Only enabled when BE_DEBUG is active.
 * default: use the assert() function of the standard library.
 **/
#define be_assert(expr)                 assert(expr)

/* File operation interface.
 * Modify these macros to port file system support.
 **/
#define be_fhandle                      FILE *
#define be_fopen(fname, mode)           fopen(fname, mode)
#define be_fclose(fp)                   fclose(fp)
#define be_fwrite(fp, buffer, len)      fwrite(buffer, 1, len, fp)
#define be_fread(fp, buffer, len)       fread(buffer, 1, len, fp)
#define be_fgets(fp, buffer, max)       fgets(buffer, max, fp)
#define be_fseek(fp, pos)               fseek(fp, pos, SEEK_SET)
#define be_ftell(fp)                    ftell(fp)
#define be_fflush(fp)                   fflush(fp)
/* #define be_fsize */

#endif
