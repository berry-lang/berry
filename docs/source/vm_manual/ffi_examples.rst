Making a Native Function
========================

Berry’s C FFI (Foreign Function Interface) operates on a virtual stack
to interact with the VM. If we need to make an ``add`` function to add
two numbers and use it in Berry in this way:

.. code:: ruby

   result = add(1, 2)

We need to know how the C code gets the arguments from the Berry
function call and how to return the value.

The function arguments are stored in a stack, and the first argument to
the last argument of the function is stored from the bottom of the stack
to the top of the stack. If you want to use C to fetch elements from the
stack, use the following set of FFIs:

.. code:: c

   int be_toint(bvm *vm, int index);
   breal be_toreal(bvm *vm, int index);
   int be_tobool(bvm *vm, int index);
   const char* be_tostring(bvm *vm, int index);
   void* be_tocomptr(bvm *vm, int index);

If you want to test if a value in the stack is a specified type, use the
following set of FFIs:

.. code:: c

   int be_isnil(bvm *vm, int index);
   int be_isbool(bvm *vm, int index);
   int be_isint(bvm *vm, int index);
   int be_isreal(bvm *vm, int index);
   int be_isnumber(bvm *vm, int index);
   int be_isstring(bvm *vm, int index);
   int be_isclosure(bvm *vm, int index);
   int be_isntvclos(bvm *vm, int index);
   int be_isfunction(bvm *vm, int index);
   int be_isproto(bvm *vm, int index);
   int be_isclass(bvm *vm, int index);
   int be_isinstance(bvm *vm, int index);
   int be_islist(bvm *vm, int index);
   int be_ismap(bvm *vm, int index);
   int be_iscomptr(bvm *vm, int index);

If you need to push values onto the stack, use these FFIs:

.. code:: c

   void be_pushnil(bvm *vm);
   void be_pushbool(bvm *vm, int b);
   void be_pushint(bvm *vm, bint i);
   void be_pushreal(bvm *vm, breal r);
   void be_pushstring(bvm *vm, const char *str);
   void be_pushnstring(bvm *vm, const char *str, size_t n);
   const char* be_pushfstring(bvm *vm, const char *format, ...);
   void be_pushvalue(bvm *vm, int index);
   void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);
   void be_pushntvfunction(bvm *vm, bntvfunc f);
   void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);
   void be_pushcomptr(bvm *vm, void *ptr);

``index`` is the position of the element on the stack, the positive
value is the offset from the bottom of the stack to the top of the
stack, and the negative value is the offset from the top of the stack to
the bottom of the stack.

The return value uses two FFIs:

.. code:: c

   be_return(vm)
   be_return_nil(vm)

These FFIs are actually macros. ``be_return`` returns the object at the
top of the stack, and be_return_nil returns ``nil``.

These FFIs are defined in berry.h.

Now let’s implement the ``add`` function:

.. code:: c

   int my_add_func(bvm *vm)
   {
       /* check the arguments are all integers */
       if (be_isint(vm, 1) && be_isint(vm, 2)) {
           bint a = be_toint(vm, 1); /* get the first argument */
           bint b = be_toint(vm, 2); /* get the second argument */
           be_pushint(vm, a + b); /* push the result to the stack */
       } else if (be_isnumber(vm, 1) && be_isnumber(vm, 2)) { /* check the arguments are all numbers */
           breal a = be_toreal(vm, 1); /* get the first argument */
           breal b = be_toreal(vm, 1); /* get the second argument */
           be_pushreal(vm, a + b); /* push the result to the stack */
       } else { /* unacceptable parameters */
           be_pushnil(vm); /* push the nil to the stack */
       }
       be_return(vm); /* return calculation result */
   }

Then register it in the appropriate place:

.. code:: c

   be_regcfunc(vm, "add", my_add_func);

Instantiate a ``list`` object in a native function
==================================================

Generating instantiated native classes in C can be cumbersome compared
to simple types. This section will guide the reader to instantiate the
``list`` class.

The ``list`` class is a wrapper around the list structure, which has a
``.data`` property for the list structure. Therefore, we first need to
construct a list structure:

.. code:: c

   be_newlist(vm);

The ``be_newlist`` function constructs a value of type ``BE_LIST``. Then
we can operate on the data:

.. code:: c

   be_pushint(vm, 100);
   be_data_append(vm, -2);
   be_pop(vm, 1); /* popping the integer 100 */

The first two lines of code are used to append the integer ``100`` to
the list, and the third line to the integer ``100`` is popped to
facilitate subsequent operations.

Since the ``BE_LIST`` type cannot be used directly in Berry, but is used
by the ``list`` class, we have to build the ``list`` class for it:

.. code:: c

   be_getglobal(vm, "list");
   be_pushvalue(vm, -2); /* push the list data to top */
   be_call(vm, 1); /* call constructor */

The constructor of the ``list`` class allows the use of the ``BE_LIST``
type parameter, which takes the argument as list data.

The complete code is as follows:

.. code:: c

   int m_listtest(bvm *vm)
   {
       be_getglobal(vm, "list");
       be_newlist(vm);
       be_pushint(vm, 100);
       be_data_append(vm, -2);
       be_pop(vm, 1);
       be_call(vm, 1);
       be_pop(vm, 1); /* pop the arguments */
       be_return(vm);
   }

Register the native function in the appropriate place:

.. code:: c

   be_regcfunc(vm, "listtest", m_listtest);
