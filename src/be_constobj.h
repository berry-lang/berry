/**
 * @file       be_constobj.h
 * @brief      Main berry entry API
 * @details    This file is part of the Berry default interpreter.
 * @author     skiars@qq.com, https://github.com/Skiars/berry
 * @copyright  (c) 2018-2022 Guan Wenliang MIT License (https://github.com/Skiars/berry/blob/master/LICENSE)
 */

#ifndef BE_CONSTOBJ_H
#define BE_CONSTOBJ_H

#ifdef __cplusplus
extern "C" {
#endif

#include "be_object.h"
#include "be_gc.h"
#include "be_map.h"
#include "be_list.h"
#include "be_class.h"
#include "be_string.h"
#include "be_module.h"

#ifndef __cplusplus

/**
 * @def be_const_header
 * @brief
 *
 */
#define be_const_header(_t)                                     \
    .next = NULL,                                               \
    .type = (_t),                                               \
    .marked = GC_CONST

/**
 * @def be_define_const_str_weak
 * @brief be_define_const_str_weak
 *
 */
#define be_define_const_str_weak(_name, _s, _len)               \
    const bcstring_t be_const_str_##_name = {                   \
        .next = NULL,                                           \
        .type = BE_STRING,                                      \
        .marked = GC_CONST,                                     \
        .extra = 0,                                             \
        .slen = _len,                                           \
        .hash = 0,                                              \
        .s = _s                                                 \
    }

/**
 * @def be_const_key
 * @brief be_const_key
 *
 */
#define be_const_key(_str, _next) {                             \
    .v.c = &be_const_str_##_str,                                \
    .type = BE_STRING,                                          \
    .next = (uint32_t)(_next) & 0xFFFFFF                        \
}

/**
 * @def be_const_key_weak
 * @brief be_const_key_weak
 *
 * try to use the predefined string in strtab, but don't create an instance if none is present
 * the behavior is exactly the same as `be_const_key()` but it not detected by coc
 *
 */
#define be_const_key_weak(_str, _next) {                        \
    .v.c = &be_const_str_##_str,                                \
    .type = BE_STRING,                                          \
    .next = (uint32_t)(_next) & 0xFFFFFF                        \
}

/**
 * @def be_const_key_literal
 * @brief be_const_key_literal
 *
 */
#define be_const_key_literal(_str, _next) {                     \
    .v.c = be_str_literal(#_str),                               \
    .type = BE_STRING,                                          \
    .next = (uint32_t)(_next) & 0xFFFFFF                        \
}

/**
 * @def be_const_key_int
 * @brief be_const_key_int
 *
 */
#define be_const_key_int(_i, _next) {                           \
    .v.i = _i,                                                  \
    .type = BE_INT,                                             \
    .next = (uint32_t)(_next) & 0xFFFFFF                        \
}

/**
 * @def be_const_func
 * @brief be_const_func
 *
 */
#define be_const_func(_func) {                                  \
    .v.nf = (_func),                                            \
    .type = BE_NTVFUNC                                          \
}

/**
 * @def be_const_static_func
 * @brief be_const_static_func
 *
 */
#define be_const_static_func(_func) {                           \
    .v.nf = (_func),                                            \
    .type = BE_NTVFUNC | BE_STATIC                              \
}

/**
 * @def be_const_nil
 * @brief be_const_nil
 *
 */
#define be_const_nil() {                                        \
    .v.i = 0,                                                   \
    .type = BE_NIL                                              \
}

/**
 * @def be_const_int
 * @brief be_const_int
 *
 */
#define be_const_int(_val) {                                    \
    .v.i = (bint_t)(_val),                                      \
    .type = BE_INT                                              \
}

/**
 * @def be_const_var
 * @brief be_const_var
 *
 */
#define be_const_var(_val) {                                    \
    .v.i = (bint_t)(_val),                                      \
    .type = BE_INDEX                                            \
}

/**
 * @def be_const_real
 * @brief be_const_real
 *
 */
#define be_const_real(_val) {                                   \
    .v.r = (breal_t)(_val),                                     \
    .type = BE_REAL                                             \
}

/**
 * @def be_const_real_hex
 * @brief be_const_real_hex
 *
 */
#define be_const_real_hex(_val) {                               \
    .v.p = (void*)(_val),                                       \
    .type = BE_REAL                                             \
}

/**
 * @def be_const_bool
 * @brief be_const_bool
 *
 */
#define be_const_bool(_val) {                                   \
    .v.b = (bbool)(_val),                                       \
    .type = BE_BOOL                                             \
}

/**
 * @def be_const_str
 * @brief be_const_str
 *
 */
#define be_const_str(_str) {                                    \
    .v.s = (bstring_t*)(_str),                                  \
    .type = BE_STRING                                           \
}

/**
 * @def be_const_comptr
 * @brief be_const_comptr
 *
 */
#define be_const_comptr(_val) {                                 \
    .v.c = (const void*)(_val),                                 \
    .type = BE_COMPTR                                           \
}

/**
 * @def be_const_class
 * @brief be_const_class
 *
 */
#define be_const_class(_class) {                                \
    .v.c = &(_class),                                           \
    .type = BE_CLASS                                            \
}

/**
 * @def be_const_closure
 * @brief be_const_closure
 *
 */
#define be_const_closure(_closure) {                            \
    .v.c = &(_closure),                                         \
    .type = BE_CLOSURE                                          \
}

/**
 * @def be_const_static_closure
 * @brief be_const_static_closure
 *
 */
#define be_const_static_closure(_closure) {                     \
    .v.c = &(_closure),                                         \
    .type = BE_CLOSURE | BE_STATIC                              \
}

/**
 * @def be_const_module
 * @brief be_const_module
 *
 */
#define be_const_module(_module) {                              \
    .v.c = &(_module),                                          \
    .type = BE_MODULE                                           \
}

/**
 * @def be_const_simple_instance
 * @brief be_const_simple_instance
 *
 */
#define be_const_simple_instance(_instance) {                   \
    .v.c = (_instance),                                         \
    .type = BE_INSTANCE                                         \
}

/**
 * @def be_const_map
 * @brief be_const_map
 *
 */
#define be_const_map(_map) {                                    \
    .v.c = &(_map),                                             \
    .type = BE_MAP                                              \
}

/**
 * @def be_const_list
 * @brief be_const_list
 *
 */
#define be_const_list(_list) {                                  \
    .v.c = &(_list),                                            \
    .type = BE_LIST                                             \
}

/**
 * @def be_define_const_map_slots
 * @brief be_define_const_map_slots
 *
 */
#define be_define_const_map_slots(_name)                        \
const bmapnode_t _name##_slots[] =

/**
 * @def be_define_const_map
 * @brief be_define_const_map
 *
 */
#define be_define_const_map(_name, _size)                       \
const bmap_t _name = {                                          \
    be_const_header(BE_MAP),                                    \
    .slots = (bmapnode_t*)_name##_slots,                        \
    .lastfree = NULL,                                           \
    .size = _size,                                              \
    .count = _size                                              \
}

/**
 * @def be_define_const_class
 * @brief be_define_const_class
 *
 */
#define be_define_const_class(_name, _nvar, _super, _name_)     \
const bclass_t _name = {                                        \
    be_const_header(BE_CLASS),                                  \
    .nvar = _nvar,                                              \
    .super = _super,                                            \
    .members = (bmap_t*)&_name##_map,                           \
    .name = (bstring_t*)&be_const_str_##_name_                  \
}

/**
 * @def be_define_const_empty_class
 * @brief be_define_const_empty_class
 *
 */
#define be_define_const_empty_class(_name, _super, _name_)      \
const bclass _name = {                                          \
    be_const_header(BE_CLASS),                                  \
    .nvar = 0,                                                  \
    .super = (bclass*)_super,                                   \
    .members = NULL,                                            \
    .name = (bstring_t*)&be_const_str_##_name_                  \
}

/**
 * @def be_define_const_module
 * @brief be_define_const_module
 *
 */
#define be_define_const_module(_name, _name_)                   \
const bmodule_t _name = {                                       \
    be_const_header(BE_MODULE),                                 \
    .table = (bmap_t*)&_name##_map,                             \
    .info.name = _name_,                                        \
}

/**
 * @def be_define_const_vector
 * @brief be_define_const_vector
 *
 */
#define be_define_const_vector(_name, _data, _size)             \
const bvector_t _name = {                                       \
    .capacity = _size,                                          \
    .size = sizeof(bvalue_t),                                   \
    .count = _size,                                             \
    .data = (void*)_data,                                       \
    .end = (void*)(_data + (_size) - 1)                         \
}

/**
 * @def be_define_const_native_module
 * @brief be_define_const_native_module
 *
 */
#define be_define_const_native_module(_module)                  \
const bntvmodule_t be_native_module(_module) = {                \
    .name = #_module,                                           \
    .attrs = NULL,                                              \
    .size = 0,                                                  \
    .module = (bmodule_t*)&(m_lib##_module)                     \
}

/**
 * @def be_local_class
 * @brief defines needed for solidified classes
 *
 */
#define be_local_class(_name, _nvar, _super, _map, _cname)      \
  const bclass be_class_##_name = {                             \
    be_const_header(BE_CLASS),                                  \
    .nvar = _nvar,                                              \
    .super = (bclass*)_super,                                   \
    .members = (bmap*)_map,                                     \
    .name = _cname                                              \
}

/**
 * @def be_local_module
 * @brief defines needed for solidified modules
 *
 */
#define be_local_module(_c_name, _module_name, _map)            \
  static const bmodule_t m_lib##_c_name = {                     \
    be_const_header(BE_MODULE),                                 \
    .table = (bmap*)_map,                                       \
    .info.name = _module_name                                   \
}

/**
 * @def be_nested_simple_instance
 * @brief be_nested_simple_instance
 *
 * only instances with no super and no sub instance are supported
 * primarily for `list` and `map`
 *
 */
#define be_nested_simple_instance(_class_ptr, _members)         \
  & (const binstance)  {                                        \
    be_const_header(BE_INSTANCE),                               \
    .super = NULL,                                              \
    .sub = NULL,                                                \
    ._class = (bclass*) _class_ptr,                             \
    .members = _members                                         \
  }

/**
 * @def be_nested_map
 * @brief be_nested_map
 *
 */
#define be_nested_map(_size, _slots)                            \
  & (const bmap) {                                              \
    be_const_header(BE_MAP),                                    \
    .slots = _slots,                                            \
    .lastfree = NULL,                                           \
    .size = _size,                                              \
    .count = _size                                              \
  }

/**
 * @def be_nested_list
 * @brief be_nested_list
 *
 */
#define be_nested_list(_size, _items)                           \
  & (const blist) {                                             \
    be_const_header(BE_LIST),                                   \
    .count = _size,                                             \
    .capacity = _size,                                          \
    .data = _items                                              \
  }

/**
 * @def be_nested_str
 * @brief be_nested_str
 *
 */
#define be_nested_str(_name_)                                   \
  {                                                             \
    { .s=((bstring_t*)&be_const_str_##_name_) },                \
    BE_STRING                                                   \
  }

/**
 * @def be_nested_str_weak
 * @brief variant that does not trigger strtab
 *
 */
#define be_nested_str_weak(_name_)                              \
  {                                                             \
    { .s=((bstring_t*)&be_const_str_##_name_) },                \
    BE_STRING                                                   \
  }

/**
 * @def be_nested_str_literal
 * @brief be_nested_str_literal
 *
 */
#define be_nested_str_literal(_name_)                           \
  {                                                             \
    { .s=(be_nested_const_str(_name_, _hash, sizeof(_name_)-1 ))\
    },                                                          \
    BE_STRING                                                   \
  }

/**
 * @def be_str_literal
 * @brief be_str_literal
 *
 */
#define be_str_literal(_str)                                    \
  be_nested_const_str(_str, 0, sizeof(_str)-1 )

/**
 * @def be_str_weak
 * @brief be_str_weak
 *
 */
#define be_str_weak(_str)                                       \
  (bstring_t*) &be_const_str_##_str

/**
 * @def be_nested_string
 * @brief be_nested_string
 *
 */
#define be_nested_string(_str, _hash, _len)                     \
  {                                                             \
    { .s=(be_nested_const_str(_str, _hash, _len ))              \
    },                                                          \
    BE_STRING                                                   \
  }

/**
 * @def be_nested_key
 * @brief be_nested_key
 *
 */
#define be_nested_key(_str, _hash, _len, _next)                 \
  {                                                             \
    { .s=(be_nested_const_str(_str, _hash, _len )) },           \
    BE_STRING,                                                  \
    (uint32_t)(_next) & 0xFFFFFF                                \
  }

#else

/**
 * @def be_define_const_str_weak
 * @brief be_define_const_str_weak
 *
 */
#define be_define_const_str_weak(_name, _s, _len)               \
const bcstring_t be_const_str_##_name = {                       \
    NULL,                                                       \
    BE_STRING,                                                  \
    GC_CONST,                                                   \
    0,                                                          \
    _len,                                                       \
    0,                                                          \
    _s                                                          \
}

/**
 * @def be_const_key
 * @brief be_const_key
 *
 */
#define be_const_key(_str, _next) {                             \
    bvaldata(&be_const_str_##_str),                             \
        BE_STRING,                                              \
        uint32_t((_next)&0xFFFFFF)                              \
}

/**
 * @def be_const_key_weak
 * @brief be_const_key_weak
 *
 * try to use the predefined string in strtab, but don't create an instance if none is present
 * the behavior is exactly the same as `be_const_key()` but it not detected by coc
 *
 */
#define be_const_key_weak(_str, _next) {                        \
    bvaldata(&be_const_str_##_str),                             \
        BE_STRING,                                              \
        uint32_t((_next)&0xFFFFFF)                              \
}

/**
 * @def be_const_key_int
 * @brief be_const_key_int
 *
 */
#define be_const_key_int(_i, _next) {                           \
    bvaldata(i),                                                \
        BE_INT,                                                 \
        uint32_t((_next)&0xFFFFFF)                              \
}

/**
 * @def be_const_func
 * @brief be_const_func
 *
 */
#define be_const_func(_func) {                                  \
    bvaldata(_func),                                            \
    BE_NTVFUNC                                                  \
}

/**
 * @def be_const_static_func
 * @brief be_const_static_func
 *
 */
#define be_const_static_func(_func) {                           \
    bvaldata(_func),                                            \
    BE_NTVFUNC | BE_STATIC                                      \
}

/**
 * @def be_const_nil
 * @brief be_const_nil
 *
 */
#define be_const_nil() {                                        \
    bvaldata(0),                                                \
    BE_NIL                                                      \
}

/**
 * @def be_const_int
 * @brief be_const_int
 *
 */
#define be_const_int(_val) {                                    \
    bvaldata(bint(_val)),                                       \
    BE_INT                                                      \
}

/**
 * @def be_const_bool
 * @brief be_const_bool
 *
 */
#define be_const_bool(_val) {                                   \
    bvaldata(bbool(_val)),                                      \
    BE_BOOL                                                     \
}

/**
 * @def be_const_var
 * @brief be_const_var
 *
 */
#define be_const_var(_val) {                                    \
    bvaldata(bint(_val)),                                       \
    BE_INDEX                                                    \
}

/**
 * @def be_const_real
 * @brief be_const_real
 *
 */
#define be_const_real(_val) {                                   \
    bvaldata(breal(_val)),                                      \
    BE_REAL                                                     \
}

/**
 * @def be_const_real_hex
 * @brief be_const_real_hex
 *
 */
#define be_const_real_hex(_val) {                               \
    bvaldata((void*)(_val)),                                    \
    BE_REAL                                                     \
}

/**
 * @def be_const_comptr
 * @brief be_const_comptr
 *
 */
#define be_const_comptr(_val) {                                 \
    bvaldata((void*)(_val)),                                    \
    BE_COMPTR                                                   \
}

/**
 * @def be_const_str
 * @brief be_const_str
 *
 */
#define be_const_str(_string) {                                 \
    bvaldata(bstring_t(_string)),                               \
    BE_STRING                                                   \
}

/**
 * @def be_const_class
 * @brief be_const_class
 *
 */
#define be_const_class(_class) {                                \
    bvaldata(&(_class)),                                        \
    BE_CLASS                                                    \
}

/**
 * @def be_const_closure
 * @brief be_const_closure
 *
 */
#define be_const_closure(_closure) {                            \
    bvaldata(&(_closure)),                                      \
    BE_CLOSURE                                                  \
}

/**
 * @def be_const_static_closure
 * @brief be_const_static_closure
 *
 */
#define be_const_static_closure(_closure) {                     \
    bvaldata(&(_closure)),                                      \
    BE_CLOSURE | BE_STATIC                                      \
}

/**
 * @def be_const_module
 * @brief be_const_module
 *
 */
#define be_const_module(_module) {                              \
    bvaldata(&(_module)),                                       \
    BE_MODULE                                                   \
}

/**
 * @def be_define_const_map_slots
 * @brief be_define_const_map_slots
 *
 */
#define be_define_const_map_slots(_name)                        \
const bmapnode_t _name##_slots[] =

/**
 * @def be_define_const_map
 * @brief be_define_const_map
 *
 */
#define be_define_const_map(_name, _size)                       \
const bmap _name(                                               \
    (bmapnode_t*)_name##_slots, _size                           \
)

/**
 * @def be_define_const_class
 * @brief be_define_const_class
 *
 */
#define be_define_const_class(_name, _nvar, _super, _name_)     \
const bclass_t _name(                                           \
    _nvar, (bclass_t*)_super, (bmap_t*)&_name##_map,            \
    (bstring_t*)&be_const_str_##_name_                          \
)

/**
 * @def be_define_const_empty_class
 * @brief be_define_const_empty_class
 *
 */
#define be_define_const_empty_class(_name, _super, _name_)      \
const bclass_t _name(                                           \
    0, (bclass_t*)_super, NULL,                                 \
    (bstring_t*)&be_const_str_##_name_                          \
)

/**
 * @def be_define_const_module
 * @brief be_define_const_module
 *
 */
#define be_define_const_module(_name, _name_)                   \
const bmodule_t _name((bmap_t*)&_name##_map, _name_)

/**
 * @def be_define_const_vector
 * @brief be_define_const_vector
 *
 */
#define be_define_const_vector(_name, _data, _size)             \
const bvector _name = {                                         \
    _size, sizeof(bvalue_t), _size,                             \
    (void*)_data, (void*)(_data + (_size) - 1)                  \
}

/**
 * @def be_define_const_native_module
 * @brief be_define_const_native_module
 *
 */
#define be_define_const_native_module(_module)                  \
const bntvmodule_t be_native_module_##_module = {               \
    #_module,                                                   \
    0, 0,                                                       \
    (bmodule_t*)&(m_lib##_module)                               \
}

#endif

/* provide pointers to map and list classes for solidified code */
extern const bclass_t be_class_list; /**< be_class_list */
extern const bclass_t be_class_map;  /**< be_class_map */

#ifdef __cplusplus
}
#endif

#endif
