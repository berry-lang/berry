#include "be_object.h"

#define exception           be_class_exception

/* @const_object_info_begin
class be_class_exception
    (scope: global, name: exception) {}
class be_class_stop_iterator
    (scope: global, name: stop_iterator, super: exception) {}
@const_object_info_end */
#include "../generate/be_fixed_be_class_exception.h"
#include "../generate/be_fixed_be_class_stop_iterator.h"
