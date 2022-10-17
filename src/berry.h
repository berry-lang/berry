/**
 * @file       berry.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2020 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BERRY_H
#define BERRY_H

#include <stddef.h>
#include <stdint.h>

#include "berry_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def BERRY_VERSION
 * @brief do not modify the version number!
 *
 */
#define BERRY_VERSION           "1.1.0"

#if BE_STACK_TOTAL_MAX < BE_STACK_FREE_MIN * 2
#error "The value of the macro BE_STACK_TOTAL_MAX is too small."
#endif

/**
 * @def BE_INTEGER
 * @brief basic type definition
 *
 */
#if BE_INTGER_TYPE == 0
  #define BE_INTEGER            int
  #define BE_INT_FMTLEN         ""
#elif BE_INTGER_TYPE == 1
  #define BE_INTEGER            long
  #define BE_INT_FMTLEN         "l"
#elif BE_INTGER_TYPE == 2
#ifdef _WIN32
    #define BE_INTEGER          __int64
    #define BE_INT_FMTLEN       "I64"
  #else
#define BE_INTEGER              long long
#define BE_INT_FMTLEN           "ll"
#endif
#else
  #error "Unsupported integer type for `bint`."
#endif
#define BE_INT_FORMAT           "%" BE_INT_FMTLEN "d"

typedef uint8_t bbyte;
typedef BE_INTEGER bint;

#if BE_USE_SINGLE_FLOAT != 0
  typedef float                 breal;
#else
typedef double breal;
#endif


/**
 * @{ \name boolean values definition.
 */
/**
 * \brief boolean values definition.
 *
 */
#ifndef __cplusplus
#define bbool                   _Bool
#define bfalse                  0
#define btrue                   1
#else
  #define bbool                 bool
  #define bfalse                false
  #define btrue                 true
#endif
/**
 * @}
 */

/**
 * @enum berrorcode
 * @brief error code definition
 *
 */
enum berrorcode {
    BE_OK = 0,       /**< BE_OK */
    BE_EXIT,         /**< BE_EXIT */
    BE_MALLOC_FAIL,  /**< BE_MALLOC_FAIL */
    BE_EXCEPTION,    /**< BE_EXCEPTION */
    BE_SYNTAX_ERROR, /**< BE_SYNTAX_ERROR */
    BE_EXEC_ERROR,   /**< BE_EXEC_ERROR */
    BE_IO_ERROR      /**< BE_IO_ERROR */
};

/**
 * @{ \name native-module member type specifier.
 */
/**
 * \brief native-module member type specifier.
 *
 */
#define BE_CNIL        0 /**< */
#define BE_CINT        1 /**< */
#define BE_CREAL       2 /**< */
#define BE_CBOOL       3 /**< */
#define BE_CFUNCTION   4 /**< */
#define BE_CSTRING     5 /**< */
#define BE_CMODULE     6 /**< */
/**
 * @}
 */


/**
 * @def BERRY_API
 * @brief API function mark
 *
 */
#if defined(_WIN32) || defined(__CYGWIN__) /* in Windows */
  #if defined(BERRY_MODULE) /* berry extension module */
    #define BERRY_API           __declspec(dllimport)
  #else /* berry core */
    #define BERRY_API           __declspec(dllexport)
  #endif
#else /* other platforms */
#define BERRY_API             extern
#endif

/**
 * @{ \name only linux.
 */
/**
 * \brief only linux
 *
 */
#if defined(_WIN32) || defined(__CYGWIN__) /* in Windows */
  #define BERRY_LOCAL
#elif defined(__GNUC__) /* in GCC */
#define BERRY_LOCAL           __attribute__ ((visibility ("hidden")))
#else /* other platforms */
  #define BERRY_LOCAL
#endif

#ifdef __cplusplus
#ifdef __cpp_constexpr
  #define BE_CONSTEXPR          constexpr
#else
  #define BE_CONSTEXPR
#endif
#endif

#ifdef __cplusplus
#define BE_EXPORT_VARIABLE      extern "C"
#else
#define BE_EXPORT_VARIABLE
#endif
/**
 * @}
 */

/**
 * @brief virtual machine structure
 */
typedef struct bvm bvm;

/**
 * @fn int (*)(bvm*)
 * @brief native function pointer
 *
 * @param
 * @return
 */
typedef int (*bntvfunc)(bvm*);

/**
 * @brief
 */
struct bclass;

/**
 * @struct bnfuncinfo
 * @brief native function information
 *
 */
typedef struct {
    const char *name;  /**< */
    bntvfunc function; /**< */
} bnfuncinfo;

/**
 * @struct bntvmodobj
 * @brief native module object node
 *
 */
typedef struct bntvmodobj {
    const char *name;                               /**< */
    int type;                                       /**< */
    union value {
        bint i;                                     /**< */
        breal r;                                    /**< */
        bbool b;                                    /**< */
        bntvfunc f;                                 /**< */
        const char *s;                              /**< */
        const void *o;                              /**< */
#ifdef __cplusplus
        BE_CONSTEXPR value(bint v) : i(v) {}        /**< */
        BE_CONSTEXPR value(breal v) : r(v) {}       /**< */
        BE_CONSTEXPR value(bbool v) : b(v) {}       /**< */
        BE_CONSTEXPR value(bntvfunc v) : f(v) {}    /**< */
        BE_CONSTEXPR value(const char *v) : s(v) {} /**< */
        BE_CONSTEXPR value(const void *v) : o(v) {} /**< */
#endif
    } u;
#ifdef __cplusplus
    BE_CONSTEXPR bntvmodobj(const char *name) :
        name(name), type(BE_CNIL), u(bint(0)) {}    /**< */
    BE_CONSTEXPR bntvmodobj(const char *name, bint v) :
        name(name), type(BE_CINT), u(v) {}          /**< */
    BE_CONSTEXPR bntvmodobj(const char *name, breal v) :
        name(name), type(BE_CREAL), u(v) {}         /**< */
    BE_CONSTEXPR bntvmodobj(const char *name, bbool v) :
        name(name), type(BE_CBOOL), u(v) {}         /**< */
    BE_CONSTEXPR bntvmodobj(const char *name, bntvfunc v) :
        name(name), type(BE_CFUNCTION), u(v) {}     /**< */
    BE_CONSTEXPR bntvmodobj(const char *name, const char *v) :
        name(name), type(BE_CSTRING), u(v) {}       /**< */
    BE_CONSTEXPR bntvmodobj(const char *name, int _tpye, const void *v) :
        name(name), type(_tpye), u(v) {}            /**< */
#endif
} bntvmodobj;

/**
 * @struct bntvmodule
 * @brief native module object
 *
 */
typedef struct bntvmodule {
    const char *name;              /**< native module name */
    const bntvmodobj *attrs;       /**< native module attributes */
    size_t size;                   /**< native module attribute count */
    const struct bmodule *module;  /**< const module object */
} bntvmodule;

/* native class object */
struct bclass;
/* we need only the pointer to `bclass` here */
typedef const struct bclass *bclass_ptr;
typedef bclass_ptr bclass_array[]; /* array of bclass* pointers, NULL terminated */

/* native module node definition macro */
#ifndef __cplusplus
#define be_native_module_nil(_name)                     \
    { .name = (_name), .type = BE_CNIL, .u.i = 0 }

#define be_native_module_int(_name, _v)                 \
    { .name = (_name), .type = BE_CINT, .u.i = (bint)(_v) }

#define be_native_module_real(_name, _v)                \
    { .name = (_name), .type = BE_CREAL, .u.r = (breal)(_v) }

#define be_native_module_bool(_name, _b)                \
    { .name = (_name), .type = BE_CBOOL, .u.b = (bbool)(_b) }

#define be_native_module_function(_name, _f)            \
    { .name = (_name), .type = BE_CFUNCTION, .u.f = (_f) }

#define be_native_module_str(_name, _s)                 \
    { .name = (_name), .type = BE_CSTRING, .u.s = (_s) }

#define be_native_module_module(_name, _m)              \
    { .name = (_name), .type = BE_CMODULE, .u.o = &(_m) }
#else
#define be_native_module_nil(_name)                     \
    bntvmodobj(_name)

#define be_native_module_int(_name, _v)                 \
    bntvmodobj(_name, bint(_v))

#define be_native_module_real(_name, _v)                \
    bntvmodobj(_name, breal(_v))

#define be_native_module_bool(_name, _b)                \
    bntvmodobj(_name, bbool(_b))

#define be_native_module_function(_name, _f)            \
    bntvmodobj(_name, _f)

#define be_native_module_str(_name, _s)                 \
    bntvmodobj(_name, _s)

#define be_native_module_module(_name, _m)              \
    bntvmodobj(_name, BE_CMODULE, &(_m))
#endif

#define be_native_module_attr_table(name)               \
    static const bntvmodobj name##_attrs[] =

#define be_native_module(name)  be_native_module_##name
#define be_native_class(name)  be_class_##name

/**
 * @def be_extern_native_module
 * @brief native module declaration macro
 *
 */
#define be_extern_native_module(name)                   \
    extern const bntvmodule be_native_module(name)

/**
 * @def be_extern_native_class
 * @brief native class declaration macro
 *
 */
#define be_extern_native_class(name)                   \
    extern const struct bclass be_native_class(name)

/**
 * @def be_define_native_module
 * @brief native module definition macro
 *
 */
#ifndef __cplusplus
#define be_define_native_module(_name, _init)           \
    const bntvmodule be_native_module(_name) = {        \
        .name = #_name,                                 \
        .attrs = _name##_attrs,                         \
        .size = sizeof(_name##_attrs)                   \
               / sizeof(_name##_attrs[0]),              \
        .module = NULL,                                 \
        .init = _init                                   \
    }
#else
#define be_define_native_module(_name, _init)           \
    const bntvmodule be_native_module(_name) = {        \
        #_name, _name##_attrs,                          \
        sizeof(_name##_attrs)                           \
            / sizeof(_name##_attrs[0]),                 \
        0, _init                                        \
    }
#endif

/**
 * @def be_define_local_const_str
 * @brief support for solidified berry functions
 *        native const strings outside of global string hash
 *
 */
#define be_define_local_const_str(_name, _s, _hash, _len) \
    static const bcstring be_local_const_str_##_name = {  \
        .next = (bgcobject *)NULL,                        \
        .type = BE_STRING,                                \
        .marked = GC_CONST,                               \
        .extra = 0,                                       \
        .slen = _len,                                     \
        .hash = 0,                                        \
        .s = _s                                           \
    }

/**
 * @def be_nested_const_str
 * @brief new version for more compact literals
 *
 */
#define be_nested_const_str(_s, _hash, _len)  \
    (bstring*) &(const bcstring) {            \
        .next = (bgcobject *)NULL,            \
        .type = BE_STRING,                    \
        .marked = GC_CONST,                   \
        .extra = 0,                           \
        .slen = _len,                         \
        .hash = 0,                            \
        .s = _s                               \
    }

/**
 * @def be_local_const_str
 * @brief
 *
 */
#define be_local_const_str(_name) (bstring*) &be_local_const_str_##_name

/**
 * @{ \name BE_IIF
 */
/**
 * @def BE_IIF
 * @brief conditional macro see  https://stackoverflow.com/questions/11632219/c-preprocessor-macro-specialisation-based-on-an-argument
 *
 */
#define BE_IIF(cond) BE_IIF_ ## cond
#define BE_IIF_0(t, f) f
#define BE_IIF_1(t, f) t
/**
 * @}
 */

/**
 * @def be_local_const_upval
 * @brief
 *
 */
#if BE_DEBUG_VAR_INFO
#define be_local_const_upval(ins, idx) { "", ins, idx }
#else
  #define be_local_const_upval(ins, idx) { ins, idx }
#endif

/**
 * @def PROTO_RUNTIME_BLOCK
 * @brief conditional block in bproto depending on compilation options
 *
 */
#if BE_DEBUG_RUNTIME_INFO
#define PROTO_RUNTIME_BLOCK   \
    NULL,     /**< varinfo */ \
    0,        /**< nvarinfo */
#else
  #define PROTO_RUNTIME_BLOCK
#endif

/**
 * @def PROTO_VAR_INFO_BLOCK
 * @brief
 *
 */
#if BE_DEBUG_VAR_INFO
#define PROTO_VAR_INFO_BLOCK   \
    NULL,     /**< varinfo */  \
    0,        /**< nvarinfo */
#else
  #define PROTO_VAR_INFO_BLOCK
#endif

/**
 * @def be_define_local_proto
 * @brief define bproto
 *
 */
#define be_define_local_proto(_name, _nstack, _argc, _is_const, _is_subproto, _is_upval)           \
  static const bproto _name##_proto = {                                           \
    NULL,                                                             /**< bgcobject *next */      \
    BE_PROTO,                                                         /**< type BE_PROTO */        \
    0x08,                                                             /**< marked outside of GC */ \
    (_nstack),                                                        /**< nstack */               \
    BE_IIF(_is_upval)(sizeof(_name##_upvals)/sizeof(bupvaldesc),0),   /**< nupvals */              \
    (_argc),                                                          /**< argc */                 \
    0,                                                                /**< varg */                 \
    NULL,                                                             /**< bgcobject *gray */      \
    BE_IIF(_is_upval)((bupvaldesc*)&_name##_upvals,NULL),             /**< bupvaldesc *upvals */   \
    BE_IIF(_is_const)((bvalue*)&_name##_ktab,NULL),                   /**< ktab */                 \
    BE_IIF(_is_subproto)((struct bproto**)&_name##_subproto,NULL),    /**< bproto **ptab */        \
    (binstruction*) &_name##_code,                                    /**< code */                 \
    be_local_const_str(_name##_str_name),                             /**< name */                 \
    sizeof(_name##_code)/sizeof(uint32_t),                            /**< codesize */             \
    BE_IIF(_is_const)(sizeof(_name##_ktab)/sizeof(bvalue),0),         /**< nconst */               \
    BE_IIF(_is_subproto)(sizeof(_name##_subproto)/sizeof(bproto*),0), /**< proto */                \
    be_local_const_str(_name##_str_source),                           /**< source */               \
    PROTO_RUNTIME_BLOCK                                               /**< */                      \
    PROTO_VAR_INFO_BLOCK                                              /**< */                      \
  }

/**
 * @def be_nested_proto
 * @brief new version for more compact literals
 *
 */
#define be_nested_proto(_nstack, _argc, _varg, _has_upval, _upvals, _has_subproto, _protos, _has_const, _ktab, _fname, _source, _code)     \
  & (const bproto) {                                                              \
    NULL,                                                       /**< bgcobject *next */      \
    BE_PROTO,                                                   /**< type BE_PROTO */        \
    0x08,                                                       /**< marked outside of GC */ \
    (_nstack),                                                  /**< nstack */               \
    BE_IIF(_has_upval)(sizeof(*_upvals)/sizeof(bupvaldesc),0),  /**< nupvals */              \
    (_argc),                                                    /**< argc */                 \
    (_varg),                                                    /**< varg */                 \
    NULL,                                                       /**< bgcobject *gray */      \
    (bupvaldesc*) _upvals,                                      /**< bupvaldesc *upvals */   \
    (bvalue*) _ktab,                                            /**< ktab */                 \
    (struct bproto**) _protos,                                  /**< bproto **ptab */        \
    (binstruction*) _code,                                      /**< code */                 \
    ((bstring*) _fname),                                        /**< name */                 \
    sizeof(*_code)/sizeof(binstruction),                        /**< codesize */             \
    BE_IIF(_has_const)(sizeof(*_ktab)/sizeof(bvalue),0),        /**< nconst */               \
    BE_IIF(_has_subproto)(sizeof(*_protos)/sizeof(bproto*),0),  /**< proto */                \
    ((bstring*) _source),                                       /**< source */               \
    PROTO_RUNTIME_BLOCK                                         /**< */                      \
    PROTO_VAR_INFO_BLOCK                                        /**< */                      \
  }

/**
 * @def be_define_local_closure
 * @brief
 *
 */
#define be_define_local_closure(_name)                \
  const bclosure _name##_closure = {                  \
    NULL,                     /**< bgcobject *next */ \
    BE_CLOSURE,               /**< type BE_CLOSURE */ \
    GC_CONST,                 /**< marked GC_CONST */ \
    0,                        /**< nupvals */         \
    NULL,                     /**< bgcobject *gray */ \
    (bproto*) &_name##_proto, /**< proto */           \
    { NULL }                  /**< upvals */          \
  }

/**
 * @def be_local_closure
 * @brief new version for more compact literals
 *
 */
#define be_local_closure(_name, _proto)       \
  static const bclosure _name##_closure = {   \
    NULL,             /**< bgcobject *next */ \
    BE_CLOSURE,       /**< type BE_CLOSURE */ \
    GC_CONST,         /**< marked GC_CONST */ \
    0,                /**< nupvals */         \
    NULL,             /**< bgcobject *gray */ \
    (bproto*) _proto, /**< proto */           \
    { NULL }          /**< upvals */          \
  }

/**
 * @{ \name debug hook typedefs.
 */
/**
 * \brief debug hook typedefs.
 *
 */
#define BE_HOOK_LINE    1 /**< */
#define BE_HOOK_CALL    2 /**< */
#define BE_HOOK_RET     4 /**< */
#define BE_HOOK_EXCEPT  8 /**< */
/**
 * @}
 */

/**
 * @struct bhookinfo
 * @brief
 *
 */
typedef struct bhookinfo {
    int type;               /**< current hook type */
    int line;               /**< current line number */
    const char *source;     /**< source path information */
    const char *func_name;  /**< current function name */
    void *data;             /**< user extended data */
} bhookinfo;

/**
 * @fn void (*)(bvm*, bhookinfo*)
 * @brief
 *
 * @param vm
 * @param info
 */
typedef void (*bntvhook)(bvm *vm, bhookinfo *info);

/**
 * @def be_assert
 * @brief the default assert definition
 *
 */
#if !BE_DEBUG
#if defined(be_assert)
#undef be_assert
#endif
#define be_assert(expr)       ((void)0)
#endif

/**
 * @fn void (*)(bvm*, int, ...)
 * @brief Observability hook
 *
 * @param vm
 * @param event
 */
typedef void (*bobshook)(bvm *vm, int event, ...);

/**
 * @enum beobshookevents
 * @brief
 *
 */
enum beobshookevents {
    BE_OBS_PCALL_ERROR,         /**< called when be_callp() returned an error, most likely an exception */
    BE_OBS_GC_START,            /**< start of GC, arg = allocated size */
    BE_OBS_GC_END,              /**< end of GC, arg = allocated size */
    BE_OBS_VM_HEARTBEAT,        /**< VM heartbeat called every million instructions */
    BE_OBS_STACK_RESIZE_START,  /**< Berry stack resized */
};

/**
 * @fn int (*)(bvm*, const void*)
 * @brief
 *
 * @param
 * @param
 * @return
 */
typedef int (*bctypefunc)(bvm*, const void*);

/**
 * @def be_writestring
 * @note FFI function
 * @brief
 *
 */
#define be_writestring(s)       be_writebuffer((s), strlen(s))

/**
 * @def be_writenewline
 * @note FFI function
 * @brief
 *
 */
#define be_writenewline()       be_writebuffer("\n", 1)

/**
 * @def be_return
 * @note FFI function
 * @brief
 *
 */
#define be_return(vm)           return be_returnvalue(vm)

/**
 * @def be_return_nil
 * @note FFI function
 * @brief
 *
 */
#define be_return_nil(vm)       return be_returnnilvalue(vm)

/**
 * @def be_loadfile
 * @note FFI function
 * @brief
 *
 */
#define be_loadfile(vm, name)   be_loadmode((vm), (name), 0)

/**
 * @def be_loadmodule
 * @note FFI function
 * @brief
 *
 */
#define be_loadmodule(vm, name) be_loadmode((vm), (name), 1)

/**
 * @def be_loadstring
 * @note FFI function
 * @brief
 *
 */
#define be_loadstring(vm, str) \
    be_loadbuffer((vm), "string", (str), strlen(str))

/**
 * @def be_dostring
 * @note FFI function
 * @brief
 *
 */
#define be_dostring(vm, s) \
    (be_loadstring((vm), (s)) || be_pcall((vm), 0))

/**
 * @fn bint be_str2int(const char*, const char**)
 * @note FFI function
 * @brief
 *
 * @param str
 * @param endstr
 * @return
 */
BERRY_API bint be_str2int(const char *str, const char **endstr);

/**
 * @fn breal be_str2real(const char*, const char**)
 * @note FFI function
 * @brief
 *
 * @param str
 * @param endstr
 * @return
 */
BERRY_API breal be_str2real(const char *str, const char **endstr);

/**
 * @fn const char be_str2num*(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param str
 * @return
 */
BERRY_API const char* be_str2num(bvm *vm, const char *str);

/**
 * @fn int be_top(bvm*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API int be_top(bvm *vm);

/**
 * @fn const char be_typename*(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API const char* be_typename(bvm *vm, int index);

/**
 * @fn const char be_classname*(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API const char* be_classname(bvm *vm, int index);

/**
 * @fn bool be_classof(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_classof(bvm *vm, int index);

/**
 * @fn int be_strlen(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API int be_strlen(bvm *vm, int index);

/**
 * @fn void be_strconcat(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_strconcat(bvm *vm, int index);

/**
 * @fn void be_pop(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param n
 */
BERRY_API void be_pop(bvm *vm, int n);

/**
 * @fn void be_remove(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_remove(bvm *vm, int index);

/**
 * @fn int be_absindex(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API int be_absindex(bvm *vm, int index);

/**
 * @fn bool be_isnil(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isnil(bvm *vm, int index);

/**
 * @fn bool be_isbool(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isbool(bvm *vm, int index);

/**
 * @fn bool be_isint(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isint(bvm *vm, int index);

/**
 * @fn bool be_isreal(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isreal(bvm *vm, int index);

/**
 * @fn bool be_isnumber(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isnumber(bvm *vm, int index);

/**
 * @fn bool be_isstring(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isstring(bvm *vm, int index);

/**
 * @fn bool be_isclosure(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isclosure(bvm *vm, int index);

/**
 * @fn bool be_isntvclos(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isntvclos(bvm *vm, int index);

/**
 * @fn bool be_isfunction(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isfunction(bvm *vm, int index);

/**
 * @fn bool be_isproto(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isproto(bvm *vm, int index);

/**
 * @fn bool be_isclass(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isclass(bvm *vm, int index);

/**
 * @fn bool be_isinstance(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isinstance(bvm *vm, int index);

/**
 * @fn bool be_ismodule(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_ismodule(bvm *vm, int index);

/**
 * @fn bool be_islist(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_islist(bvm *vm, int index);

/**
 * @fn bool be_ismap(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_ismap(bvm *vm, int index);

/**
 * @fn bool be_iscomptr(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_iscomptr(bvm *vm, int index);

/**
 * @fn bool be_iscomobj(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_iscomobj(bvm *vm, int index);

/**
 * @fn bool be_isderived(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isderived(bvm *vm, int index);

/**
 * @fn bool be_isbytes(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_isbytes(bvm *vm, int index);

/**
 * @fn bint be_toint(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bint be_toint(bvm *vm, int index);

/**
 * @fn breal be_toreal(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API breal be_toreal(bvm *vm, int index);

/**
 * @fn int be_toindex(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API int be_toindex(bvm *vm, int index);

/**
 * @fn bool be_tobool(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_tobool(bvm *vm, int index);

/**
 * @fn const char be_tostring*(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API const char* be_tostring(bvm *vm, int index);

/**
 * @fn const char be_toescape*(bvm*, int, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param mode
 * @return
 */
BERRY_API const char* be_toescape(bvm *vm, int index, int mode);

/**
 * @fn void be_tocomptr*(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void* be_tocomptr(bvm *vm, int index);

/**
 * @fn void be_moveto(bvm*, int, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param from
 * @param to
 */
BERRY_API void be_moveto(bvm *vm, int from, int to);

/**
 * @fn void be_pushnil(bvm*)
 * @note FFI function
 * @brief
 *
 * @param vm
 */
BERRY_API void be_pushnil(bvm *vm);

/**
 * @fn void be_pushbool(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param b
 */
BERRY_API void be_pushbool(bvm *vm, int b);

/**
 * @fn void be_pushint(bvm*, bint)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param i
 */
BERRY_API void be_pushint(bvm *vm, bint i);

/**
 * @fn void be_pushreal(bvm*, breal)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param r
 */
BERRY_API void be_pushreal(bvm *vm, breal r);

/**
 * @fn void be_pushstring(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param str
 */
BERRY_API void be_pushstring(bvm *vm, const char *str);

/**
 * @fn void be_pushnstring(bvm*, const char*, size_t)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param str
 * @param n
 */
BERRY_API void be_pushnstring(bvm *vm, const char *str, size_t n);

/**
 * @fn const char be_pushfstring*(bvm*, const char*, ...)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param format
 * @return
 */
BERRY_API const char* be_pushfstring(bvm *vm, const char *format, ...);

/**
 * @fn void be_pushbuffer*(bvm*, size_t)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param size
 */
BERRY_API void* be_pushbuffer(bvm *vm, size_t size);

/**
 * @fn void be_pushvalue(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_pushvalue(bvm *vm, int index);

/**
 * @fn void be_pushclosure(bvm*, void*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param cl
 */
BERRY_API void be_pushclosure(bvm *vm, void *cl);

/**
 * @fn void be_pushntvclosure(bvm*, bntvfunc, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param f
 * @param nupvals
 */
BERRY_API void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);

/**
 * @fn void be_pushntvfunction(bvm*, bntvfunc)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param f
 */
BERRY_API void be_pushntvfunction(bvm *vm, bntvfunc f);

/**
 * @fn void be_pushclass(bvm*, const char*, const bnfuncinfo*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param name
 * @param lib
 */
BERRY_API void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);

/**
 * @fn void be_pushntvclass(bvm*, const struct bclass*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param c
 */
BERRY_API void be_pushntvclass(bvm *vm, const struct bclass *c);

/**
 * @fn void be_pushcomptr(bvm*, void*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param ptr
 */
BERRY_API void be_pushcomptr(bvm *vm, void *ptr);

/**
 * @fn bool be_pushiter(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_pushiter(bvm *vm, int index);

/**
 * @fn void be_newlist(bvm*)
 * @note FFI function
 * @brief
 *
 * @param vm
 */
BERRY_API void be_newlist(bvm *vm);

/**
 * @fn void be_newmap(bvm*)
 * @note FFI function
 * @brief
 *
 * @param vm
 */
BERRY_API void be_newmap(bvm *vm);

/**
 * @fn void be_newmodule(bvm*)
 * @note FFI function
 * @brief
 *
 * @param vm
 */
BERRY_API void be_newmodule(bvm *vm);

/**
 * @fn void be_newcomobj(bvm*, void*, bntvfunc)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param data
 * @param destroy
 */
BERRY_API void be_newcomobj(bvm *vm, void *data, bntvfunc destroy);

/**
 * @fn void be_newobject(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param name
 */
BERRY_API void be_newobject(bvm *vm, const char *name);

/**
 * @fn bool be_copy(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_copy(bvm *vm, int index);

/**
 * @fn bool be_setname(bvm*, int, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param name
 * @return
 */
BERRY_API bbool be_setname(bvm *vm, int index, const char *name);

/**
 * @fn bool be_getglobal(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param name
 * @return
 */
BERRY_API bbool be_getglobal(bvm *vm, const char *name);

/**
 * @fn void be_setglobal(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param name
 */
BERRY_API void be_setglobal(bvm *vm, const char *name);

/**
 * @fn bool be_getbuiltin(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param name
 * @return
 */
BERRY_API bbool be_getbuiltin(bvm *vm, const char *name);

/**
 * @fn bool be_setmember(bvm*, int, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param k
 * @return
 */
BERRY_API bbool be_setmember(bvm *vm, int index, const char *k);

/**
 * @fn bool be_getmember(bvm*, int, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param k
 * @return
 */
BERRY_API bbool be_getmember(bvm *vm, int index, const char *k);

/**
 * @fn bool be_getmethod(bvm*, int, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param k
 * @return
 */
BERRY_API bbool be_getmethod(bvm *vm, int index, const char *k);

/**
 * @fn bool be_getindex(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_getindex(bvm *vm, int index);

/**
 * @fn bool be_setindex(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_setindex(bvm *vm, int index);

/**
 * @fn void be_getupval(bvm*, int, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param pos
 */
BERRY_API void be_getupval(bvm *vm, int index, int pos);

/**
 * @fn bool be_setupval(bvm*, int, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @param pos
 * @return
 */
BERRY_API bbool be_setupval(bvm *vm, int index, int pos);

/**
 * @fn bool be_setsuper(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_setsuper(bvm *vm, int index);

/**
 * @fn void be_getsuper(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_getsuper(bvm *vm, int index);

/**
 * @fn int be_data_size(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API int be_data_size(bvm *vm, int index);

/**
 * @fn void be_data_push(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_data_push(bvm *vm, int index);

/**
 * @fn bool be_data_insert(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_data_insert(bvm *vm, int index);

/**
 * @fn bool be_data_remove(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_data_remove(bvm *vm, int index);

/**
 * @fn bool be_data_merge(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_data_merge(bvm *vm, int index);

/**
 * @fn void be_data_resize(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_data_resize(bvm *vm, int index);

/**
 * @fn void be_data_reverse(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_data_reverse(bvm *vm, int index);

/**
 * @fn int be_iter_next(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API int be_iter_next(bvm *vm, int index);

/**
 * @fn bool be_iter_hasnext(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_iter_hasnext(bvm *vm, int index);

/**
 * @fn bool be_refcontains(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 * @return
 */
BERRY_API bbool be_refcontains(bvm *vm, int index);

/**
 * @fn void be_refpush(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_refpush(bvm *vm, int index);

/**
 * @fn void be_refpop(bvm*)
 * @note FFI function
 * @brief
 *
 * @param vm
 */
BERRY_API void be_refpop(bvm *vm);

/**
 * @fn void be_stack_require(bvm*, int)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param count
 */
BERRY_API void be_stack_require(bvm *vm, int count);

/**
 * @fn bool be_getmodule(bvm*, const char*)
 * @note FFI function
 * @brief
 *
 * @param vm
 * @param k
 * @return
 */
BERRY_API bbool be_getmodule(bvm *vm, const char *k);

/**
 * @fn bool be_iseq(bvm*)
 * @note relop operation API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bbool be_iseq(bvm *vm);

/**
 * @fn bool be_isneq(bvm*)
 * @note relop operation API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bbool be_isneq(bvm *vm);

/**
 * @fn bool be_islt(bvm*)
 * @note relop operation API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bbool be_islt(bvm *vm);

/**
 * @fn bool be_isle(bvm*)
 * @note relop operation API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bbool be_isle(bvm *vm);

/**
 * @fn bool be_isgt(bvm*)
 * @note relop operation API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bbool be_isgt(bvm *vm);

/**
 * @fn bool be_isge(bvm*)
 * @note relop operation API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bbool be_isge(bvm *vm);

/**
 * @fn int be_returnvalue(bvm*)
 * @note Function call/return API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API int be_returnvalue(bvm *vm);

/**
 * @fn int be_returnnilvalue(bvm*)
 * @note Function call/return API
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API int be_returnnilvalue(bvm *vm);

/**
 * @fn void be_call(bvm*, int)
 * @note Function call/return API
 * @brief
 *
 * @param vm
 * @param argc
 */
BERRY_API void be_call(bvm *vm, int argc);

/**
 * @fn int be_pcall(bvm*, int)
 * @note Function call/return API
 * @brief
 *
 * @param vm
 * @param argc
 * @return
 */
BERRY_API int be_pcall(bvm *vm, int argc);

/**
 * @fn void be_exit(bvm*, int)
 * @note Function call/return API
 * @brief
 *
 * @param vm
 * @param status
 */
BERRY_API void be_exit(bvm *vm, int status);

/**
 * @fn void be_raise(bvm*, const char*, const char*)
 * @note exception API
 * @brief
 *
 * @param vm
 * @param except
 * @param msg
 */
BERRY_API void be_raise(bvm *vm, const char *except, const char *msg);

/**
 * @fn int be_getexcept(bvm*, int)
 * @note exception API
 * @brief
 *
 * @param vm
 * @param code
 * @return
 */
BERRY_API int be_getexcept(bvm *vm, int code);

/**
 * @fn void be_dumpvalue(bvm*, int)
 * @note exception API
 * @brief
 *
 * @param vm
 * @param index
 */
BERRY_API void be_dumpvalue(bvm *vm, int index);

/**
 * @fn void be_dumpexcept(bvm*)
 * @note exception API
 * @brief
 *
 * @param vm
 */
BERRY_API void be_dumpexcept(bvm *vm);

/**
 * @fn void be_stop_iteration(bvm*)
 * @note exception API
 * @brief
 *
 * @param vm
 */
BERRY_API void be_stop_iteration(bvm *vm);

/**
 * @fn void be_regfunc(bvm*, const char*, bntvfunc)
 * @note exception API
 * @brief
 *
 * @param vm
 * @param name
 * @param f
 */
BERRY_API void be_regfunc(bvm *vm, const char *name, bntvfunc f);

/**
 * @fn void be_regclass(bvm*, const char*, const bnfuncinfo*)
 * @note exception API
 * @brief
 *
 * @param vm
 * @param name
 * @param lib
 */
BERRY_API void be_regclass(bvm *vm, const char *name, const bnfuncinfo *lib);

/**
 * @fn bvm be_vm_new*(void)
 * @note VM management API
 * @brief
 *
 * @return
 */
BERRY_API bvm* be_vm_new(void);

/**
 * @fn void be_vm_delete(bvm*)
 * @note VM management API
 * @brief
 *
 * @param vm
 */
BERRY_API void be_vm_delete(bvm *vm);

/**
 * @fn void be_set_obs_hook(bvm*, bobshook)
 * @note Observability hook
 * @brief
 *
 * @param vm
 * @param hook
 */
BERRY_API void be_set_obs_hook(bvm *vm, bobshook hook);

/**
 * @fn void be_set_ctype_func_hanlder(bvm*, bctypefunc)
 * @note Observability hook
 * @brief
 *
 * @param vm
 * @param handler
 */
BERRY_API void be_set_ctype_func_hanlder(bvm *vm, bctypefunc handler);

/**
 * @fn bctypefunc be_get_ctype_func_hanlder(bvm*)
 * @note Observability hook
 * @brief
 *
 * @param vm
 * @return
 */
BERRY_API bctypefunc be_get_ctype_func_hanlder(bvm *vm);

/**
 * @fn int be_loadbuffer(bvm*, const char*, const char*, size_t)
 * @note code load API
 * @brief
 *
 * @param vm
 * @param name
 * @param buffer
 * @param length
 * @return
 */
BERRY_API int be_loadbuffer(bvm *vm, const char *name, const char *buffer, size_t length);

/**
 * @fn int be_loadmode(bvm*, const char*, bool)
 * @note code load API
 * @brief
 *
 * @param vm
 * @param name
 * @param islocal
 * @return
 */
BERRY_API int be_loadmode(bvm *vm, const char *name, bbool islocal);

/**
 * @fn int be_loadlib(bvm*, const char*)
 * @note code load API
 * @brief
 *
 * @param vm
 * @param path
 * @return
 */
BERRY_API int be_loadlib(bvm *vm, const char *path);

/**
 * @fn int be_savecode(bvm*, const char*)
 * @note code load API
 * @brief
 *
 * @param vm
 * @param name
 * @return
 */
BERRY_API int be_savecode(bvm *vm, const char *name);

/**
 * @fn void be_module_path(bvm*)
 * @note module path list API
 * @brief
 *
 * @param vm
 */
BERRY_API void be_module_path(bvm *vm);

/**
 * @fn void be_module_path_set(bvm*, const char*)
 * @note module path list API
 * @brief
 *
 * @param vm
 * @param path
 */
BERRY_API void be_module_path_set(bvm *vm, const char *path);

/**
 * @fn void be_pushbytes*(bvm*, const void*, size_t)
 * @note bytes operation
 * @brief
 *
 * @param vm
 * @param buf
 * @param len
 */
BERRY_API void* be_pushbytes(bvm *vm, const void *buf, size_t len);

/**
 * @fn const void be_tobytes*(bvm*, int, size_t*)
 * @note bytes operation
 * @brief
 *
 * @param vm
 * @param index
 * @param len
 */
BERRY_API const void* be_tobytes(bvm *vm, int index, size_t *len);

/**
 * @fn void be_sethook(bvm*, const char*)
 * @note debug API
 * @brief
 *
 * @param vm
 * @param mask
 */
BERRY_API void be_sethook(bvm *vm, const char *mask);

/**
 * @fn void be_setntvhook(bvm*, bntvhook, void*, int)
 * @note debug API
 * @brief
 *
 * @param vm
 * @param hook
 * @param data
 * @param mask
 */
BERRY_API void be_setntvhook(bvm *vm, bntvhook hook, void *data, int mask);

/**
 * @fn void be_writebuffer(const char*, size_t)
 * @note basic character IO API
 * @brief
 *
 * @param buffer
 * @param length
 */
BERRY_API void be_writebuffer(const char *buffer, size_t length);

/**
 * @fn char be_readstring*(char*, size_t)
 * @note basic character IO API
 * @brief
 *
 * @param buffer
 * @param size
 * @return
 */
BERRY_API char* be_readstring(char *buffer, size_t size);

#ifdef __cplusplus
}
#endif

#endif
