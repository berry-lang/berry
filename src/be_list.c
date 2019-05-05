#include "be_list.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_vector.h"

blist* be_list_new(bvm *vm)
{
    bgcobject *gco = be_gcnew(vm, BE_LIST, blist);
    blist *list = cast_list(gco);
    if (list) {
        list->count = 0;
        list->capacity = 2;
        list->data = be_malloc(sizeof(bvalue) * list->capacity);
    }
    return list;
}

void be_list_delete(blist *list)
{
    be_free(list->data);
    be_free(list);
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

bvalue* be_list_append(blist *list, bvalue *value)
{
    bvalue *slot;
    if (list->count >= list->capacity) {
        list->capacity = be_nextsize(list->capacity);
        list->data = be_realloc(list->data, list->capacity * sizeof(bvalue));
    }
    slot = list->data + list->count++;
    if (value != NULL) {
        *slot = *value;
    }
    return slot;
}

bvalue* be_list_insert(blist *list, int index, bvalue *value)
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
        list->capacity <<= 1; /* capacity *= 2 */
        list->data = be_realloc(list->data, list->capacity * sizeof(bvalue));
    }
    data = list->data;
    for (i = ++list->count; i > index; --i) {
        data[i] = data[i - 1];
    }
    data = list->data + index;
    if (value != NULL) {
        *data = *value;
    }
    return data;
}

int be_list_remove(blist *list, int index)
{
    int i;
    bvalue *data;
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

void be_list_resize(blist *list, int count)
{
    if (count != list->count) {
        int newcap = be_nextsize(count);
        int oldcount = list->count;
        list->count = count;
        if (newcap > list->capacity) {
            bvalue *v, *end;
            list->capacity = newcap;
            list->data = be_realloc(list->data, newcap * sizeof(bvalue));
            v = list->data + oldcount;
            end = list->data + list->count;
            while (v < end) {
                var_setnil(v++);
            }
        }
    }
}
