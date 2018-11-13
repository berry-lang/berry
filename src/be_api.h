#ifndef __BE_API_H
#define __BE_API_H

#include "be_object.h"

#define be_api_regtop(vm)       ((vm)->cf->u.top)

#define be_type(v)              ((v)->type)
#define be_settype(v, t)        ((v)->type = (t))
#define be_checktype(v, t)      ((v)->type == (t))
#define be_isnil(v)             (be_checktype(v, VT_NIL))
#define be_isclass(v)           be_checktype((v), VT_CLASS)
#define be_isinstance(v)        be_checktype((v), VT_INSTANCE)

#define be_api_getint(vm, n)    be_api_getvalue(vm, n)->v.i
#define be_api_getreal(vm, n)   be_api_getvalue(vm, n)->v.r
#define be_api_getbool(vm, n)   be_api_getvalue(vm, n)->v.b
#define be_api_getptr(vm, n)    be_api_getvalue(vm, n)->v.p

void be_api_regcfunc(bvm *vm, const char *name, bcfunction f, int argc);
void be_reg_class(bvm *vm, const char *name, bclass *c);
bvalue* be_api_getvalue(bvm *vm, int n);
void be_api_retvalue(bvm *vm, bvalue *value);
void be_api_retnil(bvm *vm);
void be_api_retint(bvm *vm, bint i);
void be_api_retreal(bvm *vm, breal r);
void be_api_retbool(bvm *vm, bbool b);
void be_api_retstr(bvm *vm, bstring *str);
void be_api_retclass(bvm *vm, bclass *c);
void be_api_retinstance(bvm *vm, bobject *i);

#endif
