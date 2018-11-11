#include "be_var.h"
#include "be_vm.h"
#include "be_vector.h"
#include "be_string.h"
#include "be_map.h"
#include "be_gc.h"

void be_globalvar_init(bvm *vm)
{
    vm->gbldesc.idxtab = be_map_new(vm);
    vm->gbldesc.gvalist = be_vector_new(sizeof(bvalue));
    vm->gbldesc.nglobal = 0;
    be_gc_addgray(vm, gc_object(vm->gbldesc.idxtab));
}

int be_globalvar_find(bvm *vm, bstring *name)
{
    bvalue *res = be_map_findstr(vm->gbldesc.idxtab, name);
    if (res) {
        return res->v.i;
    }
    return -1; /* not found */
}

int be_globalvar_new(bvm *vm, bstring *name)
{
    bglobaldesc *gd = &vm->gbldesc;
    int idx = be_globalvar_find(vm, name);
    if (idx == -1) {
        bvalue *desc;
        idx = be_map_count(gd->idxtab);
        desc = be_map_insertstr(gd->idxtab, name, NULL);
        desc->v.i = idx;
        be_vector_resize(gd->gvalist, idx + 1);
        vm->global = be_vector_data(gd->gvalist);
        gd->nglobal++;
    }
    return idx;
}
