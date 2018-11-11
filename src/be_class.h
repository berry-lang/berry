#ifndef __BE_CLASS_H
#define __BE_CLASS_H

#include "be_object.h"

#define MT_VARIABLE                     VT_INT
#define MT_METHOD                       VT_CLOSURE
#define MT_PRIMMETHOD                   VT_PRIMFUNC

#define be_class_super(cl)              ((cl)->super)
#define be_class_setsuper(self, sup)    ((self)->super = (sup))
#define be_object_members(obj)          ((obj)->members)
#define be_object_member_count(obj)     ((obj)->class->nvar)
#define be_object_super(obj)            ((obj)->super)

struct bclass {
    bcommon_header;
    unsigned short nvar; /* member variable count */
    struct bclass *super;
    bmap *members;
    bstring *name;
};

struct bobject {
    bcommon_header;
    struct bobject *super;
    bclass *class;
    bvalue members[1]; /* members table */
};

bclass* be_newclass(bvm *vm, bstring *name, bclass *super);
void be_member_bind(bvm *vm, bclass *c, bstring *name);
void be_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p);
void be_prim_method_bind(bvm *vm, bclass *c, const char *name, bcfunction f, int argc);
bvalue* be_class_member(bclass *c, bstring *name);
int be_class_newobj(bvm *vm, bclass *c, bvalue *argv, int argc);
bvalue* be_object_member(bobject *obj, bstring *name, bvalue *dst);
int be_object_setmember(bobject *obj, bstring *name, bvalue *src);

#endif
