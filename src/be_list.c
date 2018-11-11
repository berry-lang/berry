#include "be_list.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"

#define DEF_LIST_CAP    4

blist* be_list_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, VT_LIST, blist);
    blist *list = cast_list(gco);
    if (list) {
        list->count = 0;
        list->capacity = DEF_LIST_CAP;
        list->data = be_malloc(sizeof(bvalue) * list->capacity);
    }
    return list;
}

void be_list_delete(blist *list)
{
    be_free(list->data);
    be_free(list);
}

bvalue* be_list_append(blist *list, bvalue *value)
{
    bvalue *slot;
    if (list->count >= list->capacity) {
        list->capacity <<= 1; /* capacity *= 2 */
        list->data = be_realloc(list->data, list->capacity * sizeof(bvalue));
    }
    slot = list->data + list->count++;
    if (value != NULL) {
        *slot = *value;
    }
    return slot;
}
