#ifndef __BE_API_H
#define __BE_API_H

#include "be_object.h"

#define be_type(v)                  var_type(v)
#define be_settype(v, t)            var_settype(v, t)
#define be_checktype(v, t)          var_istype(v, t)
#define be_isnil(v)                 var_isnil(v)
#define be_isclass(v)               var_isclass(v)
#define be_isinstance(v)            var_isinstance(v)

#define be_api_getint(vm, n)        be_api_getvalue(vm, n)->v.i
#define be_api_getreal(vm, n)       be_api_getvalue(vm, n)->v.r
#define be_api_getbool(vm, n)       be_api_getvalue(vm, n)->v.b
#define be_api_getptr(vm, n)        be_api_getvalue(vm, n)->v.p

#define be_api_topreg(vm)           ((vm)->cf->s.ur.top)

#define be_api_retnil(vm)           var_setnil(be_api_retreg(vm))
#define be_api_retint(vm, i)        var_setint(be_api_retreg(vm), i)
#define be_api_retreal(vm, r)       var_setreal(be_api_retreg(vm),r)
#define be_api_retbool(vm, b)       var_setbool(be_api_retreg(vm), b)
#define be_api_retstr(vm, s)        var_setstr(be_api_retreg(vm), s)
#define be_api_retclass(vm, c)      var_setclass(be_api_retreg(vm), c)
#define be_api_retinstance(vm, o)   var_setinstance(be_api_retreg(vm), o)
#define be_api_retfunc(vm, f)       var_setntvfunc(be_api_retreg(vm), f)

void be_api_regcfunc(bvm *vm, const char *name, bcfunction f, int argc);
void be_reg_class(bvm *vm, const char *name, bclass *c);
bvalue* be_api_getvalue(bvm *vm, int n);
bvalue* be_api_retreg(bvm *vm);
void be_api_retvalue(bvm *vm, bvalue *value);

#endif
