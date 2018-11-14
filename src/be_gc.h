#ifndef __BE_GC_H
#define __BE_GC_H

#include "be_object.h"

#define gc_object(o)        ((bgcobject*)o)
#define gc_cast(o, t, T)    ((o)->type == (t) ? (T*)(o) : NULL)
#define cast_proto(o)       gc_cast(o, VT_PROTO, bproto)
#define cast_closure(o)     gc_cast(o, VT_CLOSURE, bclosure)
#define cast_ntvfunc(o)    gc_cast(o, VT_NTVFUNC, bntvfunc)
#define cast_str(o)         gc_cast(o, VT_STRING, bstring)
#define cast_class(o)       gc_cast(o, VT_CLASS, bclass)
#define cast_object(o)      gc_cast(o, VT_INSTANCE, bobject)
#define cast_map(o)         gc_cast(o, VT_MAP, bmap)
#define cast_list(o)        gc_cast(o, VT_LIST, blist)

#define gc_iswhite(o)       ((o)->marked == GC_WHITE)
#define gc_isgray(o)        ((o)->marked == GC_GRAY)
#define gc_isdark(o)        ((o)->marked == GC_BLACK)
#define gc_setwhite(o)      ((o)->marked = GC_WHITE)
#define gc_setgray(o)       ((o)->marked = GC_GRAY)
#define gc_setdark(o)       ((o)->marked = GC_BLACK)

#define be_isgctype(t)      (t >= VT_FUNCTION)
#define be_isgcobj(o)       be_isgctype(var_type(o))
#define be_gcnew(v, t, s)   be_newgcobj((v), (t), sizeof(s))

enum {
    GC_WHITE, /* unreachable object */
    GC_GRAY,  /* unscanned object */
    GC_BLACK  /* scanned object */
} bgcmark;

void be_gc_init(bvm *vm);
void be_gc_setsteprate(bvm *vm, int rate);
void be_gc_setpause(bvm *vm, int pause);
bgcobject* be_newgcobj(bvm *vm, int type, int size);
void be_gc_fix(bvm *vm, bgcobject *obj);
void be_gc_removegray(bvm *vm, bgcobject *obj);
void be_gc_collect(bvm *vm);
void be_gc_auto(bvm *vm);

#endif
