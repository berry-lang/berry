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
 * @enum compoptmask
 * @brief Compilation options
 *
 */
typedef enum {
    COMP_NAMED_GBL = 0x00, /**< compile with named globals */
       COMP_STRICT = 0x01, /**< compile with named globals */
} compoptmask;             /**< compoptmask */

/**
 * @struct bglobaldesc
 * @brief bglobaldesc
 *
 */
typedef struct {
    struct {
        bmap *vtab;    /**< global variable index table */
        bvector vlist; /**< global variable list */
    } global;
    struct {
        bmap *vtab;    /**< built-in variable index table */
        bvector vlist; /**< built-in variable list */
    } builtin;
} bglobaldesc;         /**< bglobaldesc */

/**
 * @struct bcallframe
 * @brief bcallframe
 *
 */
typedef struct {
    bvalue *func;        /**< function register pointer */
    bvalue *top;         /**< top register pointer */
    bvalue *reg;         /**< base register pointer */
    binstruction *ip;    /**< instruction pointer (only berry-function) */
#if BE_USE_DEBUG_HOOK
    blineinfo *lineinfo; /**< lineinfo */
#endif
    int status;
} bcallframe;

struct gc16_t;           /**< memory pool for 0-16 bytes or less objects */
struct gc32_t;           /**< memory pool for 17-32 bytes */

/**
 * @struct bgc
 * @brief bgc
 *
 */
struct bgc {
    bgcobject *list;       /**< the GC-object list */
    bgcobject *gray;       /**< the gray object list */
    bgcobject *fixed;      /**< the fixed objecct list  */
    struct gc16_t* pool16; /**< pool16 */
    struct gc32_t* pool32; /**< pool32 */
    size_t usage;          /**< the count of bytes currently allocated */
    size_t threshold;      /**< The threshold of allocation for the next GC */
    bbyte steprate;        /**< the rate of increase in the distribution between two GCs (percentage) */
    bbyte status;          /**< status */
};

/**
 * @struct bstringtable
 * @brief bstringtable
 *
 */
struct bstringtable {
    bstring **table; /**< table */
    int count;       /**< string count */
    int size;        /**< size */
};

/**
 * @struct bmoduledesc
 * @brief bmoduledesc
 *
 */
struct bmoduledesc {
    bmap *loaded; /**< loaded module map */
    blist *path;  /**< module load path list */
};

/**
 * @struct bcallsnapshot
 * @brief bcallsnapshot
 *
 */
typedef struct {
    bvalue func;      /**< func */
    binstruction *ip; /**< ip */
} bcallsnapshot;      /**< bcallsnapshot */

/**
 * @struct bupval
 * @brief bupval
 *
 */
struct bupval {
    bvalue* value;           /**< value */
    union {
        bvalue value;        /**< value */
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
    bglobaldesc gbldesc;        /**< global description */
    bvalue *stack;              /**< stack space */
    bvalue *stacktop;           /**< stack top register */
    bupval *upvalist;           /**< open upvalue list */
    bstack callstack;           /**< function call stack */
    bstack exceptstack;         /**< exception stack */
    bcallframe *cf;             /**< function call frame */
    bvalue *reg;                /**< function base register */
    bvalue *top;                /**< function top register */
    binstruction *ip;           /**< function instruction pointer */
    struct blongjmp *errjmp;    /**< error jump point */
    bstack refstack;            /**< object reference stack */
    struct bmoduledesc module;  /**< module description */
    struct bstringtable strtab; /**< short string table */
    bstack tracestack;          /**< call state trace-stack */
    bmap *ntvclass;             /**< native class table */
    struct bgc gc;              /**< gc */
    bctypefunc ctypefunc;       /**< handler to ctype_func */
    bbyte compopt;              /**< compilation options */
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
    bvalue hook;                /**< hook */
    bbyte hookmask;             /**< hookmask */
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
 * @fn int be_default_init_native_function(bvm*)
 * @brief (???)
 *
 * @param vm (???)
 * @return (???)
 */
int be_default_init_native_function(bvm *vm);

/**
 * @fn void be_dofunc(bvm*, bvalue*, int)
 * @brief  (???)
 *
 * @param vm (???)
 * @param v (???)
 * @param argc (???)
 */
void be_dofunc(bvm *vm, bvalue *v, int argc);

/**
 * @fn bool be_value2bool(bvm*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param v (???)
 * @return (???)
 */
bbool be_value2bool(bvm *vm, bvalue *v);

/**
 * @fn bool be_vm_iseq(bvm*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_iseq(bvm *vm, bvalue *a, bvalue *b);

/**
 * @fn bool be_vm_isneq(bvm*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isneq(bvm *vm, bvalue *a, bvalue *b);

/**
 * @fn bool be_vm_islt(bvm*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_islt(bvm *vm, bvalue *a, bvalue *b);

/**
 * @fn bool be_vm_isle(bvm*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isle(bvm *vm, bvalue *a, bvalue *b);

/**
 * @fn bool be_vm_isgt(bvm*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isgt(bvm *vm, bvalue *a, bvalue *b);

/**
 * @fn bool be_vm_isge(bvm*, bvalue*, bvalue*)
 * @brief (???)
 *
 * @param vm (???)
 * @param a (???)
 * @param b (???)
 * @return (???)
 */
bbool be_vm_isge(bvm *vm, bvalue *a, bvalue *b);

#endif
