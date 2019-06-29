#include "be_list.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_vector.h"

#define datasize(size)          ((size) * sizeof(bvalue))

blist* be_list_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_LIST, blist);
    blist *list = cast_list(gco);
    if (list) {
        list->count = 0;
        list->capacity = 2;
        list->data = be_gc_malloc(vm, datasize(list->capacity));
    }
    return list;
}

void be_list_delete(bvm *vm, blist *list)
{
    be_gc_free(vm, list->data, datasize(list->capacity));
    be_gc_free(vm, list, sizeof(blist));
}

bvalue* be_list_index(blist *list, int index)
{
    if (index < 0) {
        index = list->count + index;
    }
    if (index < 0 || index >= list->count) {
        return NULL;
    }
    return be_list_at(list, index);
}

bvalue* be_list_append(bvm *vm, blist *list, bvalue *value)
{
    bvalue *slot;
    if (list->count >= list->capacity) {
        size_t newcap = be_nextsize(list->capacity);
        list->data = be_gc_realloc(vm, list->data,
            datasize(list->capacity), datasize(newcap));
        list->capacity = newcap;
    }
    slot = list->data + list->count++;
    if (value != NULL) {
        *slot = *value;
    }
    return slot;
}

bvalue* be_list_insert(bvm *vm, blist *list, int index, bvalue *value)
{
    int i;
    bvalue *data;
    if (index < 0) {
        index = list->count + index;
    }
    if (index < 0 || index > list->count) {
        return NULL;
    }
    if (list->count >= list->capacity) {
        size_t newcap = be_nextsize(list->capacity);
        list->data = be_gc_realloc(vm, list->data,
            datasize(list->capacity), datasize(newcap));
        list->capacity = newcap;
    }
    data = list->data;
    for (i = list->count++; i > index; --i) {
        data[i] = data[i - 1];
    }
    data = list->data + index;
    if (value != NULL) {
        *data = *value;
    }
    return data;
}

int be_list_remove(bvm *vm, blist *list, int index)
{
    int i;
    bvalue *data;
    (void)vm;
    if (index < 0) {
        index = list->count + index;
    }
    if (index < 0 || index >= list->count) {
        return bfalse;
    }
    data = list->data;
    for (i = index; i < list->count; ++i) {
        data[i] = data[i + 1];
    }
    --list->count;
    return btrue;
}

void be_list_resize(bvm *vm, blist *list, int count)
{
    if (count != list->count) {
        int newcap = be_nextsize(count);
        int oldcount = list->count;
        list->count = count;
        if (newcap > list->capacity) {
            bvalue *v, *end;
            list->capacity = newcap;
            list->data = be_gc_realloc(vm, list->data,
                datasize(list->capacity), datasize(newcap));
            list->capacity = newcap;
            v = list->data + oldcount;
            end = list->data + list->count;
            while (v < end) {
                var_setnil(v++);
            }
        }
    }
}
