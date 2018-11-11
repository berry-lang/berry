#ifndef __BE_MAP_H
#define __BE_MAP_H

#include "be_object.h"

typedef struct bmapentry {
    bvalue key;
    bvalue value;
    struct bmapentry *prev;
    struct bmapentry *next;
} bmapentry;

struct bmap {
    bcommon_header;
    bmapentry *freelist;
    bmapentry *slots;
    int size;
    int count;
};

#define be_map_count(map)   ((map)->count)
#define be_map_size(map)    (map->size)
#define be_map_slots(map)   (map->slots)

bmap* be_map_new(bvm *vm);
void be_map_delete(bmap *map);
bvalue* be_map_find(bmap *map, bvalue *key);
bvalue* be_map_insert(bmap *map, bvalue *key, bvalue *value);
void be_map_remove(bmap *map, bvalue *key);
bvalue* be_map_findstr(bmap *map, bstring *key);
bvalue* be_map_insertstr(bmap *map, bstring *key, bvalue *value);
void be_map_removestr(bmap *map, bstring *key);

#endif
