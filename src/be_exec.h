/**
 * @file       be_exec.h
 * @brief      Main berry entry API
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
typedef void (*bpfunc)(bvm *vm, void *data);

#if BE_DEBUG
bvalue* be_incrtop(bvm *vm);                              /**< be_incrtop */
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

typedef jmp_buf bjmpbuf; /**< bjmpbuf */

/**
 * @struct blongjmp
 * @brief blongjmp
 *
 */
struct blongjmp {
    bjmpbuf b;             /**< b */
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
    binstruction *ip;       /**< instruction pointer */
    int refcount;           /**< save object reference stack */
};

/**
 * @fn void be_throw(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param errorcode (???)
 */
void be_throw(bvm *vm, int errorcode);

/**
 * @fn int be_execprotected(bvm*, bpfunc, void*)
 * @brief (???)
 *
 * @param vm (???)
 * @param f (???)
 * @param data (???)
 * @return (???)
 */
int be_execprotected(bvm *vm, bpfunc f, void *data);

/**
 * @fn int be_protectedparser(bvm*, const char*, breader, void*, bool)
 * @brief (???)
 *
 * @param vm (???)
 * @param fname (???)
 * @param reader (???)
 * @param data (???)
 * @param islocal (???)
 * @return (???)
 */
int be_protectedparser(bvm *vm, const char *fname, breader reader, void *data, bbool islocal);

/**
 * @fn int be_protectedcall(bvm*, bvalue*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param v (???)
 * @param argc (???)
 * @return (???)
 */
int be_protectedcall(bvm *vm, bvalue *v, int argc);

/**
 * @fn void be_stackpush(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_stackpush(bvm *vm);

/**
 * @fn void be_stack_expansion(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param n (???)
 */
void be_stack_expansion(bvm *vm, int n);

/**
 * @fn void be_except_block_setup(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_except_block_setup(bvm *vm);

/**
 * @fn void be_except_block_resume(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_except_block_resume(bvm *vm);

/**
 * @fn void be_except_block_close(bvm*, int)
 * @brief (???)
 *
 * @param vm (???)
 * @param count (???)
 */
void be_except_block_close(bvm *vm, int count);

/**
 * @fn void be_save_stacktrace(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 */
void be_save_stacktrace(bvm *vm);

#endif
