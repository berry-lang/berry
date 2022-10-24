8. Advanced features
====================

8.1 ``strict`` mode
-------------------

Berry allows full freedom from the developer. But after some experience
in coding with Berry, you will find that there are common mistakes that
are hard to find and that the compiler could help you catch. The
``strict`` mode does additional checks **at compile time** about some
common mistakes.

This mode is enabled with ``import strict`` or when running Berry with
``-s`` option: ``berry -s``

Mandatory ``var`` for local variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This is the most common mistake, a variable assigned without ``var`` is
either global if a global already exists or local otherwise. Strict mode
rejects the assignment if there is no global with the same name.

No more allowed:

.. code:: berry

   def f()
     i = 0    # this is a local variable
     var j = 0
   end

| syntax_error: stdin:2: strict: no global 'i', did you mean 'var i'?
| 

But still works for globals:

.. code:: berry

   g_i = 0
   def f()
     g_i = 1
   end

No overriding of builtins
~~~~~~~~~~~~~~~~~~~~~~~~~

Berry allows to override a builtin. This is however generally not
desirable and a source of hard to find bugs.

::

   map = 1
   syntax_error: stdin:1: strict: redefinition of builtin 'map'

Multiple ``var`` with same name not allowed in same scope
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Berry tolerated multiple declaration of a local variable with the same
name. This is now considered as an error (even without strict mode).

::

   def f()
     var a
     var a   # redefinition of a
   end
   syntax_error: stdin:3: redefinition of 'a'

No hiding of local variable from outer scope
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In Berry you can declare local variables with the same name in inner
scope. The variable in the inner scope hides the variable from outer
scope for the duration of the scope.

The only exception is that variables starting with dot ‘.’ can mask from
outer scope. This is the case with hidden local variable ``.it`` when
multiple ``for`` are embedded.

::

   def f()
     var a    # variable in outer scope
     if a
       var a  # redefinition of a in inner scope
     end
   end
   syntax_error: stdin:4: strict: redefinition of 'a' from outer scope

8.2 Virtual members
-------------------

Virtual members allows you to dynamically and programmatically add
members and methods to classes and modules. You are no more limited to
the members declared at creation time.

This feature is inspired from Python’s ``__getattr__()`` /
``__setattr__()``. The motivation comes from LVGL integration to Berry
in Tasmota. The integration needs hundreds of constants in a module and
thousands of methods mapped to C functions. Statically creation of
attributes and methods does work but consumes a significant amount of
code space.

This features allows to create two methods:

+-----------------------------------+-----------------------------------+
| Berry method                      | Description                       |
+===================================+===================================+
| ``member``                        | ``(name:string) -> any``\ Should  |
|                                   | return the value of the specified |
|                                   | ``name``                          |
+-----------------------------------+-----------------------------------+
| ``setmember``                     | ``(name:st                        |
|                                   | ring, value:any) -> nil``\ Should |
|                                   | store the ``value`` to the        |
|                                   | virtual member with the specified |
|                                   | ``name``                          |
+-----------------------------------+-----------------------------------+

module ``undefined``
~~~~~~~~~~~~~~~~~~~~

The ``member()`` function must be able to distinguish between a member
with a ``nil`` value and the member not existing. To avoid any
ambiguity, the ``member()`` function can indicate that the member does
not exist in two ways:

-  either raise an exception
-  or ``import undefined`` and return the ``undefined`` module. This is
   used as a marker for the VM to know that the attribute does not
   exist, while benefitting from consistent exceptions

Example of a dynamic object to which you can add members, but would
return an error if the member was not previously added.

.. code:: berry

   class dyn
       var _attr
       def init()
           self._attr = {}
       end
       def setmember(name, value)
           self._attr[name] = value
       end
       def member(name)
           if self._attr.contains(name)
               return self._attr[name]
           else
               import undefined
               return undefined
           end
       end
   end

Exemple of usage:

.. code:: berry

   a = dyn()
   a.a

attribute_error: the 'dyn' object has no attribute 'a'
stack traceback:
stdin:1: in function `main`

.. code:: berry

   a.a = 1
   a.a

1

.. code:: berry

   a.a = nil
   a.a

implicit call of ``member()``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the following code ``a.b`` is executed, the Berry VM does the
following:

-  Get the object named ``a`` (local or global), raise an exception if
   it doesn’t exist
-  Check if the object ``a`` is of type ``module``, ``instance`` or
   ``class``. Raise an exception otherwise
-  Check if object ``a`` has a member called ``b``. If yes, return its
   value, if no proceed below
-  If object ``a`` is of type ``class``, raise an exception because
   virtual members do not work for static (class) methods
-  Check if object ``a`` has a member called ``member`` and it is a
   ``function``. If yes call it with parameter ``"b"`` as string. If no,
   raise an exception
-  Check the return value. If it is the module ``undefined`` raise an
   exception indicating that the member does not exist

implicit call of ``setmember()``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When the following code ``a.b = 0`` (mutator) is executed, the Berry VM
does the following:

-  Get the object named ``a`` (local or global), raise an exception if
   it doesn’t exist
-  Check if the object ``a`` is of type ``module``, ``instance`` or
   ``class``. Raise an exception otherwise

   -  If ``a`` is of type ``class``, check if member ``b`` exists. If
      yes, change its value. If no, raise an exception. (virtual members
      don’t work for classes or static methods)
   -  If ``a`` is of type ``instance``, check if member ``b`` exists. If
      yes, change its value. If no, proceed below

      -  Check if ``a`` has a member called ``setmember``. If yes call
         it, if no raise an exception

   -  If ``a`` is of type ``module``. If the module is not read-only,
      create of change the value (``setmember`` is never called for a
      writable module). If the module is read-only, then ``setmember``
      is called if it exists.

Exception handling
~~~~~~~~~~~~~~~~~~

To indicate that a member does not exist, ``member()`` shall return
``undefined`` after ``import undefined``.

You can also raise an exception in ``member()`` but be aware that Berry
might try to call methods like ``tostring()`` that will land on your
``member()`` method if they don’t exist as static methods.

To indicate that a member is invalid, ``setmember()`` should raise an
exception or return ``undefined``. Returning anything else like ``nil``
inficates that the assignment was succesful.

Be aware that you may receive member names that are not valid Berry
identifiers. The syntax ``a.("<->")`` will call ``a.member("<->")`` with
a virtual member name that is not lexically valid, i.e. cannot be called
in regular code, except by using indirect ways like ``introspect`` or
``member()``.

Specificities for classes
~~~~~~~~~~~~~~~~~~~~~~~~~

Access to members of class object do not trigger virtual members. Hence
it is not possible to have virtual static methods.

Specificities for modules
~~~~~~~~~~~~~~~~~~~~~~~~~

Modules do support reading static members with ``member()``.

When writing to a member, the behavior depends whether the module is
writable (in memory) or read-only (in firmware).

If the module is writable, the new members are added directly to the
module and ``setmember()`` is never called.

If the module is read-only, then ``setmember()`` is called whenever you
try to change or create a member. It is then your responsibility to
store the values in a separate object like a global.

Example
~~~~~~~

Example:

.. code:: python

   class T
       var a
       def init()
           self.a = 'a'
       end

       def member(name)
           return "member "+name
       end

       def setmember(name, value)
           print("Set '"+name+"': "+str(value))
       end
   end
   t=T()

Now let’s try it:

.. code:: berry

   t.a

'a'

.. code:: berry

   t.b

'member b'

.. code:: berry

    t.foo

'member foo'

.. code:: berry

   t.bar = 2

Set 'bar': 2

This works for modules too:

.. code:: berry

   m = module()
   m.a = 1
   m.member = def (name)
       return "member "+name
   end
   m.setmember(name, value)
       print("Set '"+name+"': "+str(value))
   end

Trying:

.. code:: berry

   m.a
  
1

.. code:: berry

   m.b
  
'member b'
  
.. code:: berry

   m.c = 3   # the allocation is valid so `setmember()` is not called
   m.c
  
3

More advanced example:

.. code:: berry

   class A
       var i
     
       def member(n)
         if n == 'ii' return self.i end
         return nil     # we make it explicit here, but this line is optional
       end

       def setmember(n, v)
         if n == 'ii' self.i = v end
       end
     end
   a=A()

   a.i      # returns nil
   a.ii     # implicitly calls `a.member("ii")`
   
| attribute_error: the 'A' object has no attribute 'ii'
| stack traceback:
| stdin:1: in function `main`
|

.. code:: berry

   # returns an exception since the member is nil (considered is non-existant)

   a.ii = 42    # implicitly calls `a.setmember("ii", 42)`
   a.ii         # implicitly calls `a.member("ii")` and returns `42`
   
42

.. code:: berry

   a.i          # the concrete variable was changed too

42

8.3 How-to package a module
---------------------------

This guide drives you through the different options of packaging code
for reuse using Berry’s ``import`` directive.

Behavior of ``import``
~~~~~~~~~~~~~~~~~~~~~~

When you use ``import <module> [as <name>]``, the following steps
happen:

-  There is a global cache of all modules already imported. If
   ``<module>`` was already imported, ``import`` returns the value in
   cache already returned by the first call to ``import``. No other
   actions are taken.
-  ``import`` searches for a module of name ``<module>`` in the
   following order:

1. in native modules embedded in the firmware at compile time
2. in file system, starting with current directory, then iterating in
   all directories from ``sys.path``: look for file ``<name>``, then
   ``<name>.bec`` (compiled bytecode), then ``<name>.be``. If
   ``BE_USE_SHARED_LIB`` is enabled, it also looks for shared libraries
   like ``<name>.so`` or ``<name>.dll`` although this optional is
   generally not available on MCUs.

-  The code loaded is executed. The code should finish with a ``return``
   statement. The object returned is stored in the global cache and made
   available to caller (in local or global scope).
-  If the returned object is a ``module`` and if the module as a
   ``init`` member, then an extra step is taken. The function
   ``<module>.init(m)`` is called passing as argument the module object
   itself. The value returned by ``init()`` replaces the value in the
   global cache. Note that the ``init()`` is called at most once during
   the first ``import``.

Note: an implicit ``init(m)`` function is always present in all modules,
even if none was declared. This implicit function has no effect.

Packaging a module
~~~~~~~~~~~~~~~~~~

Here is a simple example of a module:

File ``demo_module.be``:

.. code:: berry

   # simple module
   # use `import demo_module`

   demo_module = module("demo_module")

   demo_module.foo = "bar"

   demo_module.say_hello = def ()
       print("Hello Berry!")
   end

   return demo_module      # return the module as the output of import

Example of use:

.. code:: berry

   import demo_module

   demo_module
   <module: demo_module>

   demo_module.say_hello()

Hello Berry!

.. code:: berry
   
   demo_module.foo

'bar'

.. code:: berry
   
   demo_module.foo = "baz"     # the module is writable, although this is highly discouraged
   demo_module.foo
   
'baz'

Package a singleton (monad)
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The problem of using modules is that they don’t have instance variables
to keep track of data. They are essentially designed for state-less
libraries.

Below you will find an elegant way of packaging a class singleton
returned as an ``import statement``.

To do this, we use different tricks. First we declare the class for the
singleton as an inner class of a function, this prevents from polluting
the global namespace with this class. I.e. the class will not be
accessible by other code.

Second we declare a module ``init()`` function that creates the class,
creates the instance and returns it.

By this scheme, ``import <module>`` actually returns an instance of a
hidden class.

Example of ``demo_monad.be``:

.. code:: berry

   # simple monad
   # use `import demo_monad`

   demo_monad = module("demo_monad")

   # the module has a single member `init()` and delegates everything to the inner class
   demo_monad.init = def (m)
       
       # inncer class
       class my_monad
           var i

           def init()
               self.i = 0
           end

           def say_hello()
               print("Hello Berry!")
           end
       end

       # return a single instance for this class
       return my_monad()
   end

   return demo_monad      # return the module as the output of import, which is eventually replaced by the return value of 'init()'

Example:

.. code:: berry

   import demo_monad
   demo_monad
   <instance: my_monad()>     # it's an instance not a module

   demo_monad.say_hello()

Hello Berry!

.. code:: berry

   demo_monad.i = 42        # you can use it like any instance
   demo_monad.i
 
42

.. code:: berry

   demo_monad.j = 0         # there is strong member checking compared to modules
   
attribute_error: class 'my_monad' cannot assign to attribute 'j'
stack traceback:
stdin:1: in function `main`

8.4 Solidification
------------------

Solidification is the process of capturing compiled Berry structures and
code (classes, modules, maps, lists…) and storing them into firmware. It
reduces dramatically the use of memory, but has some limitations.

``solidify`` module
~~~~~~~~~~~~~~~~~~~

Solidification is handle by ``solidify`` module. This module is not
compiled by default because of its size (~10kB). You need to compile
with ``#define BE_USE_SOLIDIFY_MODULE 1`` directive.

The module has a single member ``dump(x)`` that takes a single argument
(the object to solidify) and output to ``stdout`` the solidified code.

By default, solidify adds all string constants to the global pool. You
can generate weak strings instead (eligible to pruning by the linker) by
setting the second argument to ``true``.

By default ``solidify.dump`` outputs the solidified code to standard
output. You can specify a file as third argument. The file needs to be
open in writeable mode, and is not closed so that you can concatenate
multiple objects.

``solidify.dump(object:any, [, strings_weak:bool, file_out:file]) -> nil``

Solidification of functions
~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can solidify a single function.

Example:

.. code:: berry

   > def f() return "hello" end
   > import solidify
   > solidify.dump(f)

.. code:: c

   /********************************************************************
   ** Solidified function: f
   ********************************************************************/
   be_local_closure(f,   /* name */
     be_nested_proto(
       0,                          /* nstack */
       0,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str(hello),
       }),
       &be_const_str_f,
       &be_const_str_solidified,
       ( &(const binstruction[ 1]) {  /* code */
         0x80060000,  //  0000  RET    1   K0
       })
     )
   );
   /*******************************************************************/

To compile using weak strings (i.e. strings that can be eliminated by
the linker if the object is not included in the target executable), use
``solidify.dump(f, true)``:

.. code:: c


   /********************************************************************
   ** Solidified function: f
   ********************************************************************/
   be_local_closure(f,   /* name */
     be_nested_proto(
       0,                          /* nstack */
       0,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str_weak(hello),
       }),
       be_str_weak(f),
       &be_const_str_solidified,
       ( &(const binstruction[ 1]) {  /* code */
         0x80060000,  //  0000  RET    1   K0
       })
     )
   );
   /*******************************************************************/

Solidification of classes
~~~~~~~~~~~~~~~~~~~~~~~~~

When you solidify a class, it embeds all the sub-elements. An ``C`` stub
is also added to create the class and add it to the global scope.

.. code:: berry

   >  class demo
         var i
         static foo = "bar"

         def init()
             self.i = 0
         end

         def say_hello()
             print("Hello Berry!")
         end
     end
   > import solidify
   > solidify.dump(demo)

.. code:: c


   /********************************************************************
   ** Solidified function: init
   ********************************************************************/
   be_local_closure(demo_init,   /* name */
     be_nested_proto(
       1,                          /* nstack */
       1,                          /* argc */
       2,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 2]) {     /* constants */
       /* K0   */  be_nested_str(i),
       /* K1   */  be_const_int(0),
       }),
       &be_const_str_init,
       &be_const_str_solidified,
       ( &(const binstruction[ 2]) {  /* code */
         0x90020101,  //  0000  SETMBR R0  K0  K1
         0x80000000,  //  0001  RET    0
       })
     )
   );
   /*******************************************************************/


   /********************************************************************
   ** Solidified function: say_hello
   ********************************************************************/
   be_local_closure(demo_say_hello,   /* name */
     be_nested_proto(
       3,                          /* nstack */
       1,                          /* argc */
       2,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str(Hello_X20Berry_X21),
       }),
       &be_const_str_say_hello,
       &be_const_str_solidified,
       ( &(const binstruction[ 4]) {  /* code */
         0x60040001,  //  0000  GETGBL R1  G1
         0x58080000,  //  0001  LDCONST    R2  K0
         0x7C040200,  //  0002  CALL   R1  1
         0x80000000,  //  0003  RET    0
       })
     )
   );
   /*******************************************************************/


   /********************************************************************
   ** Solidified class: demo
   ********************************************************************/
   be_local_class(demo,
       1,
       NULL,
       be_nested_map(4,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(i, -1), be_const_var(0) },
           { be_const_key(say_hello, 2), be_const_closure(demo_say_hello_closure) },
           { be_const_key(init, -1), be_const_closure(demo_init_closure) },
           { be_const_key(foo, 1), be_nested_str(bar) },
       })),
       (bstring*) &be_const_str_demo
   );
   /*******************************************************************/

   void be_load_demo_class(bvm *vm) {
       be_pushntvclass(vm, &be_class_demo);
       be_setglobal(vm, "demo");
       be_pop(vm, 1);
   }

Sub-classes are also supported.

.. code:: berry

   > class demo_sub : demo
         var j

         def init()
             super(self).init()
             self.j = 1
         end
     end
   > solidify.dump(demo_sub)

.. code:: c


   /********************************************************************
   ** Solidified function: init
   ********************************************************************/
   be_local_closure(demo_sub_init,   /* name */
     be_nested_proto(
       3,                          /* nstack */
       1,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 3]) {     /* constants */
       /* K0   */  be_nested_str(init),
       /* K1   */  be_nested_str(j),
       /* K2   */  be_const_int(1),
       }),
       &be_const_str_init,
       &be_const_str_solidified,
       ( &(const binstruction[ 7]) {  /* code */
         0x60040003,  //  0000  GETGBL R1  G3
         0x5C080000,  //  0001  MOVE   R2  R0
         0x7C040200,  //  0002  CALL   R1  1
         0x8C040300,  //  0003  GETMET R1  R1  K0
         0x7C040200,  //  0004  CALL   R1  1
         0x90020302,  //  0005  SETMBR R0  K1  K2
         0x80000000,  //  0006  RET    0
       })
     )
   );
   /*******************************************************************/


   /********************************************************************
   ** Solidified class: demo_sub
   ********************************************************************/
   extern const bclass be_class_demo;
   be_local_class(demo_sub,
       1,
       &be_class_demo,
       be_nested_map(2,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(init, -1), be_const_closure(demo_sub_init_closure) },
           { be_const_key(j, 0), be_const_var(0) },
       })),
       be_str_literal("demo_sub")
   );
   /*******************************************************************/

   void be_load_demo_sub_class(bvm *vm) {
       be_pushntvclass(vm, &be_class_demo_sub);
       be_setglobal(vm, "demo_sub");
       be_pop(vm, 1);
   }

Solidification of modules
~~~~~~~~~~~~~~~~~~~~~~~~~

When you solidify a module, it embeds all the sub-elements. It also
works with embedded lists or maps.

.. code:: berry

   > def say_hello() print("Hello Berry!") end
   > m = module("demo_module")
   > m.i = 0
   > m.s = "foo"
   > m.f = say_hello
   > m.l = [0,1,"a"]
   > m.m = {"a":"b", "2":3}
   > import solidify
   > solidify.dump(m)

.. code:: c

   /********************************************************************
   ** Solidified function: say_hello
   ********************************************************************/
   be_local_closure(demo_module_say_hello,   /* name */
     be_nested_proto(
       2,                          /* nstack */
       0,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str(Hello_X20Berry_X21),
       }),
       &be_const_str_say_hello,
       &be_const_str_solidified,
       ( &(const binstruction[ 4]) {  /* code */
         0x60000001,  //  0000  GETGBL R0  G1
         0x58040000,  //  0001  LDCONST    R1  K0
         0x7C000200,  //  0002  CALL   R0  1
         0x80000000,  //  0003  RET    0
       })
     )
   );
   /*******************************************************************/


   /********************************************************************
   ** Solidified module: demo_module
   ********************************************************************/
   be_local_module(demo_module,
       "demo_module",
       be_nested_map(5,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(l, -1), be_const_simple_instance(be_nested_simple_instance(&be_class_list, {
           be_const_list( *     be_nested_list(3,
       ( (struct bvalue*) &(const bvalue[]) {
           be_const_int(0),
           be_const_int(1),
           be_nested_str(a),
       }))    ) } )) },
           { be_const_key(m, 3), be_const_simple_instance(be_nested_simple_instance(&be_class_map, {
           be_const_map( *     be_nested_map(2,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(a, -1), be_nested_str(b) },
           { be_const_key(2, -1), be_const_int(3) },
       }))    ) } )) },
           { be_const_key(i, 4), be_const_int(0) },
           { be_const_key(f, -1), be_const_closure(demo_module_say_hello_closure) },
           { be_const_key(s, -1), be_nested_str(foo) },
       }))
   );
   BE_EXPORT_VARIABLE be_define_const_native_module(demo_module);
   /********************************************************************/

Limitations of solidification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Solidification works for many objects: ``class``, ``module``,
``functions`` and embedded constants or objects like ``int``, ``real``,
``string``, ``list`` and ``map``.

Limitations:

-  Upvals are not supported. You cannot solidify a closure that captures
   upvals from outer scope
-  Capturing global variables requires to compile with ``-g`` “named
   globals” option (enabled by default on Tasmota)
-  String constants are limited to 255 bytes, long strings (above 255
   characters are not supported - because nobody ever had a need for)
-  Solidified objects are read-only, this has some consequences on
   classes. You can solidify a class with its static members when it is
   created, but you cannot solidify a function that creates a class
   deriving from another class or with static members. The core reason
   is that setting the superclass or assigning the static members is
   implemented using mutating code on the new class - which cannot work
   on a read-only non-mutating class.
-  Solidified code may be dependent on the size of ``int`` and ``real``
   and may not be ported across MCUs with different sizes of types. You
   may need to re-solidify for each target.
