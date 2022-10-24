9. FFI
======

**Foreign Function Interface** (FFI) is an interface for interaction
between different languages. Berry provides a set of FFI to realize the
interaction with C language, this set of interfaces is also very easy to
use in C++. Most of the FFI interfaces are functions, and their
declarations are placed in the *berry.h* file. In order to reduce the
amount of RAM used, FFI also provides a mechanism for generating a fixed
hash table during C compilation. This mechanism must use external tools
to generate C code.

9.1 Basics
----------

The most important interactive function in FFI should be the function of
calling Berry code and C function mutually. In order to realize that two
languages call each other’s functions, we must first understand the
parameter passing mechanism of the Berry function.

9.1.1 Virtual Machine
~~~~~~~~~~~~~~~~~~~~~

Unlike compiled languages, Berry language cannot run directly on a
physical machine, but in a specific software environment, which is
**Virtual Machine** (VM). Similar to a real computer, the source code in
text form cannot be executed in a virtual machine, but must be converted
into “bytecode” by a compiler. The Berry virtual machine is defined as a
C structure ``bvm``, the content of this structure is invisible to FFI.
Through some FFI functions, we can create and initialize a virtual
machine. We introduce the use of virtual machines through a simple
example:

.. code:: c

   void berry_test(void)
   {
       bvm *vm = be_vm_new(); // Construct a VM
       be_loadstring(vm, "print('Hello Berry')"); // Compile test code
       be_pcall(vm, 0); // Call function
       be_vm_delete(vm); // Destroy the VM
   }

This code gives a complete example of using a virtual machine. First,
call the function ``be_vm_new`` to construct a new virtual machine, and
then all operations are completed in this virtual machine object.
``be_vm_new`` The function will automatically link the standard library
when creating a virtual machine. The function of lines 4 to 5 is to
compile the source code in a string into a Berry function and then call
it. Finally, call the ``be_vm_delete`` function on line 6 to destroy the
virtual machine. Executing this function will get a line of output in
the terminal:

::

   Hello Berry

In all scenarios, the virtual machine construction, library loading and
destruction process are the same as the 3rd, 4th and 6th lines in the
above example. If necessary, the way to compile or load the source code
may be different. For example, for the source code in the form of a
file, it can be compiled through the ``be_loadfile`` function. The
source code will be compiled into a Berry function, and the function
will be stored on the top of the stack. The Berry function can be
executed by calling the FFI function ``be_pcall`` or ``be_call``. You
can also use the REPL through the ``be_repl`` function. The interface of
the REPL will be described in the relevant chapters.

9.1.2 Virtual Stack
~~~~~~~~~~~~~~~~~~~

Berry uses a virtual stack and native functions written in C to pass
values. Each element in the stack is a Berry value. When Berry code
calls a native function, it always creates a new stack and pushes all
the parameters onto the stack. This virtual stack can also be used in C
code to store data, and the value stored in the stack will not be
reclaimed by the garbage collector.

.. figure:: https://user-images.githubusercontent.com/49731213/135971121-059e665c-7f65-4a6a-bbe4-da9acf4cf8b5.png
   :alt: Virtual_Stack

   Virtual_Stack

The virtual stack used by Berry is shown in Figure above.

The virtual stack grows from left to right. When Berry code calls a
native function, it will get an initial stack. The position of the first
value of the stack is called **base**, and the last position is called
**top**, the native function Only the value from the bottom of the stack
to the position before the top of the stack can be accessed. The
position of the bottom of the stack is fixed, while the position of the
top of the stack can be moved, and the top of the stack is always empty.
The reason for this property is that after the new value is pushed into
the virtual stack, the original position of the top of the stack will be
written The new value, the top pointer of the stack will move forward to
the next position; conversely, if the value at the top of the virtual
stack is popped, the top pointer of the stack will decrease 1. At this
time, although the position of the top pointer of the stack is
objectively Value, but this value is invalid and may be cleared at any
time, so the pointer position on the top of the stack is still empty.
When the virtual stack is empty, the bottom pointer ``base`` is equal to
the top pointer ``top``. The virtual stack does not strictly follow the
operating rules of the stack: in addition to push and pop, the virtual
stack can also be accessed by index, and even insert or delete values at
any position. There are two ways to index elements in the stack: one is
based on the bottom of the stack **Absolute index**, the absolute index
value is a positive integer starting from 1; the other is based on the
top of the stack **Relative index**, The relative index value is a
negative integer starting from  − 1. Take Figure above as an example,
the index value 1, 2…8 is an absolute index, and the absolute index of
an element is the distance from the element to the bottom of the stack.
The index value  − 1,  − 2… − 8 is a relative index, and the relative
index value of an element is the negative number of the distance from
the element to the top of the stack. If an index value *index* is valid,
then the element it refers to should be between the bottom of the stack
and the top of the stack, which means that the expression

``1 ≤ abs(*index*) ≤ *top* − *base* + 1`` is satisfied.

For convenience, we stipulate that the stack bottom pointer ``base`` is
used as a reference, and its absolute index 1, and the previous value of
``base`` is not considered (usually, ``base`` is not the bottom position
of the entire stack). For example, when a native function returns, the
location where the return value is stored is just before ``base``, and
these locations are usually not accessible by the native function.

Operate Virtual Stack
~~~~~~~~~~~~~~~~~~~~~

Index and stack size
^^^^^^^^^^^^^^^^^^^^

As mentioned earlier, two indexing methods can be used to access the
virtual stack, and the index value must be valid. At the same time, in
many cases it is also necessary to push new values onto the stack. In
this case, the programmer must ensure that the stack will not overflow.
By default, Berry guarantees ``BE_STACK_FREE_MIN`` space for native
functions to use. This value can be modified in the file *berry.h*. Its
default value is usually 10, which should be sufficient in most cases.
If you really need to expand the stack, you can call the FFI function
``be_stack_require``. The prototype of this function is:

.. code:: c

   void be_stack_require(bvm *vm, int count);

The parameter ``count`` is the amount of space needed. When the
remaining space in the virtual stack is insufficient, the stack capacity
will be expanded, otherwise this function will do nothing.

**caveat**: If a stack overflow occurs, or if an invalid index is used
to access the stack, the program will crash. You can turn on the
debugging switch ``BE_DEBUG`` (section [section::BE_DEBUG]), which will
turn on the assertion function, and you can get some debugging
information at runtime to catch errors such as stack overflow or invalid
index.

Get value from stack
^^^^^^^^^^^^^^^^^^^^

There is a set of functions in FFI to get values from the virtual stack.
These functions usually convert the values in the stack into simple
values supported by the C language and then return. The following are
commonly used FFIs to get values from the stack:

.. code:: c

   bint be_toint(bvm *vm, int index);
   breal be_toreal(bvm *vm, int index);
   int be_tobool(bvm *vm, int index);
   const char* be_tostring(bvm *vm, int index);
   void* be_tocomptr(bvm *vm, int index);

The parameter form of these functions is the same, but the return value
is different. The first four functions are easy to understand. Just like
their names, the function of ``be_toint`` is to convert the values in
the virtual stack to C integer values (``bint`` is usually an alias of
type ``int``) and return. The function of the last function
``be_tocomptr`` is to take out a pointer value of a general type from
the virtual stack. The specific meaning of this pointer is explained by
the C program itself.

These functions use the same way to interpret the parameters: the
parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the element to be retrieved, which can be a
relative index or an absolute index. You cannot use FFI to remove
Berry’s complex data types from the virtual stack, so you cannot remove
a ``map`` type or ``class`` type from the stack. One of the benefits of
this design is that there is no need to consider garbage collection in
native functions.

Native function
~~~~~~~~~~~~~~~

**Native function** It is implemented by C language and can be called by
Berry code. The native function can be an ordinary function. In this
case, calling the native function will not generate any dynamically
allocated space, just like a normal C function call. Native functions
can also be closures, and space needs to be allocated for free variables
when creating native closures. Under normal circumstances, simple native
functions are sufficient to meet the needs. They save resources than
native closures and are easier to use.

Define native function
^^^^^^^^^^^^^^^^^^^^^^

The native function itself is a C function, but they all have a specific
form. The definition of the native function is:

.. code:: c

   int a_native_function(bvm *vm)
   {
       // do something ...
   }

The native function must be a C function whose parameter is a pointer to
``bvm`` and the return value is ``int``. Berry’s functions must return a
value, and native functions are no exception. Unlike the return value of
the C language, the return value of the native function is not the value
carried by the C ``return`` statement. You can use these FFIs to return
the value of the native function, and they also make the C function
return:

.. code:: c

   be_return(bvm *vm);
   be_return_nil(bvm *vm);

These FFIs are actually two macros, and there is no need to use the C
``return`` statement when using them. ``be_return`` Will put the top of
the virtual stack

Use native function
^^^^^^^^^^^^^^^^^^^

After the native function is defined, it must be added to the
interpreter in some way before it can be called in Berry code. One of
the easiest ways is to add native functions to Berry’s built-in object
table. The process of setting native objects as Berry built-in objects
is called **registered**. The FFI of the registered native function is:

.. code:: c

   void be_regfunc(bvm *vm, const char *name, bntvfunc f);

``vm`` is the current virtual machine instance, ``name`` is the name of
the native function, and ``f`` is the pointer of the native function.
The specific behavior of this function is related to the value of the
``BE_USE_PRECOMPILED_OBJECT`` macro (although the FFI is still available
when using the compile-time construction technique, it cannot
dynamically register the built-in variables. In this case, please refer
to the method of registering the built-in objects. 1.3). The definition
of native function type ``bntvfunc`` is:

.. code:: c

   typedef int (*bntvfunc)(bvm*);

In fact, the ``bntvfunc`` type is the function pointer type with the
parameter ``bvm`` and the return value type ``int``. ``be_regfunc`` The
function should be called before parsing the Berry source code.

You can also push the native function into the virtual stack, and then
use an FFI function ``be_call`` to call it. A more common usage is to
use the native function object in the virtual stack as the return value.

Complete example
^^^^^^^^^^^^^^^^

We end this section with a simple example. Here, we are going to
implement a ``add`` function that adds two numbers and returns the
result of the calculation. First, we define a native function to
implement this function:

.. code:: c

   static int l_add(bvm *vm)
   {
       int top = be_top(vm); // Get the number of arguments
       /* Verify the number and type of arguments */
       if (top == 2 && be_isnumber(vm, 1) && be_isnumber(vm, 2)) {
           breal x = be_toreal(vm, 1); // Get the first argument
           breal y = be_toreal(vm, 2); // Get the second argument
           be_pushreal(vm, x + y); // Push the result onto the stack
           be_return(vm); // Return the value at the top of the stack
       }
       be_return_nil(vm); // Return nil when something goes wrong
   }

Native functions usually do not need to be used outside the C file, so
they are generally declared as ``static`` types. Use the ``be_top``
function to get the absolute index of the top of the virtual stack
(``top`` value), which is the capacity of the stack. We can call
``be_top`` before the native function performs the virtual stack
operation, at this time the virtual stack capacity is equal to the real
parameter amount. For the ``add`` function, we need two parameters to
participate in the operation, so check whether the number of parameters
is 2 in the fourth line (``top == 2``). And we need to check whether the
two parameters are both numeric types, so we need to call the
``be_isnumber`` function to check. If everything is correct, the
parameters will be taken out of the virtual stack, then the calculation
result will be pushed onto the stack, and finally returned using
``be_return``. If the parameter verification fails, ``be_return_nil``
will be called to return the value of ``nil``.

Next, register this native function to the built-in object table. For
simplicity, we register it after loading the library:

.. code:: c

   bvm *vm = be_vm_new(); // Construct a VM
   be_regfunc(vm, "myadd", l_add); // Register the native function "myadd"

The second line is where the native function is registered, and we named
it ``myadd``. At this point, the definition and registration of the
native function is complete. As a verification, you can compile the
interpreter, then enter the REPL and run some tests. You should get
results like this:

.. code:: berry

   > myadd
   <function: 0x562a210f0f90>
   > myadd(1.0, 2.5)
   3.5
   > myadd(2.5, 2)
   4.5
   > myadd(1, 2)
   3

Types and Functions
-------------------

Type
~~~~

This section will introduce some types that need to be used in FFI.
These types are generally used by FFI functions. Generally, the types
and declarations in FFI can be found in the *berry.h* file. Unless
otherwise specified in this section, the definition or declaration is
provided in *berry.h* by default.

``bvm`` Type is used to store the state information of the Berry virtual
machine. Details of this type are not visible to external programs.
Therefore, this definition can only be found in the *berry.h* file:

.. code:: c

   typedef struct bvm bvm;

Most FFI functions use the ``bvm`` type as the first parameter, because
they all operate on the virtual machine internally. Hiding the internal
implementation of ``bvm`` helps reduce the coupling between the FFI
standard and the VM. Outside the interpreter, usually only ``bvm``
pointers are used. To create a new ``bvm`` object, use the ``be_vm_new``
function, and destroy the ``bvm`` object using the ``be_vm_delete``
function.

Native function type. The definition of this type is:

.. code:: c

   typedef int (*bntvfunc)(bvm*);

This type is a native function pointer, and some FFIs that register or
add native functions to the virtual machine use parameters of this type.
Variables or parameters of this type need to be initialized with a
function name whose parameter is of type ``bvm`` and whose return value
is of type ``int``.

This type is used when registering native functions in batches or
building native classes. It is defined as:

.. code:: c

   typedef struct {
       const char *name; // The name of the function or object
       bntvfunc function; // The function pointer
   } bnfuncinfo;

The ``name`` member of ``bnfuncinfo`` represents the name of a function
or object, and the ``function`` member is a native function pointer.

This type is a built-in integer type of Berry. It is defined in the
*berry.h* document. By default, ``bint`` is implemented using the
``long long`` type, and the implementation of ``bint`` can be modified
by modifying the configuration file.

This is Berry’s built-in real number type, which is actually the
floating point type in C language. ``breal`` is defined as:

.. code:: c

   #if BE_SINGLE_FLOAT != 0
       typedef float breal;
   #else
       typedef double breal;
   #endif

You can use the macro ``BE_SINGLE_FLOAT`` to control the specific
implementation of ``breal``: when the value of ``BE_SINGLE_FLOAT`` is
``0``, the ``double`` type implementation ``breal`` will be used,
otherwise the ``float`` type implementation ``breal`` will be used.

[section::errorcode]

This enumeration type is used in some FFI return values. The definition
of this type is:

.. code:: c

   enum berrorcode {
       BE_OK = 0,
       BE_IO_ERROR,
       BE_SYNTAX_ERROR,
       BE_EXEC_ERROR,
       BE_MALLOC_FAIL,
       BE_EXIT
   };

The meaning of these enumeration values are:

-  ``BE_OK``: There is no error, the function is executed successfully.

-  ``BE_IO_ERROR``: A file reading error occurred when the interpreter
   was reading the source file. The error is generally caused by the
   absence of the file.

-  ``BE_SYNTAX_ERROR``: A syntax error occurred when the interpreter was
   compiling the source code. After this error occurs, the interpreter
   will not generate bytecode, so it cannot continue to execute
   bytecode.

-  ``BE_EXEC_ERROR``: Runtime error. When this error occurs, execution
   of Berry code is stopped and the environment is restored to the most
   recent recovery point.

-  ``BE_MALLOC_FAIL``: Memory allocation failed. This error is caused by
   insufficient heap space.

-  ``BE_EXIT``: Indicates that the program exits and the enumeration
   value is not an error. Executing Berry’s ``exit`` function causes the
   interpreter to return this value.

It should be noted that when a ``BE_MALLOC_FAIL`` error occurs, dynamic
memory allocation can no longer be performed, which means that string
objects can no longer be allocated, so the function that returns this
error usually does not give more detailed error information.

Functions and Macros
~~~~~~~~~~~~~~~~~~~~

This function is used to create a new virtual machine instance. The
function prototype is:

.. code:: c

   bvm* be_vm_new(void);

The return value of the function is a pointer to the virtual machine
instance. ``be_vm_new`` The number is the first function called when the
Berry interpreter is created. This function will do a lot of work: apply
for memory for the virtual machine, initialize the state and attributes
of the virtual machine, initialize the GC (garbage collector), and The
standard library is loaded into the virtual machine instance, etc.

This function is used to destroy a virtual machine instance, the
function prototype is:

.. code:: c

   void be_vm_delete(bvm *vm);

The parameter ``vm`` is the pointer of the virtual machine object to be
destroyed. Destroying the virtual machine will release all the objects
in the virtual machine, including the values in the stack and the
objects managed by the GC. The virtual machine pointer after destruction
will be an invalid value, and it can no longer be referenced.

This function is used to load a piece of source code from the buffer and
compile it into bytecode. The prototype of the function is:

.. code:: c

   int be_loadbuffer(bvm *vm, const char *name, const char *buffer, size_t length);

The parameter ``vm`` is the virtual machine pointer. ``name`` is a
string, which is usually used to mark the source of the source code. For
example, the source code input from the standard input device can pass
the string ``"stdin"`` to this parameter, and the source code input from
the file can be The file name is passed to this parameter. ``buffer``
The parameter is the buffer for storing the source code. The
organization of this buffer is very similar to the string of C. It is a
continuous sequence of characters, but the buffer pointed to by
``buffer`` does not require ``’\0’`` characters as Terminator.
``length`` The parameter indicates the length of the buffer. This length
refers to the number of bytes of source code text in the buffer.

To give a simple example, if we want to use the ``be_loadbuffer``
function to compile a string, the general usage is:

.. code:: c

   const char *str = "print('Hello Berry')";
   be_loadbuffer(vm, "string", str, strlen(str));

Here we use the string ``"string"`` to represent the source code, you
can also modify it to any value. Note that the C standard library
function ``strlen`` function is used here to get the length of the
string buffer (actually the number of bytes in the string).

If the compilation is successful, ``be_loadbuffer`` will compile the
source code into a Berry function and place it on the top of the virtual
stack. If the compilation encounters an error, ``be_loadbuffer`` will
return an error value of type ``berrorcode`` (Section errorcode, and if
possible, will store the specific error message string at the top of the
virtual stack.

``be_loadstring`` is a macro defined as:

.. code:: c

   #define be_loadstring(vm, str) be_loadbuffer((vm), "string", (str), strlen(str))

This macro is just a simple wrapper for the ``be_loadbuffer`` function.
``vm`` The parameter is a pointer to the virtual machine instance, and
the ``str`` parameter is a pointer to the source code string. It is very
convenient to use ``be_loadstring`` to compile strings, for example:

.. code:: c

   be_loadstring(vm, "print('Hello Berry')");

This way of writing is more concise than using ``be_loadbuffer``, but
you must ensure that the string ends with a ``’\0’`` character.

This function is used to compile a source code file. The function
prototype is:

.. code:: c

   int be_loadfile(bvm *vm, const char *name);

The function of this function is similar to the ``be_loadbuffer``
function, except that the function will be compiled by reading the
source code file. The parameter ``vm`` is the pointer of the virtual
machine instance, and the parameter ``name`` is the file name of the
source file. This function will call the file interface, and by default
it will use functions such as ``fopen`` in the C standard library to
manipulate files.

If you use the file interface of the C standard library, you can use
relative path or absolute path file names. If the file does not exist,
``be_loadfile`` will return a ``BE_IO_ERROR`` error (section errorcode)
and push the error message onto the top of the stack. Other error
messages are the same as the ``be_loadbuffer`` function. It is
recommended to use the ``be_loadfile`` function to compile the source
file, instead of reading all the source files into a buffer, and then
call the ``be_loadbuffer`` function to compile the source code. The
former will read the source file in segments and only create a small
read buffer in the memory, thus saving more memory.

This function returns the absolute index value of the top element in the
virtual stack. This value is also the number of elements in the virtual
stack (the capacity of the virtual stack). Call this function before
adding or subtracting elements in the virtual stack to get the number of
parameters of the native function. The prototype of this function is:

.. code:: c

   int be_top(bvm *vm);

This function is usually used to obtain the number of parameters of a
native function. When used for this purpose, it is recommended to call
``be_top`` at the top of the native function body. E.g:

.. code:: c

   static int native_function_example(bvm *vm)
   {
       int argc = be_top(vm); // Get the number of arguments
       // ...
   }

This function converts the type of the Berry object into a string and
returns it. For example, it returns ``"int"`` for an integer object, and
``"function"`` for a function object. The prototype of this function is:

.. code:: c

   const char* be_typename(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the object to be operated. The ``type``
function in the Berry standard library is implemented by calling
``be_typename``. Please refer to section baselib_type for the return
string corresponding to the parameter type.

This function is used to get the class name of an object or class. The
function prototype is:

.. code:: c

   const char* be_classname(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the object to be operated. If the value at
``index`` is an instance, the ``be_classname`` function will return the
class name string to which the instance belongs, and if the value at
``index`` is a class, it will directly return the class name string. In
other cases ``be_classname`` will return ``NULL``.

This function returns the length of the specified Berry string. The
function prototype is:

.. code:: c

   int be_strlen(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the object to be operated. This function
returns the number of bytes in the string at ``index`` (the ``’\0’``
characters at the end of the Berry string are not counted). If the value
of the ``index`` position is not a string, the ``be_strlen`` function
will return ``0``.

Although the ``Berry`` string is compatible with the C string format, it
is not recommended to use the ``strlen`` function of the C standard
library to measure the length of the Berry string. For Berry strings,
``be_strlen`` is faster than ``strlen`` and has better compatibility.

This function is used to splice two Berry strings. The function
prototype is:

.. code:: c

   void be_strconcat(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance.
This function will concatenate the string at the parameter position of
``index`` with the string at the top position of the stack, and then put
the resulting string into the position indexed by ``index``.

This function pops the value at the top of the stack. The function
prototype is:

.. code:: c

   void be_pop(bvm *vm, int n);

The parameter ``vm`` is the pointer of the virtual machine instance, and
the parameter ``n`` is the number of values to be popped from the stack.
Note that the value of ``n`` cannot exceed the capacity of the stack.

This function will remove a value from the stack. The function prototype
is:

.. code:: c

   void be_remove(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
the parameter ``index`` is the index of the object to be removed. After
the value at ``index`` is moved out, the following values will be filled
forward, and the stack capacity will be reduced by one. The value of
``index`` cannot exceed the capacity of the stack.

This function returns the absolute index value of a given index value,
and its function prototype is:

.. code:: c

   int be_absindex(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
the parameter ``index`` is the input index value. If ``index`` is
positive, the return value of ``be_absindex`` is the value of ``index``.
If ``index`` is negative, the return value of textttbe_absindex is the
absolute index value corresponding to ``index``. When ``index`` is a
negative value (relative index), its index position cannot be lower than
the bottom of the stack.

This function creates a new ``list`` value, and its function prototype
is:

.. code:: c

   void be_newlist(bvm *vm);

The parameter ``vm`` is the pointer of the virtual machine instance.
After this function is successfully called, the new ``list`` value will
be pushed onto the top of the stack. ``list`` value is an internal
representation of a list, not to be confused with an instance of the
``list`` class.

This function creates a new ``map`` value, and its function prototype
is:

.. code:: c

   void be_newmap(bvm *vm);

The parameter ``vm`` is the pointer of the virtual machine instance.
After this function is successfully called, the new ``map`` value will
be pushed onto the top of the stack. ``map`` value is an internal
representation of a list, not to be confused with an instance of the
``map`` class.

This function pushes the global variable with the specified name onto
the stack. Its function prototype is:

.. code:: c

   void be_getglobal(bvm *vm, const char *name);

The parameter ``vm`` is the pointer of the virtual machine instance, and
the parameter ``name`` is the name of the global variable. After this
function is called, the global variable named ``name`` will be pushed
onto the top of the virtual stack.

This function is used to set the value of the member variable of the
instance object class. The function prototype is:

.. code:: c

   void be_setmember(bvm *vm, int index, const char *k);

The parameter ``vm`` is the pointer of the virtual machine instance, the
parameter ``index`` is the index of the instance object, and the
parameter ``k`` is the name of the member. This function will copy the
value at the top of the stack to the member ``k`` of the index position
instance. Note that the top element of the stack will not pop up
automatically.

This function is used to get the value of the member variable of the
instance object class. The function prototype is:

.. code:: c

   void be_getmember(bvm *vm, int index, const char *k);

The parameter ``vm`` is the pointer of the virtual machine instance, the
parameter ``index`` is the index of the instance object, and the
parameter ``k`` is the name of the member. This function pushes the
value of the member of the index position instance ``k`` onto the top of
the virtual stack.

This function is used to get the value of ``list`` or ``map``. The
function prototype is:

.. code:: c

   void be_getindex(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
the parameter ``index`` is the index of the object to be operated. This
function is used to get an element from the ``map`` or ``list``
container (internal values, not instances of ``map`` or ``list``
classes), and the index of the element is stored at the top of the stack
(relative index is  − 1). After calling this function, the value
obtained from the container will be pushed onto the top of the stack. If
there is no subscript pointed to by the container, the value of ``nil``
will be pushed onto the top of the stack. For example, if the element
with index 1 in the virtual stack is a ``list``, and we want to extract
the element with index 0 from it, then we can use the following code:

.. code:: c

   be_pushint(vm, 0); // Push the index value 0 onto the virtual-stack
   be_getindex(vm, 1); // Get an element from the list container

We first push the integer value ``0`` onto the stack, and this value
will be used as the index to get the element from the ``list``
container. The second line of code implements to get elements from the
``list`` container. The index value of the ``list`` container in the
example is 1 in the virtual stack. The retrieved element is stored at
the top of the stack, and we can use the relative index  − 1 to access
it.

This function is used to set a value in ``list`` or ``map``. The
function prototype is:

.. code:: c

   void be_setindex(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
the parameter ``index`` is the subscript of the object to be operated.
This function is used to write an element of the ``map`` or ``list``
container. The index of the value to be written in the virtual stack is
 − 1, and the index of the subscript of the write position in the
virtual stack is  − 2. If the element with the specified subscript does
not exist in the container, the write operation will fail.

Assuming that the position with index ``1`` in the virtual stack has a
value of ``map``, and it has an element with a subscript of ``"test"``,
an example of setting the element at the subscript of ``"test"`` to
``100`` is:

.. code:: c

   be_pushstring(vm, "test"); // Push the index "index"
   be_pushint(vm, 100); // Push the value 100
   be_setindex(vm, 1); // Set the key-value pair to map["test"] = 100

We must first push the subscript and the value to be written on the
stack in order. For ``map``, it is a key-value pair. In the example, the
first two lines of code complete these tasks. The third line calls the
``be_setindex`` function to write the value into the ``map`` object.

This function is used to read an Up Value of the native closure. The
function prototype is:

.. code:: c

   void be_getupval(bvm *vm, int index, int pos);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the native closure index value of the Up Value to be read;
``pos`` is the position of the Up Value in the native closure Up Value
table (numbering starts from 0). The read Up Value will be pushed onto
the top of the virtual stack.

This function is used to set an Up Value of the native closure. The
function prototype is:

.. code:: c

   void be_setupval(bvm *vm, int index, int pos);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the native closure index value to be written into the Up
Value; ``pos`` is the position of the Up Value in the native closure Up
Value table (numbering starts from 0). This function obtains a value
from the top of the virtual stack and writes it to the target Up Value.
After the operation is completed, the top value of the stack will not be
popped from the stack.

This function is used to get the parent object of the base class or
instance of the class. The function prototype is:

.. code:: c

   void be_getsuper(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the class or object to be operated. If the value at
``index`` is a class with a base class, the function will push its base
class onto the top of the stack; if the value at ``index`` is an object
with a parent object, the function will take its parent The object is
pushed onto the top of the stack; otherwise, a value of ``nil`` is
pushed onto the top of the stack.

This function is used to get the number of elements contained in the
container. The function prototype is:

.. code:: c

   int be_data_size(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the container object to be operated. If the
value at ``index`` is a Map value or List value, the function returns
the number of elements contained in the container, otherwise it returns
``-1``.

This function is used to append a new element to the end of the
container. The function prototype is:

.. code:: c

   void be_data_push(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the container object to be operated. The
object at ``index`` must be a List value. This function gets a value
from the top of the stack and appends it to the end of the container.
After the operation is completed, the value at the top of the stack will
not be popped from the stack.

This function is used to insert a pair of elements into the container.
The function prototype is:

.. code:: c

   void be_data_insert(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the container object to be operated. The
object at ``index`` must be a List value or a Map value. The inserted
element forms a pair of key-value pairs. The value is stored at the top
of the stack, and the key is stored at the previous index on the top of
the stack. It should be noted that the key inserted into the Map
container cannot be a ``nil`` value, and the key inserted into the List
container must be an integer value. If the operation is successful, the
function will return ``bture``, otherwise it will return ``bfalse``.

This function is used to remove an element in the container. The
function prototype is:

.. code:: c

   void be_data_remove(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the container object to be operated. The
object at ``index`` must be a List value or Map value. For the Map
container, the key to delete the element is stored on the top of the
virtual stack (need to be pressed before the function call); for the
List container, the index of the element to be deleted is stored on the
top of the virtual stack (need to be before the function call) push
into). If the operation is successful, the function will return
``bture``, otherwise it will return ``bfalse``.

This function is used to reset the capacity of the container. The
function prototype is:

.. code:: c

   void be_data_resize(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the container object to be operated. This
function is only available for List containers, and the new capacity is
stored on the top of the virtual stack (must be an integer).

This function is used to get the next element of the iterator. The
function prototype is:

.. code:: c

   int be_iter_next(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the iterator to be operated. The iterator
object may be an iterator of a List container or a Map container. For
the List iterator, this function pushes the iteration result value onto
the top of the stack, while for the Map iterator, it pushes the key
value into the previous position and the top of the stack respectively.
Calling this function will update the iterator. If the function returns
``0``, the call fails, returning ``1`` to indicate that the current
iterator is a List iterator, and returning ``2`` to indicate that the
current iterator is a Map iterator.

This function is used to test whether there is another element in the
iterator. The function prototype is:

.. code:: c

   int map_hasnext(bvm *vm, int index)

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the iterator to be operated. The iterator
object may be an iterator of a List container or a Map container. If
there are more iterable elements in the iterator, return ``1``,
otherwise return ``0``.

This function is used to test whether there is a reference to the
specified object in the reference stack. It must be used in conjunction
with ``be_refpush`` and ``be_refpop``. This API can avoid recursion when
traversing objects that have their own references. The function
prototype is:

.. code:: c

   int be_refcontains(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the object to be operated. This function is
used for the value of an instance type. If there is a reference to the
object in the reference stack, it returns ``1``, otherwise it returns
``0``.

Push the reference of the specified object onto the reference stack. The
function prototype is:

.. code:: c

   int be_refpush(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance;
``index`` is the index of the object to be operated. This function is
used for the value of an instance type.

Pop the object at the top of the reference stack. The function prototype
is:

.. code:: c

   int be_refpop(bvm *vm);

The parameter ``vm`` is the pointer of the virtual machine instance.
This function is used in pairs with ``be_refpush``. The following is the
use of the reference stack API to avoid the problem of infinite
recursive traversal when the object itself is referenced:

.. code:: c

   int list_traversal(bvm *vm)
   {
       // ...
       if (be_refcontains(vm, 1)) {
           be_return(vm);
       }
       be_refpush(vm, 1);
       // Traversing the container, may call list_traversal recursively.
       be_refpop(vm);
       be_return(vm);
   }

This is a simplified traversal process of the List container. For the
complete code, please refer to the source code of the function
``m_tostring`` in *be_listlib.c*. We assume that the index of the List
object is ``1``. First, we check whether the List already exists in the
reference stack (line 4), and if the reference already exists, return
directly, otherwise proceed with subsequent processing. To make
``be_refcontains`` usable, we need to use ``be_refpush`` and
``be_refpop`` to process the reference stack before and after the actual
traversal operation (lines 7 and 9).

This function tests the amount of free space on the stack and expands
the stack space if it is insufficient. The function prototype is:

.. code:: c

   void be_stack_require(bvm *vm, int count);

The parameter ``vm`` is the pointer of the virtual machine instance;
``count`` is the required free stack capacity. If the free capacity of
the virtual stack allocated by the VM to the native function is lower
than this value, an expansion operation will be performed.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is ``nil``, if it is, it returns ``1``,
otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_isnil(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is of type ``bool``, if it is, the
function returns ``1``, otherwise it returns ``0``. The prototype of
this function is:

.. code:: c

   int be_isbool(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is an integer type, if it is, it returns
``1``, otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_isint(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is a real number type, if it is, it
returns ``1``, otherwise it returns ``0``. The prototype of this
function is:

.. code:: c

   int be_isreal(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is an integer or a real number type, if
it is, it returns ``1``, otherwise it returns ``0``. The prototype of
this function is:

.. code:: c

   int be_isnumber(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is a string type, if it is, it returns
``1``, otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_isstring(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is a closure type, if it is, it returns
``1``, otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_isclosure(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is a primitive closure type, if it is, it
returns ``1``, otherwise it returns ``0``. The prototype of this
function is:

.. code:: c

   int be_isntvclos(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is a function type, if it is, it returns
``1``, otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_isfunction(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured. There are three
types of functions: closure, native function and native closure.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is of type ``proto``, if it is, it
returns ``1``, otherwise it returns ``0``. The prototype of this
function is:

.. code:: c

   int be_isproto(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured. ``proto`` The type
is the function prototype of the Berry closure.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is of type ``class``, if it is, it
returns ``1``, otherwise it returns ``0``. The prototype of this
function is:

.. code:: c

   int be_isclass(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is of type ``instance``, if it is, it
returns ``1``, otherwise it returns ``0``. The prototype of this
function is:

.. code:: c

   int be_isinstance(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is an instance or sub-instance of class
``bytes``, if it is, it returns ``1``, otherwise it returns ``0``. The
prototype of this function is:

.. code:: c

   int be_isbytes(bvm *vm, int index);

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is of type ``list``, if it is, it returns
``1``, otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_islist(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is of type ``map``, if it is, it returns
``1``, otherwise it returns ``0``. The prototype of this function is:

.. code:: c

   int be_ismap(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

This function returns whether the value indexed by the parameter
``index`` in the virtual stack is a universal pointer type, if it is, it
returns ``1``, otherwise it returns ``0``. The prototype of this
function is:

.. code:: c

   int be_iscomptr(bvm *vm, int index);

The parameter ``vm`` is the pointer of the virtual machine instance, and
``index`` is the index of the value to be measured.

.. code:: c

   bint be_toint(bvm *vm, int index);

Get the value of the index position of ``index`` from the virtual stack
and return it as an integer type. This function does not check the
correctness of the type. If the value is an instance, the method
``toint()`` is called if it exists.

.. code:: c

   breal be_toreal(bvm *vm, int index);

Get the value of the index position of ``index`` from the virtual stack
and return it as a floating-point number type. This function does not
check the correctness of the type.

.. code:: c

   bint be_toindex(bvm *vm, int index);

Get the value of the index position of ``index`` from the virtual stack
and return it as an integer type. This function does not check the
correctness of the type. Unlike ``be_toint``, the return value type of
``be_toindex`` is ``int``, while the return value of the former is
``bint``.

.. code:: c

   bbool be_tobool(bvm *vm, int index);

Get the value of the index position of ``index`` from the virtual stack
and return it as a Boolean type. If the indexed value is not of Boolean
type, it will be converted according to the rules in section type_bool,
and the conversion process will not cause the indexed value to change.
If the value is an instance, the method ``tobool()`` is called if it
exists.

.. code:: c

   const char* be_tostring(bvm *vm, int index);

Get the value of the index position of ``index`` from the virtual stack
and return it as a string type. If the indexed value is not a string
type, the indexed value will be converted to a string, and the
conversion process will replace the value at the indexed position in the
virtual stack with the converted string. The string returned by this
function always ends with ``’\0’`` characters. If the value is an
instance, the method ``tostring()`` is called if it exists.

.. code:: c

   void* be_tocomptr(bvm* vm, int index);

Get the value of the index position of ``index`` from the virtual stack
and return it as a general pointer type. This function does not check
the correctness of the type.

.. code:: c

   const void* be_tobytes(bvm *vm, int index, size_t *len);

Get the value of the index position of ``index`` from the virtual stack
and return it as a bytes buffer. The pointer of the buffer is returned,
and the size is stored in ``*len`` (unless ``len`` is NULL). This
function works only for instances of the ``bytes`` class, or returns
``NULL``.

.. code:: c

   void be_pushnil(bvm *vm);

Push a ``nil`` value onto the virtual stack.

.. code:: c

   void be_pushbool(bvm *vm, int b);

Push a Boolean value onto the virtual stack. The parameter ``b`` is the
boolean value to be pushed onto the stack. When the value is ``0``, it
means false, otherwise it is true.

.. code:: c

   void be_pushint(bvm *vm, bint i);

Push an integer value ``i`` onto the virtual stack.

.. code:: c

   void be_pushreal(bvm *vm, breal r);

Push a floating point value ``r`` onto the virtual stack.

.. code:: c

   void be_pushstring(bvm *vm, const char *str)

Push the string ``str`` onto the virtual stack. The parameter ``str``
must point to a C string that ends with a null character ``’\0’``, and a
null pointer cannot be passed in.

.. code:: c

   void be_pushnstring(bvm *vm, const char *str, size_t n);

Push the string ``str`` of length ``n`` onto the virtual stack. The
length of the string is subject to the parameter ``n``, and the null
character is not used as the end mark of the string.

.. code:: c

   const char* be_pushfstring(bvm *vm, const char *format, ...);

Push the formatted string into the virtual stack. The parameter
``format`` is a formatted string, which contains the text to be pushed
onto the stack, and the ``format`` parameter contains a label, which can
be replaced by the value specified by the subsequent additional
parameter and formatted as required. According to the label of the
``format`` string, a series of additional parameters may be required,
and each additional parameter will replace the corresponding ``%`` label
in the ``format`` parameter.

.. container::
   :name: tab::format_specifier

   +-------------------------+-----------------------------------------+---+
   | **specifier**           | **Description**                         |   |
   +=========================+=========================================+===+
   | ``d``                   | Format as decimal signed integer        |   |
   |                         | (positive numbers do not output sign)   |   |
   +-------------------------+-----------------------------------------+---+
   | ``f``                   | Single or double precision floating     |   |
   |                         | point number formatted as a decimal     |   |
   +-------------------------+-----------------------------------------+---+
   | ``g``                   | Single or double precision floating     |   |
   |                         | point number formatted as exponential   |   |
   +-------------------------+-----------------------------------------+---+
   | ``s``                   | Format as string                        |   |
   +-------------------------+-----------------------------------------+---+
   | ``c``                   | Format as a single character            |   |
   +-------------------------+-----------------------------------------+---+
   | ``p``                   | Format as pointer address               |   |
   +-------------------------+-----------------------------------------+---+
   | ``%``                   | Escaped as ``%`` Character (no          |   |
   |                         | parameter)                              |   |
   +-------------------------+-----------------------------------------+---+

   ``format`` Label parameter description

``be_pushfstring`` The function is similar to the standard function of C
``printf``, but the function of formatting strings is relatively basic
and does not support operations such as customizing the width and
decimal places. A typical example is:

.. code:: c

   be_pushfstring(vm, "%s: %d", "hello", 12); // Good, it works!
   be_pushfstring(vm, "%s: %.5d", "hello", 12); // Error, the specified width is not supported.

This means that ``be_pushfstring`` can only perform simple formatting
operations. If the requirements cannot be met, it is recommended to use
``sprintf`` formatted strings for operations.

.. code:: c

   void be_pushvalue(bvm *vm, int index);

Push the value with index ``index`` onto the top of the virtual stack.

.. code:: c

   void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);

Push a native closure onto the top of the virtual stack. The parameter
``f`` is the C function pointer of the native closure, and ``nupvals``
is the upvalue number of the closure.

.. code:: c

   void be_pushntvfunction(bvm *vm, bntvfunc f);

Push a native function onto the top of the virtual stack, and the
parameter ``f`` is the native function pointer.

.. code:: c

   void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);

Push a native class onto the top of the virtual stack. The parameter
``name`` is the name of the native class, and the parameter ``lib`` is
the attribute description of the native class.

.. code:: c

   void be_pushcomptr(bvm *vm, void *ptr);

Push a general pointer onto the top of the virtual stack. The general
pointer ``ptr`` points to a certain C data area. Since the content
pointed to by this pointer is not maintained by Berry’s garbage
collector, users have to maintain the life cycle of the data themselves.

.. code:: c

   void* be_pushbytes(bvm *vm, const void *buf, size_t len);

Push a ``bytes()`` buffer starting at position ``buf`` and of size
``len``. The buffer is copied into Berry allocated memory, you don’t
need to keep the buffer valid after this call.

.. code:: c

   bbool be_pushiter(bvm *vm, int index);

Push an iterator onto the top of the virtual stack.

Push an error message onto the top of the stack. After executing the
FFI, the interpreter will directly return to the position that can
handle the error, and the code immediately following will not be
executed. The function prototype is:

.. code:: c

   void be_pusherror(bvm *vm, const char *msg);

The parameter ``vm`` is the pointer of the virtual machine instance;
``msg`` is the string containing the error information.

Move the value at the ``from`` index to the ``to`` index position. This
function does not delete the value of ``from`` index position, only
modifies the value of ``to`` index position.

Compile-time construction technology
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The compile-time construction technology is mainly implemented by *coc*
which is located in the *coc/coc* path of the interpreter source code
directory. *coc* Tool is used to generate constant strings, and constant
objects as C code, and will be compiled into constants when the
interpreter is compiled. In principle, the *coc* tool will generate code
from the declaration information of the constant object (in accordance
with a specific format). The process will automatically calculate the
Hash value and generate the Hash table.

*Makefile* in the root directory of the interpreter project will
automatically compile this tool and run the tool before compiling the
interpreter source code. The content of *Makefile* ensures that when
using the ``make`` command, the code for constructing the object at
compile time will always be updated through the tool (if it needs to be
updated). The code for constructing objects at compile time can be
manually generated through the ``make prebuild`` command, which is
stored in the *generate* folder.

The compile-time construction can be turned on or off by modifying the
``BE_USE_PRECOMPILED_OBJECT`` macro. In any case, the tool *coc* is
called to generate constant object codes (the codes are not used when
compile-time construction is turned off).

Use command
^^^^^^^^^^^

``coc`` Tool is used to generate code for constant objects. The format
of the command is

.. code:: bash

   tools/coc/coc -o <dst_path> <src_path(s)> -c <include_path>

The output path *dst_path* is used to store the generated code, and the
source path *src_path* is a list of paths that need to be scanned for
the source code (use spaces to separate multiple paths).
``include_path`` contains a C header file scanned to detect compilation
directives. ``coc`` tries to compile only the necessary constants. Since
*generate* is used as the generated code path in the source code of the
interpreter, *dst_path* must be *generate*. Taking the standard
interpreter project as an example, the command to use the tool in
``map_build`` should be

.. code:: bash

   tools/coc/coc -o generate default src -c default/berry_conf.h

The meaning of this command is: the output path is *generate*, and the
source path is *src* and *default*.

output path
^^^^^^^^^^^

Strictly speaking, the *generate* folder used as the output path cannot
be placed anywhere, it must be stored in a parent directory containing
the path. The include path refers to the path where the header file will
be searched in the project. Taking the standard interpreter source code
as an example, the include path is *src* and *default*. Therefore, in
the standard interpreter project, the *generate* folder is stored in the
root directory of the interpreter source code (the parent directory of
*src* and *default*).

The reason for the above rules is that the following codes are used in
the interpreter source code to refer to constant objects:

.. code:: c

   #include "../generate/be_fixed_xxx.h"

If readers want to define constant objects themselves, they also need to
use such code to include the corresponding header files. This section
will introduce how to use these tools to define and use constant
objects.

Compile-time string table
~~~~~~~~~~~~~~~~~~~~~~~~~

The compile-time string table is used to store constant strings.
Constant strings are objects that are transparent to the script. They
are not created or destroyed when the interpreter is running, but are
always stored as constants in the data segment of the interpreter
program. If you need to use a string as a constant string, you can add
the prefix ``be_const_str_`` in front of the string in the interpreter
source code, and the declaration can be placed anywhere in the source
file (including comments). For example, to create a constant string with
the content ``"string"``, you need to declare the symbol
``be_const_str_string`` in the source file, and this symbol is also the
variable name that refers to the constant string in the C code.

All keywords will create constant strings. If you modify the
keyword-related code in the Berry interpreter, the corresponding code in
*coc* must also be modified.

If the string contains special symbols, they are automatically escaped
as ``_XHH`` where ``HH`` is the hex representation (uppercase) of the
character. For example ``"`` is represented by ``_X3A``. This
representation is bihective so it’s easy to convert to and from the
original string.

Use constant string
^^^^^^^^^^^^^^^^^^^

Normally, there is no need to manually declare constant strings, nor to
use them manually. If you really need to call the constant string
manually, include the header file *be_constobj.h* to use all constant
string variables (this header file has declarations for all constant
strings). The typical use of constant strings is to construct objects at
compile time. The declaration and definition of constant strings in this
process are automatically handled by the tool.

In any case, the FFI function ``be_pushstring`` should be used directly
to create a string. When a string has a constant string, it will not
repeatedly create a new string object, but directly use the
corresponding constant string.

By default, all strings used are referenced in a global
``m_const_string_table`` hashtable. However, some projects may have many
compilation variants for which some sets of string are not needed. If
all string constants are stored in all variants, this creates a waste of
flash size. For this reason, some strings can be declared as ``weak``
strings in the sense of having a ``weak`` reference. In such case the
string constant is declared in C code, but not included in the global
map object. This means that the linker can choose to not include the
string constants if it is not referenced by any code. The con is that if
you dynamically create a string object with the same value, a new object
is created in memory (while it would not for a regular string constant).
To indicate weak strings, use the ``strings: weak`` modifier (see
below).

Construct object at compile time
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Objects constructed at compile time are also called constant objects.
The data structure of these objects is constructed when the interpreter
is compiled and cannot be modified at runtime. ``map_build`` A set of
declaration rules is defined in the tool to generate C code for constant
objects. The declaration information of the constant object is directly
stored in the source file (*\*.c*). In order to distinguish it from
other content, a complete declaration information should be included in
the following boot code:

::

   @const_object_info_begin
   @const_object_info_end

The constant object declaration information does not conform to the C
language syntax, so they should be placed in a multi-line comment
(included with ``/* */``). All constant objects have the same
declaration form. The declaration structure of a constant object is
called “object declaration block”, which is composed of

::

   type object_name (attributes) {
       member_fields
   }

``type`` is the type of constant object, it can be ``map``, ``class``,
``module`` or ``vartab``. ``object_name`` is the variable name of the
constant object in C language. ``attributes`` is the attribute list of
constant objects. An attribute is composed of attribute name and
attribute value. The attribute name and attribute value are separated by
semicolons, and multiple attributes are separated by commas. For
example, the attribute list ``scope: global, name: map`` means that the
``scope`` attribute of a constant object is ``global``, and the ``name``
attribute is ``map``. Also ``strings: weak`` indicate to generate weak
string constants for the names of member fields or any string constant.
``member_fields`` is the list of member domains of constant objects. A
member is composed of name and value, separated by commas. Each line can
declare one member, and multiple members must be declared on multiple
lines.

The **coc** tool uses regular expressions to parse the object
declaration block. In the parsing process, the entire object declaration
block will be matched first, and the information ``type`` and
``object_name`` will be matched. For the information of ``attributes``
and ``member_fields``, further Analysis. In order to facilitate the
implementation, the tool does not have strict requirements on the syntax
of the object declaration block, and lacks a complete error handling
mechanism, so you should ensure that the syntax is correct when writing
the object declaration block.

In order to facilitate understanding, we illustrate with a simple
constant class:

.. code:: c

   /* @const_object_info_begin
   class be_class_map (scope: global, name: map) {
       .data, var
       init, func(m_init)
       tostring, func(m_tostring)
   }
   @const_object_info_end */
   #include "../generate/be_fixed_be_class_map.h"

In this example, the declaration information of the entire constant
class is in the C language comment, so it will not affect the
compilation of the C code. The object declaration block is placed
between ``@const_object_info_begin`` and ``@const_object_info_end`` to
ensure that the **coc** tool detects the object declaration block.

Since it is a constant class declaration, the value of *type* in the
object declaration block is ``class``, and ``be_class_map`` is the
variable name of the constant object in the C code. Two attributes are
declared in the attribute list of the object (the part enclosed in
parentheses), and the meaning of these attributes will be introduced in
the “Compile-Time Construction Class” section of this section. Three
members are defined in the member list surrounded by curly braces,
multiple members are separated by newlines, and the name of the member
and the value of the member are separated by a comma.There are several
legal formats for member names:

-  Berry variable name format: start with a letter or underscore,
   followed by several letters, underscores or numbers.

-  Use “``.``” as the first character, followed by letters, underscores
   or numbers.

-  Overloadable operators, such as “``+``”, “``-``” and “``<<``” etc.

The value of a member can be of the following types:

-  ``var``: This symbol will be compiled into an integer object
   (``be_const_var``), and the value of the integer object is
   automatically incremented from ``0``. ``var`` It is designed for the
   declaration of member variables in the class, and its automatic
   numbering feature is used to realize the serial number of member
   variables.

-  ``func(symbol)``: Declare native member functions or methods of
   constant objects. The symbol will be compiled into a native function
   value (``be_const_func``), ``symbol`` is the native function pointer
   corresponding to the member. ``m_init`` and ``m_tostring`` in the
   example are two native functions.

-  ``closure(symbol)``: Declare pre-compiled bytecode member functions
   or methods of constant objects. The symbol will be compiled into a
   native function value (``be_const_closure``), ``symbol`` is the name
   of the solidified function. See ``module solidify`` to know how to
   solidify objects.

-  ``nil()``: This symbol will be compiled into an nil value
   (``be_const_nil``).

-  ``int(value)``: This symbol will be compiled into an integer object
   (``be_const_int``), the value of the integer object is ``value``.

-  ``real(value)``: This symbol will be compiled into a real number
   object (``be_const_real``), the value of the real number object is
   ``value``.

-  ``comptr(value)``: This symbol will be compiled into a pointer object
   (``be_const_comptr``), the value of the pointer is ``value`` and can
   be used to pass the address of a C global structure.

-  ``class(symbol)``: This symbol will be compiled into a class object
   (``be_const_class``). ``symbol`` is a pointer to this type of object,
   and the pointer needs to point to a constant type object.

-  ``module(symbol)``: This symbol will be compiled into a module object
   (``be_const_module``). ``symbol`` is a pointer to the module object,
   and the pointer needs to point to a constant module object.

-  ``ctype_func(symbol)``: This symbol will be compiled into a native
   function (``be_const_ctype_func``). ``symbol`` is a pointer to the C
   mapping definition. This feature is used by
   `berry_mapping <https://github.com/berry-lang/berry_mapping>`__

In order to use the ``be_class_map`` object, we need to include the
corresponding header file in the C code to ensure that the object will
be compiled. The usual practice is to include the corresponding header
file near the object declaration block. In the example, line 8 contains
it. The corresponding header file can be used to construct
``be_class_map`` objects at compile time.

After processing by the **coc** tool , each object declaration block
will be compiled into a header file named *be_fixed_be_xxx.h*, and *xxx*
is the C variable name of the object. In order to compile constant
objects in C code, we need to include the corresponding header files. It
is usually recommended to include the corresponding header files near
the object declaration block. The 8th line in the example contains
*be_fixed_be_class_map.h* to construct the ``be_class_map`` object at
compile time.

Construct Map at Compile Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Maps constructed at compile-time are also constant ``map`` objects. They
are generally not declared directly using object declaration blocks, but
are declared in other compile-time construction structures. When
constructing the constant ``map``, the constant object type information
should be ``map``, which supports a ``scope`` attribute. When the
``scope`` attribute value is ``local``, the constant object is
``static``, the attribute When it is ``global``, it is ``extern``, and
the value of this attribute is ``local`` by default. The constant
``map`` object’s ``member_fields`` supports common member name/member
value specifications, and member values are only stored as data without
special interpretation. The following is an example of using the object
declaration block to directly declare a constant ``map`` object:

::

   map map_name (scope: local/global) {
       init, func(m_init)
   }

Compile-time construction class
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To construct a class at compile time, use the object declaration block
to declare, and the type information of the object is ``class``. The
declared properties of this object are ``scope`` and ``name``. ``scope``
The scope of the C variable of the attribute declaration object, when
the value is ``local`` (default), the scope is ``static``, when it is
``global``, the scope is ``extern``; ``name`` The value of the attribute
is that class Name, anonymous class can omit this parameter. Since the
attribute list of a class only stores methods and member variable
indexes, the ``member_fields`` of the class constructed at compile time
can only use the values ``var`` and ``func()``. A simple compile-time
construction class declaration block is:

::

   class be_class_map (scope: global, name: map) {
       .data, var
       init, func(m_init)
       tostring, func(m_tostring)
   }

Building Module at Compile Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The type information of the building block declaration block at compile
time is ``module``.

.. code:: c

   module math (scope: global) {
       sin, func(m_sin)
       cos, func(m_cos)
       pi, real(M_PI)
   }

Construct Built-in Domain at Compile Time
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: c

   vartab m_builtin (scope: local) {
       assert, func(l_assert)
       print, func(l_print)
       list, class(be_class_list)
   }
