#ifndef __BE_LIST_H
#define __BE_LIST_H

#include "be_object.h"

typedef struct blist {
    bcommon_header;
    int count, capacity;
    bvalue *data;
} blist;

#define be_list_data(list)          ((list)->data)
#define be_list_count(list)         ((list)->count)
#define be_list_at(list, index)     ((list)->data + index)

blist* be_list_new(bvm *vm);
void be_list_delete(blist *list);
bvalue* be_list_append(blist *list, bvalue *value);

#endif
