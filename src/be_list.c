/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "be_list.h"
#include "be_mem.h"
#include "be_gc.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_exec.h"
#include <string.h>

#define datasize(size)          ((size) * sizeof(bvalue_t))

blist_t* be_list_new(bvm_t *vm)
{
    bgcobject_t *gco = be_gcnew(vm, BE_LIST, blist_t);
    blist_t *list = cast_list(gco);
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

void be_list_delete(bvm_t *vm, blist_t *list)
{
    be_free(vm, list->data, datasize(list->capacity));
    be_free(vm, list, sizeof(blist_t));
}

blist_t* be_list_copy(bvm_t *vm, blist_t *original)
{
    bgcobject_t *gco = be_gcnew(vm, BE_LIST, blist_t);
    blist_t *list = cast_list(gco);
    if (list) {
        size_t size = datasize(original->capacity);
        list->count = original->count;
        list->capacity = original->capacity;
        var_setlist(vm->top, list);
        be_incrtop(vm);
        list->data = be_malloc(vm, size);
        be_stackpop(vm, 1);
        memcpy(list->data, original->data, size);
    }
    return list;
}

bvalue_t* be_list_index(blist_t *list, int index)
{
    if (index < 0) {
        index = list->count + index;
    }
    if (index < 0 || index >= list->count) {
        return NULL;
    }
    return be_list_at(list, index);
}

bvalue_t* be_list_push(bvm_t *vm, blist_t *list, bvalue_t *value)
{
    bvalue_t *slot;
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

bvalue_t* be_list_insert(bvm_t *vm, blist_t *list, int index, bvalue_t *value)
{
    int i;
    bvalue_t *data;
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

int be_list_remove(bvm_t *vm, blist_t *list, int index)
{
    int i;
    bvalue_t *data;
    (void)vm;
    if (index < 0) {
        index = list->count + index;
    }
    if (index < 0 || index >= list->count) {
        return bfalse;
    }
    data = list->data;
    list->count--;
    for (i = index; i < list->count; ++i) {
        data[i] = data[i + 1];
    }
    return btrue;
}

void be_list_resize(bvm_t *vm, blist_t *list, int count)
{
    if (count != list->count) {
        int newcap = be_nextsize(count);
        if (newcap > list->capacity) {
            bvalue_t *v, *end;
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

void be_list_merge(bvm_t *vm, blist_t *list, const blist_t *other)
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
        memcpy(list->data + dst_len, other->data, src_len * sizeof(bvalue_t));
        list->count = length;
    }
}

void be_list_reverse(blist_t *list)
{
    bvalue_t *left = list->data;
    bvalue_t *right = left + list->count - 1;
    for (; left < right; ++left, --right) {
        bvalue_t temp = *left;
        *left = *right;
        *right = temp;
    }
}

void be_list_pool_init(bvm_t *vm, blist_t *list)
{
    bvalue_t *head;
    be_list_resize(vm, list, 0);
    head = be_list_push(vm, list, NULL);
    var_setint(head, 0);
}

int be_list_pool_alloc(bvm_t *vm, blist_t *list, bvalue_t *src)
{
    bvalue_t *head = be_list_data(list), *node;
    int id = var_toidx(head); /* get the first free node */
    if (id) {
        node = head + id;
        head->v.i = var_toint(node); /* link the next free node to head */
    } else {
        id = be_list_count(list);
        node = be_list_push(vm, list, NULL);
    }
    *node = *src;
    return id;
}

void be_list_pool_free(blist_t *list, int id)
{
    bvalue_t *head = be_list_data(list);
    bvalue_t *node = head + id;
    be_assert(id > 0 && id < list->count);
    /* insert a new free node to head */
    *node = *head;
    head->v.i = id;
}
