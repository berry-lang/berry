#ifndef __BE_LIST_H
#define __BE_LIST_H

#include "be_object.h"

struct blist {
    bcommon_header;
    int count, capacity;
    bvalue *data;
};

#define be_list_data(list)          ((list)->data)
#define be_list_count(list)         ((list)->count)
#define be_list_at(list, index)     ((list)->data + index)
#define be_list_end(list)           ((list)->data + (list)->count)

blist* be_list_new(bvm *vm);
void be_list_delete(blist *list);
bvalue* be_list_index(blist *list, int index);
bvalue* be_list_append(blist *list, bvalue *value);
bvalue* be_list_insert(blist *list, int index, bvalue *value);
int be_list_remove(blist *list, int index);
void be_list_resize(blist *list, int count);

#endif
