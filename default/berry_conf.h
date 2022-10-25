/**
 * @file       berry_conf.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BERRY_CONF_H
#define BERRY_CONF_H

#include <assert.h>

/**
 * @def BE_DEBUG
 * @brief Berry interpreter debug switch.
 * @note Default: 0
 */
#ifndef BE_DEBUG
#define BE_DEBUG                        0
#endif

/**
 * @def BE_INTGER_TYPE
 * @brief Select integer length.
 * @note Default: 2
 *
 * If the value is 0, use an integer of type int, use a long
 * integer type when the value is 1, and use a long long integer
 * type when the value is 2.
 *
 */
#define BE_INTGER_TYPE                  2

/**
 * @def BE_USE_SINGLE_FLOAT
 * @brief Select floating point precision.
 * @note Default: 0
 *
 * Select floating point precision.
 * Use double-precision floating-point numbers when the value
 * is 0 (default), otherwise use single-precision floating-point
 *
 */
#define BE_USE_SINGLE_FLOAT             0

/**
 * @def BE_BYTES_MAX_SIZE
 * @brief Maximum size in bytes of a `bytes()` object.
 * @note Default: 32kb
 *
 * Putting too much pressure on the memory allocator can do
 * harm, so we limit the maximum size.
 *
 */
#define BE_BYTES_MAX_SIZE               (32*1024)   /* 32 kb default value */

/**
 * @def BE_USE_PRECOMPILED_OBJECT
 * @brief Precompiled objects at runtime.
 * @note Default: 1
 *
 * Use precompiled objects to avoid creating these objects at
 * runtime. Enable this macro can greatly optimize RAM usage.
 *
 */
#define BE_USE_PRECOMPILED_OBJECT       1

/**
 * @def BE_DEBUG_RUNTIME_INFO
 * @brief Set runtime error debugging information.
 * @note Default: 1
 *
 * 0: unable to output source file and line number at runtime.
 * 1: output source file and line number information at runtime.
 * 2: the information use uint16_t type (save space).
 *
 */
#define BE_DEBUG_RUNTIME_INFO           1

/**
 * @def BE_DEBUG_VAR_INFO
 * @brief Set variable debugging tracking information.
 * @note Default: 1
 *
 * 0: disable variable debugging tracking information at runtime.
 * 1: enable variable debugging tracking information at runtime.
 *
 */
#define BE_DEBUG_VAR_INFO               1

/**
 * @def BE_USE_PERF_COUNTERS
 * @brief Use the obshook function to report low-level actions.
 * @note Default: 1
 *
 *
 */
#define BE_USE_PERF_COUNTERS            1

/**
 * @def BE_VM_OBSERVABILITY_SAMPLING
 * @brief observability hook
 * @note Default: 20 - which translates to 2^20 or ~1 million instructions
 *
 * If BE_USE_PERF_COUNTERS == 1
 * then the observability hook is called regularly in the VM loop
 * allowing to stop infinite loops or too-long running code.
 * The value is a power of 2.
 *
 */
#define BE_VM_OBSERVABILITY_SAMPLING    20

/**
 * @def BE_STACK_TOTAL_MAX
 * @brief Set the maximum total stack size.
 * @note Default: 20000
 *
 */
#define BE_STACK_TOTAL_MAX              20000

/**
 * @def BE_STACK_FREE_MIN
 * @brief Minimum free count of the stac
 * @note Default: 10
 *
 * Set the minimum free count of the stack. The stack idles will
 * be checked when a function is called, and the stack will be
 * expanded if the number of free is less than BE_STACK_FREE_MIN
 *
 */
#define BE_STACK_FREE_MIN               10

/**
 * @def BE_STACK_START
 * @brief Set the starting size of the stack at VM creation.
 * @note Default: 50
 *
 */
#define BE_STACK_START                  50

/**
 * @def BE_CONST_SEARCH_SIZE
 * @brief Constants in function
 * @note Default: 50
 *
 * Constants in function are limited to 255. However the compiler
 * will look for a maximum of pre-existing constants to avoid
 * performance degradation. This may cause the number of constants
 * to be higher than required.
 * Increase is you need to solidify functions.
 *
 */
#define BE_CONST_SEARCH_SIZE            50

/**
 * @def BE_USE_STR_HASH_CACHE
 * @brief Short string
 * @note Default: 0
 *
 * The short string will hold the hash value when the value is
 * true. It may be faster but requires more RAM.
 *
 */
#define BE_USE_STR_HASH_CACHE           0

/**
 * @def BE_USE_FILE_SYSTEM
 * @brief File system interface.
 * @note Default: 0
 *
 * The file system interface will be used when this macro is true
 * or when using the OS module. Otherwise the file system interface
 * will not be used.
 *
 */
#define BE_USE_FILE_SYSTEM              1

/**
 * @def BE_USE_SCRIPT_COMPILER
 * @brief Script compiler
 * @note Default: 1
 *
 * Enable compiler when BE_USE_SCRIPT_COMPILER is not 0, otherwise
 * disable the compiler.
 *
 */
#define BE_USE_SCRIPT_COMPILER          1

/**
 * @def BE_USE_BYTECODE_SAVER
 * @brief Save bytecode
 * @note Default: 1
 *
 * Enable save bytecode to file when BE_USE_BYTECODE_SAVER is not 0,
 * otherwise disable the feature.
 *
 */
#define BE_USE_BYTECODE_SAVER           1

/**
 * @def BE_USE_BYTECODE_LOADER
 * @brief Loader bytecode.
 * @note Default: 1
 *
 * Enable load bytecode from file when BE_USE_BYTECODE_LOADER is not 0,
 * otherwise disable the feature.
 *
 */
#define BE_USE_BYTECODE_LOADER          1

/**
 * @def BE_USE_SHARED_LIB
 * @brief Shared library.
 * @note Default: 1
 *
 * Enable shared library  when BE_USE_SHARED_LIB is not 0,
 * otherwise disable the feature.
 *
 */
#define BE_USE_SHARED_LIB               1

/**
 * @def BE_USE_OVERLOAD_HASH
 * @brief Overload hash methods.
 * @note Default: 1
 *
 * Allows instances to overload hash methods for use in the
 * built-in Map class. Disable this feature to crop the code
 * size.
 *
 */
#define BE_USE_OVERLOAD_HASH            1

/**
 * @def BE_USE_DEBUG_HOOK
 * @brief Berry debug hook switch.
 * @note Default: 0
 */
#define BE_USE_DEBUG_HOOK               1

/**
 * @def BE_USE_DEBUG_GC
 * @brief GC debug mode.
 * @note Default: 0
 *
 * Enable GC debug mode. This causes an actual gc after each
 * allocation. It's much slower and should not be used
 * in production code.
 *
 */
#define BE_USE_DEBUG_GC                  0

/**
 * @def BE_USE_DEBUG_STACK
 * @brief Stack Resize debug mode.
 * @note Default: 0
 *
 * Enable Stack Resize debug mode. At each function call
 * the stack is reallocated at a different memory location
 * and the previous location is cleared with toxic data.
 *
 */
#define BE_USE_DEBUG_STACK               0

/**
 * @note BE_USE_XXX_MODULE
 * These macros control whether the related module is compiled.
 * When they are true, they will enable related modules. At this
 * point you can use the import statement to import the module.
 * They will not compile related modules when they are false.
 * @{
 */
#define BE_USE_STRING_MODULE            1 /**< BE_USE_STRING_MODULE */
#define BE_USE_JSON_MODULE              1 /**< BE_USE_JSON_MODULE */
#define BE_USE_MATH_MODULE              1 /**< BE_USE_MATH_MODULE */
#define BE_USE_TIME_MODULE              1 /**< BE_USE_TIME_MODULE */
#define BE_USE_OS_MODULE                1 /**< BE_USE_OS_MODULE */
#define BE_USE_GLOBAL_MODULE            1 /**< BE_USE_GLOBAL_MODULE */
#define BE_USE_SYS_MODULE               1 /**< BE_USE_SYS_MODULE */
#define BE_USE_DEBUG_MODULE             1 /**< BE_USE_DEBUG_MODULE */
#define BE_USE_GC_MODULE                1 /**< BE_USE_GC_MODULE */
#define BE_USE_SOLIDIFY_MODULE          1 /**< BE_USE_SOLIDIFY_MODULE */
#define BE_USE_INTROSPECT_MODULE        1 /**< BE_USE_INTROSPECT_MODULE */
#define BE_USE_STRICT_MODULE            1 /**< BE_USE_STRICT_MODULE */
/** @} */

/**
 * @note BE_EXPLICIT_XXX
 * If these macros are defined, the corresponding function will
 * use the version defined by these macros. These macro definitions
 * are not required.
 * The default is to use the functions in the standard library.
 * @{
 */
#define BE_EXPLICIT_ABORT               abort   /**< BE_EXPLICIT_ABORT */
#define BE_EXPLICIT_EXIT                exit    /**< BE_EXPLICIT_EXIT */
#define BE_EXPLICIT_MALLOC              malloc  /**< BE_EXPLICIT_MALLOC */
#define BE_EXPLICIT_FREE                free    /**< BE_EXPLICIT_FREE */
#define BE_EXPLICIT_REALLOC             realloc /**< BE_EXPLICIT_REALLOC */
/** @} */

/**
 * @def be_assert
 * @brief Berry debug assertion
 * @note Default: use the assert() function of the standard library.
 *
 * Only enabled when BE_DEBUG is active.
 *
 */
#define be_assert(expr)                 assert(expr)

#endif
