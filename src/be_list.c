#include "be_list.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_exec.h"
#include <string.h>

#define datasize(size)          ((size) * sizeof(bvalue))

blist* be_list_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_LIST, blist);
    blist *list = cast_list(gco);
    if (list) {
        list->count = 0;
        list->capacity = 2;
        var_setlist(vm->top, list);
        be_incrtop(vm);
        list->data = be_malloc(vm, datasize(list->capacity));
        be_stackpop(vm, 1);
    }
    return list;
}

void be_list_delete(bvm *vm, blist *list)
{
    be_free(vm, list->data, datasize(list->capacity));
    be_free(vm, list, sizeof(blist));
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
        int newcap = be_nextsize(list->capacity);
        list->data = be_realloc(vm, list->data,
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
        int newcap = be_nextsize(list->capacity);
        list->data = be_realloc(vm, list->data,
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
        if (newcap > list->capacity) {
            bvalue *v, *end;
            list->data = be_realloc(vm, list->data,
                datasize(list->capacity), datasize(newcap));
            list->capacity = newcap;
            v = list->data + list->count;
            end = list->data + count;
            while (v < end) {
                var_setnil(v++);
            }
        }
        list->count = count;
    }
}

void be_list_merge(bvm *vm, blist *list, const blist *other)
{
    int dst_len = list->count;
    int src_len = other->count;
    int length = src_len + dst_len;
    if (length != 0) {
        int newcap = be_nextsize(length);
        if (newcap > list->capacity) {
            list->data = be_realloc(vm, list->data,
                datasize(list->capacity), datasize(newcap));
            list->capacity = newcap;
        }
        memcpy(list->data + dst_len, other->data, src_len * sizeof(bvalue));
        list->count = length;
    }
}
