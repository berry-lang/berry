/**
 * @file       berry_vm.h
 * @brief      Berry file
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_VM_H
#define BE_VM_H

#include "be_object.h"

/**
 * @def comp_is_named_gbl
 * @brief comp_is_named_gbl
 *
 */
#define comp_is_named_gbl(vm)       ((vm)->compopt & (1<<COMP_NAMED_GBL))

/**
 * @def comp_set_named_gbl
 * @brief comp_set_named_gbl
 *
 */
#define comp_set_named_gbl(vm)      ((vm)->compopt |= (1<<COMP_NAMED_GBL))

/**
 * @def comp_clear_named_gbl
 * @brief comp_clear_named_gbl
 *
 */
#define comp_clear_named_gbl(vm)    ((vm)->compopt &= ~(1<<COMP_NAMED_GBL))

/**
 * @def comp_is_strict
 * @brief comp_is_strict
 *
 */
#define comp_is_strict(vm)          ((vm)->compopt & (1<<COMP_STRICT))

/**
 * @def comp_set_strict
 * @brief comp_is_strict
 *
 */
#define comp_set_strict(vm)         ((vm)->compopt |= (1<<COMP_STRICT))

/**
 * @def comp_clear_strict
 * @brief comp_set_strict
 *
 */
#define comp_clear_strict(vm)       ((vm)->compopt &= ~(1<<COMP_STRICT))

/**
 * @typedef compoptmask_t
 * @brief Compilation options
 *
 */
typedef enum {
    COMP_NAMED_GBL = 0x00, /**< compile with named globals */
       COMP_STRICT = 0x01, /**< compile with named globals */
} compoptmask_t;

/**
 * @typedef bglobaldesc_t
 * @brief bglobaldesc_t
 *
 */
typedef struct {
    struct {
        bmap_t *vtab;    /**< global variable index table */
        bvector_t vlist; /**< global variable list */
    } global;
    struct {
        bmap_t *vtab;    /**< built-in variable index table */
        bvector_t vlist; /**< built-in variable list */
    } builtin;
} bglobaldesc_t;

/**
 * @typedef bcallframe_t
 * @brief bcallframe_t
 *
 */
typedef struct {
    bvalue_t *func;        /**< function register pointer */
    bvalue_t *top;         /**< top register pointer */
    bvalue_t *reg;         /**< base register pointer */
    binstruction_t *ip;    /**< instruction pointer (only berry-function) */
#if BE_USE_DEBUG_HOOK
    blineinfo_t *lineinfo; /**< lineinfo */
#endif
    int status;
} bcallframe_t;

struct gc16_t;             /**< memory pool for 0-16 bytes or less objects */
struct gc32_t;             /**< memory pool for 17-32 bytes */

/**
 * @struct bgc
 * @brief bgc
 *
 */
struct bgc {
    bgcobject_t *list;     /**< the GC-object list */
    bgcobject_t *gray;     /**< the gray object list */
    bgcobject_t *fixed;    /**< the fixed object list  */
    struct gc16_t* pool16; /**< pool16 */
    struct gc32_t* pool32; /**< pool32 */
    size_t usage;          /**< the count of bytes currently allocated */
    size_t threshold;      /**< The threshold of allocation for the next GC */
    bbyte_t steprate;      /**< the rate of increase in the distribution between two GCs (percentage) */
    bbyte_t status;        /**< status */
};

/**
 * @struct bstringtable
 * @brief bstringtable
 *
 */
struct bstringtable {
    bstring_t **table; /**< table */
    int count;         /**< string count */
    int size;          /**< size */
};

/**
 * @struct bmoduledesc
 * @brief bmoduledesc
 *
 */
struct bmoduledesc {
    bmap_t *loaded; /**< loaded module map */
    blist_t *path;  /**< module load path list */
};

/**
 * @typedef bcallsnapshot_t
 * @brief bcallsnapshot_t
 *
 */
typedef struct {
    bvalue_t func;      /**< func */
    binstruction_t *ip; /**< ip */
} bcallsnapshot_t;

/**
 * @struct bupval
 * @brief bupval
 *
 */
struct bupval {
    bvalue_t* value;         /**< value */
    union {
        bvalue_t value;      /**< value */
        struct bupval* next; /**< next */
    } u;                     /**< u */
    int refcnt;              /**< refcnt */
};

/**
 * @struct bvm
 * @brief bvm
 *
 */
struct bvm {
    bglobaldesc_t gbldesc;      /**< global description */
    bvalue_t *stack;            /**< stack space */
    bvalue_t *stacktop;         /**< stack top register */
    bupval_t *upvalist;         /**< open upvalue list */
    bstack_t callstack;         /**< function call stack */
    bstack_t exceptstack;       /**< exception stack */
    bcallframe_t *cf;           /**< function call frame */
    bvalue_t *reg;              /**< function base register */
    bvalue_t *top;              /**< function top register */
    binstruction_t *ip;         /**< function instruction pointer */
    struct blongjmp *errjmp;    /**< error jump point */
    bstack_t refstack;          /**< object reference stack */
    struct bmoduledesc module;  /**< module description */
    struct bstringtable strtab; /**< short string table */
    bstack_t tracestack;        /**< call state trace-stack */
    bmap_t *ntvclass;           /**< native class table */
    struct bgc gc;              /**< gc */
    bctypefunc ctypefunc;       /**< handler to ctype_func */
    bbyte_t compopt;            /**< compilation options */
    int32_t bytesmaxsize;       /**< max allowed size for bytes() object, default 32kb but can be increased */
    bobshook obshook;           /**< obshook */
#if BE_USE_PERF_COUNTERS
    uint32_t counter_ins;       /**< instructions counter */
    uint32_t counter_enter;     /**< counter for times the VM was entered */
    uint32_t counter_call;      /**< counter for calls, VM or native */
    uint32_t counter_get;       /**< counter for GETMBR or GETMET */
    uint32_t counter_set;       /**< counter for SETMBR */
    uint32_t counter_try;       /**< counter for `try` statement */
    uint32_t counter_exc;       /**< counter for raised exceptions */
    uint32_t counter_gc_kept;   /**< counter for objects scanned by last gc */
    uint32_t counter_gc_freed;  /**< counter for objects freed by last gc */
#endif
#if BE_USE_DEBUG_HOOK
    bvalue_t hook;              /**< hook */
    bbyte_t hookmask;           /**< hookmask */
#endif
};

/**
 * @def NONE_FLAG
 * @brief NONE_FLAG
 *
 */
#define NONE_FLAG           0

/**
 * @def BASE_FRAME
 * @brief BASE_FRAME
 *
 */
#define BASE_FRAME          (1 << 0)

/**
 * @def PRIM_FUNC
 * @brief PRIM_FUNC
 *
 */
#define PRIM_FUNC           (1 << 1)

/**
 * @fn int be_default_init_native_function(bvm_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
int be_default_init_native_function(bvm_t *vm);

/**
 * @fn void be_dofunc(bvm_t*, bvalue_t*, int)
 * @brief  (???)
 *
 * @param vm (???)
 * @param v (???)
 * @param argc (???)
 */
void be_dofunc(bvm_t *vm, bvalue_t *v, int argc);

/**
 * @fn bool be_value2bool(bvm_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param v (???)
 * @return (???)
 */
bbool be_value2bool(bvm_t *vm, bvalue_t *v);

/**
 * @fn bool be_vm_iseq(bvm_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_iseq(bvm_t *vm, bvalue_t *a, bvalue_t *b);

/**
 * @fn bool be_vm_isneq(bvm_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isneq(bvm_t *vm, bvalue_t *a, bvalue_t *b);

/**
 * @fn bool be_vm_islt(bvm_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_islt(bvm_t *vm, bvalue_t *a, bvalue_t *b);

/**
 * @fn bool be_vm_isle(bvm_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isle(bvm_t *vm, bvalue_t *a, bvalue_t *b);

/**
 * @fn bool be_vm_isgt(bvm_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isgt(bvm_t *vm, bvalue_t *a, bvalue_t *b);

/**
 * @fn bool be_vm_isge(bvm_t*, bvalue_t*, bvalue_t*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isge(bvm_t *vm, bvalue_t *a, bvalue_t *b);

#endif
