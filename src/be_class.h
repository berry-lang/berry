#ifndef BE_CLASS_H
#define BE_CLASS_H

#include "be_object.h"

#define MT_VARIABLE                     BE_INT
#define MT_METHOD                       BE_CLOSURE
#define MT_PRIMMETHOD                   BE_NTVFUNC

#define be_class_name(cl)               ((cl)->name)
#define be_class_members(cl)           ((cl)->members)
#define be_class_super(cl)              ((cl)->super)
#define be_class_setsuper(self, sup)    ((self)->super = (sup))
#define be_instance_name(obj)           ((obj)->class->name)
#define be_instance_members(obj)        ((obj)->members)
#define be_instance_member_count(obj)   ((obj)->class->nvar)
#define be_instance_super(obj)          ((obj)->super)

struct bclass {
    bcommon_header;
    unsigned short nvar; /* members variable count */
    struct bclass *super;
    bmap *members;
    bstring *name;
};

struct binstance {
    bcommon_header;
    struct binstance *super;
    bclass *class;
    bvalue members[1]; /* members table */
};

bclass* be_newclass(bvm *vm, bstring *name, bclass *super);
void be_member_bind(bvm *vm, bclass *c, bstring *name);
void be_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p);
void be_prim_method_bind(bvm *vm, bclass *c, bstring *name, bntvfunc f);
int be_class_newobj(bvm *vm, bclass *c, bvalue *argv, int argc);
int be_instance_member(binstance *obj, bstring *name, bvalue *dst);
int be_instance_setmember(binstance *obj, bstring *name, bvalue *src);

#endif
