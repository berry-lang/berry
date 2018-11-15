#ifndef __BE_API_H
#define __BE_API_H

#include "be_object.h"

#define be_settype(v, t)        var_settype(v, t)
#define be_checktype(v, t)      var_istype(v, t)
#define be_isnil(v)             var_isnil(v)
#define be_isclass(v)           var_isclass(v)
#define be_isinstance(v)        var_isinstance(v)

#define be_getint(vm, n)        be_getvalue(vm, n)->v.i
#define be_getreal(vm, n)       be_getvalue(vm, n)->v.r
#define be_getbool(vm, n)       be_getvalue(vm, n)->v.b
#define be_getptr(vm, n)        be_getvalue(vm, n)->v.p

#define be_api_topreg(vm)       ((vm)->cf->s.ur.top)

#define be_retnil(vm)           var_setnil(be_retreg(vm))
#define be_retint(vm, i)        var_setint(be_retreg(vm), i)
#define be_retreal(vm, r)       var_setreal(be_retreg(vm),r)
#define be_retbool(vm, b)       var_setbool(be_retreg(vm), b)
#define be_retstr(vm, s)        var_setstr(be_retreg(vm), s)
#define be_retclass(vm, c)      var_setclass(be_retreg(vm), c)
#define be_retinstance(vm, o)   var_setinstance(be_retreg(vm), o)
#define be_retfunc(vm, f)       var_setntvfunc(be_retreg(vm), f)

void be_regclass(bvm *vm, const char *name, bclass *c);
bvalue* be_getvalue(bvm *vm, int n);
bvalue* be_retreg(bvm *vm);
void be_retvalue(bvm *vm, bvalue *value);

#endif
