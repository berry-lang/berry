2. Types and Variables
======================

**Type** is an attribute of data, which defines the meaning of the data
and the operations that can be performed on the data. Types can be
divided into built-in types and user-defined types. Built-in types refer
to some basic types built into the Berry language, among which types
that are not based on class definitions are called **Simple type**.
Types based on class definitions are called **Class type**, some of the
built-in types are class types, and user-defined types are also class
types.

2.1 Built-in type
-----------------

2.1.1 Simple Type
~~~~~~~~~~~~~~~~~

``nil``
^^^^^^^

The Nil type is the null type, which means that the object has an
invalid value, or it can be said that the object has no meaningful
value. This is a very special type. Although we might say that a
variable is ``nil``, in fact the nil type has no value, so what we are
talking about here is that the type of the variable is nil (not a
value).

The default value of a variable before assignment is ``nil``. This type
can be used in logic operations. In logic operations, ``nil`` is
equivalent to ``false``.

Integer type
^^^^^^^^^^^^

The integer type (integer) represents a signed integer, referred to as
an integer. The number of bits of the integer represented by this type
depends on the specific implementation, and usually consists of a 32-bit
signed integer on a 32-bit platform. Integer is an arithmetic type and
supports all arithmetic operations. Pay attention to the value range of
integers when using this type. The typical value range of 32-bit signed
integers is between  −2147483648 and 2147483647.

Any value can be converted to ``int`` using the ``int()`` function;
however ``int(nil) == nil``. If the argument is an instance and if it
contains a member ``toint()`` it we be called and the return value
converted to ``int``.

Real Number Type
^^^^^^^^^^^^^^^^

The real type (real), to be precise, is a floating-point type. Real
number types are usually implemented as single-precision floating-point
numbers or double-precision floating-point numbers. The real number type
is also an arithmetic type. Compared with the integer type, the real
number type has higher precision and a larger value range, so this type
is more suitable for mathematical calculations. It should be noted that
the real number type is actually a floating point number, so there are
still precision problems. For example, it is not recommended to compare
two values of type ``real`` for equality.

When integers and real numbers participate in operations at the same
time, the integers are usually converted to real numbers.

Boolean type
^^^^^^^^^^^^

The Boolean type (boolean) is used for logical operations. It has two
values ``true`` and ``false``, which represent the two true values (true
and false) in logic and Boolean algebra. The Boolean type is mainly used
for conditional judgment. The operands and return values of logical
expressions and relational expressions are all boolean types, and
statements such as ``if`` and ``while`` all use boolean types as
conditional checks.

In many cases, non-boolean values can also be used as boolean types.
This is because the interpreter will implicitly convert the parameters.
This is also the reason that conditional check expressions such as
``if`` statements can use any type of parameters. The rules for
converting various types to Boolean types are:

-  ``nil``: converted to ``false``.

-  **Integer**: when the value is ``0``, it is converted to ``false``,
   otherwise it is converted to ``true``.

-  **Real number**: when the value is ``0.0``, it is converted to
   ``false``, otherwise it is converted to ``true``.

-  **String**: when the value is "" (empty string) it is concerted to
   ``false`` otherwise it is converted to ``true``.

-  **Comobj** and **Comptr**: when the internal pointer is ``NULL`` it
   is converted to ``false``, otherwise it is converted to ``true``.

-  **Instance**: if the instance contains a method ``tobool()``, the
   return value of the method will be used, otherwise it will be
   converted to ``true``.

-  All other types: convert to ``true``.

Any value can be converted to ``bool`` using ``bool()`` function.

String
^^^^^^

A string is a sequence of characters. In terms of storage, Berry divides
strings into long strings and short strings. There is only one instance
of the same short character string in memory, and all short character
strings are linked in a hash table. This design helps to improve the
performance of string equality comparison and can reduce memory usage.
Since the use frequency of long strings is low, and the overhead of hash
operation is quite high, they are not linked to the hash table, so there
may be multiple identical instances in the memory. The string is
read-only after it is created. Therefore, “modifying” the string will
generate a new string, and the original string will not be modified.

Berry does not care about the format or encoding of characters. For
example, the string ``’abc’`` is actually the ASCII code of the
characters ``’a’``, ``’b’`` and ``’c’``. Therefore, if there are wide
characters in the string (the character length is greater than 1 byte),
the number of characters in the string cannot be directly counted. In
fact, using the ``size()`` function can only get the number of bytes in
the string. In addition, in order to facilitate interaction with the C
language, Berry’s string always ends with ``’\0’`` characters. This
feature is transparent to the Berry program.

The string type can be compared in size, so it can be used in relational
operations.

Function
^^^^^^^^

A function is a piece of code that is encapsulated and available for
call, generally used to implement a specific function. Function is
actually a big category, which includes several subtypes such as
closures, native functions, and native closures. For Berry code, all
function subtypes have the same behavior. Functions belong to the first
type of value in Berry, so they can be passed as values. In addition, it
can be directly used in expressions through the “literal” form of
“anonymous functions”.

A function is a read-only object and cannot be modified once defined.
You can compare whether two functions are equal (whether they are the
same function), but the function type cannot be compared.\ **Native
function** (native function) and **Native closure** (native closure)
refer to functions and closures implemented in C language. One of the
main purposes of native functions and native closures is to provide
functions that the Berry language does not provide, such as IO
operations and low-level operations. If a piece of code is used
frequently and has performance requirements, it is also recommended to
rewrite it as a native function or native closure.

Class
^^^^^

In object-oriented programming, a class is an extensible program code
template. Classes are used to create instance objects, so the class can
be said to be the “type” of the instance. All instance objects are of
type ``instance`` and they all have a corresponding class, which is
called instance **Class type**. To put it simply, a class is a value
representing the type of an instance object, and a class is an
abstraction of the characteristics of an instance. A class is also a
read-only object, once defined, it cannot be modified.

Classes can only compare equals and unequals, but cannot compare sizes.

Examples
^^^^^^^^

An instance is a materialized object generated by a class, and the
process of generating an instance from a class is called
``Instantiate``. In object-oriented programming, “instance” is usually
synonymous with “object”. However, in order to distinguish from
non-instance objects, we do not use the term “object” alone, but use
“instance” or “instance object”. Berry instances are always allocated
dynamically and need to be used with a garbage collector. In addition to
memory allocation, the process of instantiation also needs to initialize
the instance, this process is completed by ``Constructor``. In addition,
you can complete the destruction of the object through ``Destructor``
before reclaiming the object’s memory.

In the internal implementation, the instance will contain a reference to
the class, and the instance itself only stores member variables and not
methods.

2.1.2 Class Type
~~~~~~~~~~~~~~~~

Some of the built-in types are class types, they are ``list``, ``map``
and ``range``. Unlike custom types, built-in class types can be
constructed using literals, for example ``[1, 2, 3]`` is a literal of
type ``list``.

List
^^^^

The List class is a container that provides support for list data types.
Berry’s list is an ordered collection of elements, and each element in
the list has a unique integer index, and each element can be accessed
directly according to the index. List supports inserting or deleting
elements at any position, and the element can be of any type. In
addition to using indexes, you can also use iterators to access elements
in the list.

The implementation of List is a dynamic array, and this data structure
has good random access performance. The efficiency of adding and
deleting elements at the end of the list is very high, but the
efficiency of adding and deleting elements in the middle of the list is
low.

The literal initialization method of the List container is to use a list
of objects surrounded by square brackets, and multiple objects are
separated by commas, for example:

.. code:: berry

   []
   ['string']
   [0, 1, 2,'list']

Operations: see chapter 7.

Map
^^^

Map is also a kind of container, map is a collection of key-value pairs,
and each possible key appears at most once in the collection. The Map
container provides the following basic operations:

-  Add key-value pairs to the collection

-  Remove key-value pairs from the collection

-  Modify the value corresponding to an existing key

-  Find the corresponding value by key

Map is implemented using a hash table and has high search efficiency.
The operation of adding and deleting key-value pairs will consume more
time if “re-hashing” occurs.

The Map container can also be initialized using literal values, written
in curly braces to enclose a list of key-value pairs, separated by
colons between keys and values, and separated by commas between
key-value pairs. E.g:

.. code:: berry

   {}
   {'str':'hello'}
   {'str':'hello','int': 45, 78: nil}

Operations: see chapter 7.

Range
^^^^^

The Range container represents an integer range, which is usually used
to iterate in an integer range. This type has a ``__lower__`` member and
``__upper__`` member, which represent the lower and upper bounds of the
range, respectively. The literal value of Range is a pair of integers
connected using the ``..`` operator:

.. code:: berry

   0 .. 10
   -5 .. 5

When the Range class is used for iteration, the elements of the
iteration are all integer values from the lower bound to the upper
bound, including boundary values. For example, the iteration result of
``0..5`` is:

.. code:: berry

   0 1 2 3 4 5

Therefore, it should be noted that for a range of *x* .. (*x*\ +\ *n*),
the number of iterations is *n* + 1. A common construct to iterate
through elements of a list by item is:

.. code:: berry

   for i: 0..size(l)-1

Open range: if you omit the last range, it is implicitly replaced with
MAXINT.

.. code:: berry

   > r = 10..
   > r
   (10..9223372036854775807)

Bytes
^^^^^

Bytes object denote a bytes buffer which can be used to manipulate bytes
buffers or to read/write some C memory areas or structures.

See Chapter 7.

2.2 Variables
-------------

A variable is a storage space with a name, and the data or information
stored in the storage space is called the value of the variable.
Variable names are used to refer to variables in source code. In
different scopes, a variable name can bind multiple independent
variables, but variables have no aliases. The value of the variable can
be accessed or changed at any time during the running of the program.
Berry is a dynamically typed language, so the type of variable value is
determined at runtime, and the variable can store any type of value.

2.2.1 define variables
~~~~~~~~~~~~~~~~~~~~~~

The first way to define a variable is to use an assignment statement to
assign a value to a new variable name:

.. code::

   ’var’ = expression

**variable** is the name of the variable, and the variable name is an
identifier (see section identifier). **expression** is the expression to
initialize the variable.

.. code:: berry

   a = 1
   b ='str'

However, this method of defining variables has some limitations. Take
the following code as an example:

.. code:: berry

   i = 0
   do
       i = 1
       print(i) # 1
   end
   print(i) # 1

The ``do`` statement in the routine constitutes the inner scope. We
modified the value of the variable ``i`` at line 3, and the value of
``i`` is still ``1`` after leaving the inner scope at line 6 . If we
want the variable ``i`` of the inner scope to be an independent
variable, the method of defining the variable by directly assigning to
the new variable name cannot meet the requirement, because the
identifier ``i`` already exists in the outer scope. In this case, the
variable can be defined by the ``var`` keyword:

.. code::

   ’var’ variable
   ’var’ variable = expression

There are two ways of using ``var`` to define a variable: The first is
to follow the variable name **variable** after the keyword ``var``, in
this case the variable will be initialized to ``nil``, and the other is
written in The variable is initialized at the same time as the variable
is defined. In this case, an initial value expression **expression** is
required. Using ``var`` to define a variable has two possible results:
if the current scope does not define the variable of **variable**,
define and initialize the variable, otherwise it is equivalent to
reinitialize the variable. Therefore, the variable defined with ``var``
will shield the variable with the same name in the outer scope.

Now we change the previous example to use the ``var`` keyword to define
variables:

.. code:: berry

   i = 0
   do
       var i = 1
       print(i) # 1
   end
   print(i) # 0

From the modified routine, it can be found that the value of the
variable ``i`` in the inner scope is ``1``, and its value in the outer
scope is ``0``. This proves that after using the ``var`` keyword, a new
variable ``i`` is defined in the inner scope and the variable with the
same name in the outer scope is blocked. After the inner scope ends, the
identifier ``i`` is once again bound to the variable ``i`` in the outer
scope.

When using the ``var`` keyword to define a variable, you can also use a
list of multiple variable names, separated by commas. You can also
initialize one or more variables when defining variables:

.. code:: berry

   var a = 0, b, c ='test'

2.2.2 Scope and Life Cycle
~~~~~~~~~~~~~~~~~~~~~~~~~~

As mentioned earlier, variable names can be bound to multiple variable
entities (storage spaces), and variable names are bound to only one
entity at each position. The entity bound by the variable name needs to
be determined according to the position where the variable name appears.

**Scope** refers to the code area where the name and the entity are
uniquely bound. Outside the scope, the name may be bound to other
entities, or not bound to any entity. The entity is only visible in the
scope bound to the name, that is, the variable is only valid in its
scope.A code block (see block) is a scope. A variable is only available
inside the block, and names in different blocks may bind different
variable entities. The following example demonstrates the scope of
variables:

.. code:: berry

   var i = 0
   do
       var j ='str'
       print(i, j) # 0 str
   end
   # The variable j is not available here
   print(i) # 0

The names ``i`` and ``j`` are defined in this routine. The name ``i`` is
defined outside the ``do`` sentence, and the name defined in the
outermost block has **Global scope** (global scope). The name with
global scope is available in the entire program after customization. The
name ``j`` is defined in the block in the ``do`` sentence, and the name
of this type of definition in the non-outermost block has **Local
scope** (local scope). A name with a local scope cannot be accessed
outside the scope.

Berry has some built-in objects, which are all in the global scope.
However, built-in objects and global variables defined in scripts are
not in the same global scope. Built-in objects actually belong to
**Built-in scope** (built-in scope). The scope is globally visible as
the ordinary global scope, but can be covered by the ordinary global
scope. Built-in objects include functions and classes in the standard
library. These objects include ``print`` functions, ``type`` functions,
and ``map`` classes. Different from other scopes, the variables in the
built-in scope are read-only, so “assignment” to the variables in the
built-in scope actually defines a variable with the same name in the
global scope, which overrides The symbols in the built-in scope.

nested scope
^^^^^^^^^^^^

Nested scope means that the scope contains another scope. We call the
contained scope **Inner scope**, and the scope that contains the inner
scope **Outer scope**. The name defined in the outer scope can be
accessed in all inner scopes. The inner scope can also rebind the name
already defined in the outer scope. The previous example using ``var``
to define variables describes this scenario.

Variable Life Cycle
^^^^^^^^^^^^^^^^^^^

There is no concept of variable names when the program is running, and
variables exist in the form of entities at this time. The “validity
period” of a variable during program execution is the variable’s **Life
cycle**. Variables at runtime are only valid within the scope. After
leaving the scope, the variables will be destroyed to reclaim resources.

Variables defined in the global scope are called **Global variable** and
have **Static life cycle**. Such variables can be accessed during the
entire program running and will not be destroyed. Variables defined in
the local scope are called **Local variable** and have **Dynamic life
cycle**. Such variables cannot be accessed after leaving the scope and
will be destroyed.

Due to the different life cycles, local variables and global variables
use different ways to allocate storage space. Local variables are
allocated on a structure called **Stack** (stack), and objects allocated
based on the stack can be quickly reclaimed at the end of the scope.
Global variables are allocated in **Global table** (global table).
Objects in the global table will not be recycled once they are created,
and the table can be accessed anywhere in the program.

2.2.3 Type of variable
~~~~~~~~~~~~~~~~~~~~~~

Berry determines the type of the variable at runtime. In other words,
the variable can store any type of value. Therefore Berry is a **Dynamic
typing** language. The interpreter does not deduce the type of the
variable at compile time, which may cause some errors to be exposed at
runtime. For example, the error generated by executing the expression
``’1’ + 1`` is a runtime error rather than a compiler error. The
advantage of using dynamic types is that many designs can be simplified,
and the program will be more flexible, not to mention the need to design
a complex type inference system.

Due to the lack of type checking by the interpreter, user code may need
to determine the type of value by itself, and this feature can also be
used to implement some special operations. This feature also makes
overloaded functions unnecessary. For example, the native function
``type`` accepts any type of parameter and returns a string describing
the parameter type.
