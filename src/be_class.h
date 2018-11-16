#ifndef __BE_CLASS_H
#define __BE_CLASS_H

#include "be_object.h"

#define MT_VARIABLE                     BE_INT
#define MT_METHOD                       BE_CLOSURE
#define MT_PRIMMETHOD                   BE_NTVFUNC

#define be_class_name(cl)               ((cl)->name)
#define be_class_super(cl)              ((cl)->super)
#define be_class_setsuper(self, sup)    ((self)->super = (sup))
#define be_instance_name(obj)           ((obj)->class->name)
#define be_instance_fields(obj)         ((obj)->fields)
#define be_instance_member_count(obj)   ((obj)->class->nvar)
#define be_instance_super(obj)          ((obj)->super)

struct bclass {
    bcommon_header;
    unsigned short nvar; /* fields variable count */
    struct bclass *super;
    bmap *fields;
    bstring *name;
};

struct binstance {
    bcommon_header;
    struct binstance *super;
    bclass *class;
    bvalue fields[1]; /* fields table */
};

bclass* be_newclass(bvm *vm, bstring *name, bclass *super);
void be_field_bind(bclass *c, bstring *name);
void be_method_bind(bvm *vm, bclass *c, bstring *name, bproto *p);
void be_prim_method_bind(bvm *vm, bclass *c, bstring *name, bcfunction f, int argc);
bvalue* be_class_field(bclass *c, bstring *name);
int be_class_newobj(bvm *vm, bclass *c, bvalue *argv, int argc);
bvalue* be_instance_field(binstance *obj, bstring *name, bvalue *dst);
int be_instance_setfield(binstance *obj, bstring *name, bvalue *src);

#endif
