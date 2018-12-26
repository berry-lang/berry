#ifndef __BE_GC_H
#define __BE_GC_H

#include "be_object.h"

#define BE_GCOBJECT         BE_STRING

#define gc_object(o)        cast(bgcobject*, o)
#define gc_cast(o, t, T)    ((o) && (o)->type == (t) ? (T*)(o) : NULL)
#define cast_proto(o)       gc_cast(o, BE_PROTO, bproto)
#define cast_closure(o)     gc_cast(o, BE_CLOSURE, bclosure)
#define cast_ntvclos(o)     gc_cast(o, BE_NTVCLOS, bntvclos)
#define cast_str(o)         gc_cast(o, BE_STRING, bstring)
#define cast_class(o)       gc_cast(o, BE_CLASS, bclass)
#define cast_instance(o)    gc_cast(o, BE_INSTANCE, binstance)
#define cast_map(o)         gc_cast(o, BE_MAP, bmap)
#define cast_list(o)        gc_cast(o, BE_LIST, blist)

#define gc_ismark(o, m)     (((o)->marked & 0x03) == m)
#define gc_iswhite(o)       gc_ismark((o), GC_WHITE)
#define gc_isgray(o)        gc_ismark((o), GC_GRAY)
#define gc_isdark(o)        gc_ismark((o), GC_BLACK)
#define gc_setmark(o, m)    { (o)->marked &= ~0x03; (o)->marked |= (m) & 0x03; }
#define gc_setwhite(o)      gc_setmark((o), GC_WHITE)
#define gc_setgray(o)       gc_setmark((o), GC_GRAY)
#define gc_setdark(o)       gc_setmark((o), GC_BLACK)
#define gc_isfixed(o)       (((o)->marked & 0x04) != 0)
#define gc_setfixed(o)      ((o)->marked |= 0x04)
#define gc_clearfixed(o)    ((o)->marked &= ~0x04)

#define be_isgctype(t)      (t >= BE_GCOBJECT)
#define be_isgcobj(o)       be_isgctype(var_type(o))
#define be_gcnew(v, t, s)   be_newgcobj((v), (t), sizeof(s))

enum {
    GC_WHITE, /* unreachable object */
    GC_GRAY,  /* unscanned object */
    GC_BLACK  /* scanned object */
} bgcmark;

void be_gc_init(bvm *vm);
void be_gc_deleteall(bvm *vm);
void be_gc_setsteprate(bvm *vm, int rate);
void be_gc_setpause(bvm *vm, int pause);
bgcobject* be_newgcobj(bvm *vm, int type, int size);
bgcobject* be_gc_newstr(bvm *vm, int size, int islong);
void be_gc_fix(bvm *vm, bgcobject *obj);
void be_gc_unfix(bvm *vm, bgcobject *obj);
void be_gc_collect(bvm *vm);
void be_gc_auto(bvm *vm);

#endif
