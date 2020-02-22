#ifndef BE_CLASS_H
#define BE_CLASS_H

#include "be_object.h"

#define MT_VARIABLE                     BE_INT
#define MT_METHOD                       BE_CLOSURE
#define MT_PRIMMETHOD                   BE_NTVFUNC

#define be_class_name(cl)               ((cl)->name)
#define be_class_members(cl)            ((cl)->members)
#define be_class_super(cl)              ((cl)->super)
#define be_class_setsuper(self, sup)    ((self)->super = (sup))
#define be_instance_name(obj)           ((obj)->class->name)
#define be_instance_class(obj)          ((obj)->class)
#define be_instance_members(obj)        ((obj)->members)
#define be_instance_member_count(obj)   ((obj)->class->nvar)
#define be_instance_super(obj)          ((obj)->super)

struct bclass {
    bcommon_header;
    unsigned short nvar; /* members variable data field count */
    struct bclass *super;
    bmap *members;
    bstring *name;
    bgcobject *gray; /* for gc gray list */
};

struct binstance {
    bcommon_header;
    struct binstance *super;
    bclass *class;
    bgcobject *gray; /* for gc gray list */
    bvalue members[1]; /* members variable data field */
};

bclass* be_newclass(bvm *vm, bstring *name, bclass *super);
void be_class_compress(bvm *vm, bclass *c);
int be_class_attribute(bvm *vm, bclass *c, bstring *attr);
void be_member_bind(bvm *vm, bclass *c, bstring *name);
void be_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p);
void be_prim_method_bind(bvm *vm, bclass *c, bstring *name, bntvfunc f);
int be_class_closure_count(bclass *c);
void be_class_upvalue_init(bvm *vm, bclass *c);
int be_class_newobj(bvm *vm, bclass *c, bvalue *argv, int argc);
int be_instance_member(bvm *vm, binstance *obj, bstring *name, bvalue *dst);
int be_instance_setmember(bvm *vm, binstance *obj, bstring *name, bvalue *src);

#endif
