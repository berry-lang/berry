/**
 * @file       be_exec.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_EXEC_H
#define BE_EXEC_H

#include "be_object.h"
#include <setjmp.h>

/**
 * @fn void (*bpfunc)(bvm *vm, void *data)
 * @brief Protected-call function
 *
 * @param vm
 * @param data
 */
typedef void (*bpfunc)(bvm_t *vm, void *data);

#if BE_DEBUG
bvalue_t* be_incrtop(bvm_t *vm);                          /**< be_incrtop */
#else
/* increase top register */
#define be_incrtop(vm)          ((vm)->top++)             /**< be_incrtop */
#endif

#define be_stackpop(vm, n)      ((vm)->top -= (n))        /**< be_stackpop */

/* in MinGW-w64, setjmp / longjmp may be broken,
 * so here is replaced by __builtin version. */
#if defined(__GNUC__) && defined(__MINGW32__)
  #define be_setjmp(env)        __builtin_setjmp(env)     /**< be_setjmp */
  #define be_longjmp(env, v)    __builtin_longjmp(env, v) /**< be_longjmp */
#else
  #define be_setjmp(env)        setjmp(env)               /**< be_setjmp */
  #define be_longjmp(env, v)    longjmp(env, v)           /**< be_longjmp */
#endif

typedef jmp_buf bjmpbuf_t; /**< bjmpbuf_t */

/**
 * @struct blongjmp
 * @brief blongjmp
 *
 */
struct blongjmp {
    bjmpbuf_t b;           /**< b */
    struct blongjmp *prev; /**< prev */
    volatile int status;   /**< error code */
};

/**
 * @struct bexecptframe
 * @brief bexecptframe
 *
 */
struct bexecptframe {
    struct blongjmp errjmp; /**< long jump information */
    int depth;              /**< function call stack depth */
    binstruction_t *ip;     /**< instruction pointer */
    int refcount;           /**< save object reference stack */
};

/**
 * @fn void be_throw(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param errorcode (???)
 */
void be_throw(bvm_t *vm, int errorcode);

/**
 * @fn int be_execprotected(bvm_t*, bpfunc, void*)
 * @brief (???)
 *
 * @param vm (???)
 * @param f (???)
 * @param data (???)
 * @return (???)
 */
int be_execprotected(bvm_t *vm, bpfunc f, void *data);

/**
 * @fn int be_protectedparser(bvm_t*, const char*, breader, void*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param fname (???)
 * @param reader (???)
 * @param data (???)
 * @param islocal (???)
 * @return (???)
 */
int be_protectedparser(bvm_t *vm, const char *fname, breader reader, void *data, bbool islocal);

/**
 * @fn int be_protectedcall(bvm_t*, bvalue_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param v (???)
 * @param argc (???)
 * @return (???)
 */
int be_protectedcall(bvm_t *vm, bvalue_t *v, int argc);

/**
 * @fn void be_stackpush(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_stackpush(bvm_t *vm);

/**
 * @fn void be_stack_expansion(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param n (???)
 */
void be_stack_expansion(bvm_t *vm, int n);

/**
 * @fn void be_except_block_setup(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_except_block_setup(bvm_t *vm);

/**
 * @fn void be_except_block_resume(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_except_block_resume(bvm_t *vm);

/**
 * @fn void be_except_block_close(bvm_t*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param count (???)
 */
void be_except_block_close(bvm_t *vm, int count);

/**
 * @fn void be_save_stacktrace(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_save_stacktrace(bvm_t *vm);

#endif
