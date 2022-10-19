7. Libraries and Modules
========================

7.1 Basic library
-----------------

There are some functions and classes that can be used directly in the
standard library. They provide basic services for Berry programs, so
they are also called basic libraries. The functions and classes in the
basic library are visible in the global scope (belonging to the built-in
scope), so they can be used anywhere. Do not define variables with the
same name as the functions or classes in the base library. Doing so will
make it impossible to reference the functions and classes in the base
library.

7.1.1 Built-in function
~~~~~~~~~~~~~~~~~~~~~~~

``print`` function
^^^^^^^^^^^^^^^^^^

**Example**

.. code:: python

   print(...)

**Description**

This function prints the input parameters to the standard output device.
The function can accept any type and any number of parameters. All types
will print their value directly, and for an instance, this function will
check whether the instance has a ``tostring()`` method, and if there is,
print the return value of the instance calling the ``tostring()``
method, otherwise it will print the address of the instance.

.. code:: python

   print('Hello World!') # Hello World!
   print([1, 2, '3']) # [1, 2, '3']
   print(print) # <function: 0x561092293780>

``input`` function
^^^^^^^^^^^^^^^^^^

**Example**

.. code:: python

   input()
   input(prompt)

**Description**

``input`` The function is used to input a line of character string from
the standard input device. This function can use the ``prompt``
parameter as an input prompt, and the ``prompt`` parameter must be of
string type. After calling the ``input`` function, characters will be
read from the keyboard buffer until a newline character is encountered.

.. code:: python

   input('please enter a string:') # please enter a string:

``input`` The function does not return until the “Enter” key is pressed,
so the program “stuck” is not an error.

``type`` function
^^^^^^^^^^^^^^^^^

**Example**

.. code:: python

   type(value)

-  *value*: Input parameter (expect to get its type).

-  *return value*: A string describing the parameter type.

**Description**

This function receives a parameter of any type and returns the type of
the parameter. The return value is a string describing the type of the
parameter. Table below shows the return values corresponding to the main
parameter types:

============== ============== ============== ==============
Parameter Type return value   Parameter Type return value
============== ============== ============== ==============
Nil            ``’nil’``      Integer        ``’int’``
Real           ``’real’``     Boolean        ``’bool’``
Function       ``’function’`` Class          ``’class’``
String         ``’string’``   Instance       ``’instance’``
native pointer ``’ptr’``                     
============== ============== ============== ==============

.. code:: python

   type(0) #'int'
   type(0.5) #'real'
   type('hello') #'string'
   type(print) #'function'

``classname`` function
^^^^^^^^^^^^^^^^^^^^^^

**Example**

.. code:: python

   classname(object)

**Description**

This function returns the class name (string) of the parameter.
Therefore the parameter must be a class or instance, and other types of
parameters will return ``nil``.

.. code:: python

   classname(list) #'list'
   classname(list()) #'list'
   classname({}) #'map'
   classname(0) # nil

``classof`` function
^^^^^^^^^^^^^^^^^^^^

**Example**

.. code:: python

   classof(object)

**Description**

Returns the class of an instance object. The parameter ``object`` must
be an instance. If the function is successfully called, it will return
the class to which the instance belongs, otherwise it will return
``nil``.

.. code:: python

   classof(list) # nil
   classof(list()) # <class: list>
   classof({}) # <class: map>
   classof(0) # nil

``str`` function
^^^^^^^^^^^^^^^^

**Example**

.. code:: python

   str(value)

**Description**

This function converts the parameters into strings and returns. ``str``
Functions can accept any type of parameters and convert them. When the
parameter type is an instance, it will check whether the instance has a
``tostring()`` method, if there is, the return value of the method will
be used, otherwise the address of the instance will be converted into a
string.

.. code:: python

   str(0) # '0'
   str(nil) #'nil'
   str(list) #'list'
   str([0, 1, 2]) #'[0, 1, 2]'

``number`` function
'''''''''''''''''''

.. code:: python

   number(value)

**Description**

This function converts the input string or number into a numeric type
and returns. If the input parameter is an integer or real number, it
returns directly. If it is a character string, try to convert the
character string to a numeric value in decimal format. The integer or
real number will be automatically judged during the conversion. Other
types return ``nil``.

**Example**

.. code:: python

   number(5) # 5
   number('45.6') # 45.6
   number('50') # 50
   number(list) # nil

``int`` function
''''''''''''''''

.. code:: python

   int(value)

**Description**

This function converts the input string or number into an integer and
returns it. If the input parameter is an integer, return directly, if it
is a real number, discard the decimal part. If it is a string, try to
convert the string to an integer in decimal. Other types return ``nil``.
When the parameter type is an instance, it will check whether the
instance has a ``toint()`` method, if there is, the return value of the
method will be used.

**Example**

.. code:: python

   int(5) # 5
   int(45.6) # 45
   int('50') # 50
   int('0x10) # 16 - hex literal are valid
   int(list) # nil

``real`` function
'''''''''''''''''

.. code:: python

   real(value)

**Description**

This function converts the input string or number into a real number and
returns. If the input parameter is a real number, it will return
directly, if it is an integer, it will be converted to a real number. If
it is a string, try to convert the string to a real number in decimal.
Other types return ``nil``.

**Example**

.. code:: python

   real(5) # 5, type(real(5)) →'real'
   real(45.6) # 45.6
   real('50.5') # 50.5
   real(list) # nil

``bool`` function
'''''''''''''''''

.. code:: python

   bool(value)

**Description**

This function converts the input string or number into a boolean and
returns it.

The conversion follows the following rules:

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

**Example**

.. code:: python

   bool() # false
   bool(nil) # false
   bool(false) # false
   bool(true) # true
   bool(0) # false
   bool(1) # true
   bool("") # false
   bool("a") # true
   bool(3.5) # true
   bool(list) # true
   bool([]) # true
   bool({}) # true
   # advanced
   import introspect
   bool(introspect.toptr(0)) # false
   bool(introspect.toptr(0x1000)) # true

``size`` function
'''''''''''''''''

.. code:: python

   size(value)

**Description**

This function returns the size of the input string. If the input
parameter is not a string, 0 is returned. The length of the string is
calculated in bytes. This function also works for ``list`` and ``map``
instances and returns the number of elements.

**Example**

.. code:: python

   size(10) # 0
   size('s') # 1
   size('string') # 6
   size([1,2]) # 2
   size({"a":1}) # 1

``super`` function
''''''''''''''''''

.. code:: python

   super(object)

**Description**

This function returns the parent object of the instance. When you
instantiate a derived class, it will also instantiate its base class.
The ``super`` function is required to access the instance of the base
class (that is, the parent object).

Please look at chapter 6 about magic behavior of ``super(object)`` when
calling a super method.

**Example**

.. code:: python

   class mylist: list end
   l = mylist() # classname(l) -->'mylist'
   sl = super(l) # classname(sl) -->'list'

``assert`` function
'''''''''''''''''''

.. code:: python

   assert(expression)
   assert(expression, message)

**Description**

This function is used to implement the assertion function. ``assert``
The function accepts a parameter. When the value of the parameter is
``false`` or ``nil``, the function will trigger an assertion error,
otherwise the function will not have any effect. It should be noted that
even if the parameter is a value equivalent to ``false`` in logical
operations (for example, ``0``), it will not trigger an assertion error.
The parameter ``message`` is optional and must be a string. If this
parameter is used, the text information given in ``message`` will be
output when an assertion error occurs, otherwise the default
“``Assert Failed``” message will be output.

**Example**

.. code:: python

   assert(false) # assert failed!
   assert(nil) # assert failed!
   assert() # assert failed!
   assert(0) # assert failed!
   assert(false,'user assert message.') # user assert message.
   assert(true) # pass

``compile`` function
''''''''''''''''''''

.. code:: python

   compile(string)
   compile(string, 'string')
   compile(filename, 'file')

**Description**

This function compiles the Berry source code into a function. The source
code can be a string or a text file. ``compile`` The first parameter of
the function is a string, and the second parameter is a string
``’string’`` or ``’file’``. When the second parameter is ``’string’`` or
there is no second parameter, the ``compile`` function will compile the
first parameter as the source code. When the second parameter is
``’file’``, the ``compile`` function will compile the file corresponding
to the first parameter. If the compilation is successful, ``compile``
will return the compiled function, otherwise it will return ``nil``.

**Example**

.. code:: python

   compile('print(\'Hello World!\')')() # Hello World!
   compile('test.be','file')

``list`` Class
~~~~~~~~~~~~~~

``list`` is a built-in type, which is a sequential storage container
that supports subscript reading and writing. ``list`` Similar to arrays
in other programming languages. Obtaining an instance of the ``list``
class can be constructed using a pair of square brackets: ``[]`` will
generate an empty ``list`` instance, and ``[expr, expr, ...]`` will
generate a ``list`` instance with several elements. It can also be
instantiated by calling the ``list`` class: executing ``list()`` will
get an empty ``list`` instance, and ``list(expr, expr, ...)`` will
return an instance with several elements.

``list`` method (Constructor)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Initialize the ``list`` container. This method can accept 0 to multiple
parameters. The ``list`` instance generated when multiple parameters are
passed will have these parameters as elements, and the arrangement order
of the elements is consistent with the arrangement order of the
parameters.

``tostring`` method
^^^^^^^^^^^^^^^^^^^

Serialize the ``list`` instance to a string and return it. For example,
the result of executing ``[1, [], 1.5].tostring()`` is
``’[1, [], 1.5]’``. If the ``list`` container refers to itself, the
corresponding position will use an ellipsis instead of the specific
value:

.. code:: python

   l = [1, 2]
   l[0] = l
   print(l) # [[...], 2]

``concat`` method
^^^^^^^^^^^^^^^^^

Converts each element of the list to strings, and concatenate using the
provided string.

.. code:: python

   l = [1, 2, 3]
   l.concat()  # '123'
   l.concat(", ")  # '1, 2, 3'

``push`` method
^^^^^^^^^^^^^^^

Append an element to the end of the ``list`` container. The prototype of
this method is ``push(value)``, the parameter ``value`` is the value to
be appended, and the appended value is stored at the end of the ``list``
container. The append operation increases the number of elements in the
``list`` container by 1. You can append any type of value to the
``list`` instance.

``insert`` method
^^^^^^^^^^^^^^^^^

Insert an element at the specified position of the ``list`` container.
The prototype of this method is ``insert(index, value)``, the parameter
``index`` is the position to be inserted, and ``value`` is the value to
be inserted. After inserting an element at the position ``index``, all
the elements that originally started from this position will move
backward by one element. The insert operation increases the number of
elements in the ``list`` container by 1. You can insert any type of
value into the ``list`` container.

Suppose that the value of a ``list`` instance ``l`` is ``[0, 1, 2]``,
and we insert a string ``’string’`` at position 1, and we need to call
``l.insert(1, ’string’)``. Finally, the new ``list`` value is
``[0, ’string’, 1, 2]``.

If the number of elements in a ``list`` container is *S*, the value
range of the insertion position is {*i* ∈ ℤ :  − *S* ≤ *i* < *S*}. When
the insertion position is positive, index backward from the head of the
``list`` container, otherwise index forward from the end of the ``list``
container.

``remove`` method
^^^^^^^^^^^^^^^^^

Remove an element from the container. The prototype of this method is
``remove(index)``, and the parameter ``index`` is the position of the
element to be removed. After the element is removed, the element behind
the removed element will move forward by one element, and the number of
elements in the container will be reduced by 1. Like the ``insert``
method, the ``remove`` method can also use positive or negative indexes.

``item`` method
^^^^^^^^^^^^^^^

Get an element in the ``list`` container. The prototype of this method
is ``item(index)``, the parameter ``index`` is the index of the element
to be obtained, and the return value of the method is the element at the
index position. ``list`` The container supports multiple indexing
methods:

-  Integer index: The index value can be a positive integer or a
   negative integer. If the index is negative, it is relative to the end
   of the list; i.e. ``-1`` indicates the last element in the list. The
   return value of ``item`` is the element at the index position. If the
   index position exceeds the number of elements in the container or is
   before the 0th element, the ``item`` method will return ``nil``.

-  ``list`` Index: Using a list of integers as an index, ``item``
   returns a ``list``, and each element in the return value ``list`` is
   an element corresponding to each integer index in the parameter
   ``list``. The value of the expression ``[3, 2, 1].item([0, 2])`` is
   ``[3, 1]``. If an element type in the parameter ``list`` is not an
   integer, then the value at that position in the return value ``list``
   is ``nil``.

-  ``range`` Index: Using an integer range as an index, ``item`` returns
   a ``list``. The returned value stores the indexed elements from
   ``list`` from the lower limit to the upper limit of the parameter
   ``range``. If the index exceeds the index range of the indexed
   ``list``, the return value ``list`` will use ``nil`` to fill the
   position beyond the index.

``setitem`` method
^^^^^^^^^^^^^^^^^^

Set the value of the specified position in the container. The prototype
of this method is ``setitem(index, value)``, ``index`` is the position
of the element to be written, and ``value`` is the value to be written.
``index`` is the integer index value of the writing position. Index
positions outside the index range of the container will cause
``setitem`` to fail to execute.

``size`` method
^^^^^^^^^^^^^^^

Returns the number of elements in the container, which is the length of
the container. The prototype of this method is ``size()``.

``resize`` method
^^^^^^^^^^^^^^^^^

Reset ``list`` the length of the container. The prototype of this method
is ``resize(count)``, and the parameter ``count`` is the new length of
the container. When using ``resize`` to increase the length of the
container, the new element will be initialized to ``nil``. Using
``resize`` to reduce the length of the container will discard some
elements at the end of the container. E.g:

.. code:: python

   l = [1, 2, 3]
   l.resize(5) # Expansion, l == [1, 2, 3, nil, nil]
   l.resize(2) # Reduce, l == [1, 2]

``iter`` method
^^^^^^^^^^^^^^^

Returns an iterator for traversing the current ``list`` container.

``find`` method
^^^^^^^^^^^^^^^

Similar to ``item`` or ``list[idx]``. The only difference is if the
index is out of range, ``find`` return ``nil`` instead or raising an
exception.

``reverse`` method
^^^^^^^^^^^^^^^^^^

Changes the list in-place and reverses the order of elements. Also
returns the resulting list.

``map`` Class
~~~~~~~~~~~~~

``map`` Class is a built-in class type used to provide an unordered
container of key-value pairs. Inside the Berry interpreter, ``map`` uses
the Hash table to implement. You can use curly brace pairs to construct
a ``map`` container. Using an empty curly brace pair ``{}`` will
generate an empty ``map`` instance. If you need to construct a non-empty
``map`` instance, use a colon to separate the key and value, and use a
semicolon to separate multiple key-value pairs. For example,
``{0: 1, 2: 3}`` has two key-value pairs (0,1) and (2,3). You can also
get an empty ``map`` instance by calling the ``map`` class.

``map`` method (Constructor)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Initialize the ``map`` container, this method does not accept
parameters. Executing ``map()`` will get an empty ``map`` instance.

.. _tostring-method-1:

``tostring`` method
^^^^^^^^^^^^^^^^^^^

Serialize ``map`` as a string and return. The serialized string is
similar to literal writing. For example, the result of executing
``’str’: 1, 0: 2`` is ``"’str’: 1, 0: 2"``. If the ``map`` container
refers to itself, the corresponding position will use an ellipsis
instead of the specific value:

.. code:: python

   m = {'map': nil,'text':'hello'}
   m['map'] = m
   print(m) # {'text':'hello','map': {...}}

.. _insert-method-1:

``insert`` method
^^^^^^^^^^^^^^^^^

Insert a key-value pair in the ``map`` container. The prototype of this
method is ``insert(key, value)``, the parameter ``key`` is the key to be
inserted, and ``value`` is the value to be inserted. If the key ``map``
to be inserted exists in the container, the original key-value pair will
be updated.

.. _remove-method-1:

``remove`` method
^^^^^^^^^^^^^^^^^

Remove a key-value pair from the ``map`` container. The prototype of
this method is ``remove(key)``, and the parameter ``key`` is the key of
the key-value pair to be deleted.

.. _item-method-1:

``item`` method
^^^^^^^^^^^^^^^

Get a value in the ``map`` container. The prototype of this method is
``item(key)``, the parameter ``key`` is the key of the value to be
obtained, and the return value of the method is the value corresponding
to the key.

.. _setitem-method-1:

``setitem`` method
^^^^^^^^^^^^^^^^^^

Set the value corresponding to the specified key in the container. The
prototype of this method is ``setitem(key, value)``, ``key`` is the key
of the key-value pair to be written, and ``value`` is the value to be
written. If there is no key-value pair with the key ``key`` in the
container, the ``setitem`` method will fail.

.. _size-method-1:

``size`` method
^^^^^^^^^^^^^^^

Return the number of key-value pairs of the ``map`` container, which is
the length of the container. The prototype of this method is ``size()``.

``contains`` method
^^^^^^^^^^^^^^^^^^^

Returns boolean ``true`` if a matching key-value pair is found in the
``map`` container, otherwise ``false``. The prototype of this method is
``contains(key)``.

.. _find-method-1:

``find`` method
^^^^^^^^^^^^^^^

Returns the value corresponding to the specified key in the container.
The prototype of this method is ``find(key)`` or
``find(key, defaultvalue)``, ``key`` is the key of the key-value pair to
be accessed, and ``defaultvalue`` is the default value returned if the
key is not found. If no default value is specified, ``nil`` is returned
instead.

``range`` Class
~~~~~~~~~~~~~~~

``range`` The class is used to represent an integer closed interval. Use
the binary operator ``..`` to construct an instance of ``range``. The
left and right operands of the operator are required to be integers. For
example, ``0..10`` means the integer interval [0,10] ∩ ℤ.

If you don’t specify the high range, it is set to ``MAXINT``. Example:
``print(0..)  # (0..9223372036854775807)``

There are typically two ways to traverse a list:

.. code:: python

   l = [1,2,3,4]
   for e:l print(e) end # 1/2/3/4
   for i:0..size(l)-1 print(l[i]) end # 1/2/3/4

``bytes`` Class
~~~~~~~~~~~~~~~

``bytes`` object are represented as arrays of Hex bytes. ``bytes``
constructor takes a string of Hex and builds the in-memory buffer.

Example:

.. code:: python

   b = bytes()
   print(b)   # bytes('')
   b = bytes("1155AA")  # sequence of bytes 0x11 0x55 0xAA
   size(b)  # 3 = 3 bytes
   b[0]     # 17 (0x11)
   b[0] = 16  # assign first byte
   print(b)   # bytes('1055AA')

``bytes`` method (Constructor)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Initialize a bytes array. There are several options.

**Option 1: empty value**

``bytes()`` creates a new empty bytes array. ``size(bytes()) == 0``.

There is no limit in the size of a bytes array, except the available
memory. An internal buffer is allocated and reallocated in case the
previous one was too small. The initial buffer is 36 bytes, but you can
pre-allocated a larger (or smaller) buffer if you know in advance the
size needed.

Similarly the buffer is automatically shrunk if it is used less than its
needed size.

.. code:: python

   b = bytes(4096)   # pre-allocated 4096 bytes

**Option 2: initial value**

If first argument is a ``string`` it is parsed as a list of Hex values.
You can add an optional second argument to pre-allocate a bigger buffer.

.. code:: python

   b = bytes("BEEF0000")
   print(b)   # bytes('beef0000')
   b = bytes("112233", 128)   # pre-allocate 128 bytes internally
   print(b)   # bytes('112233')

**Option 3: fixed size**

If the size provided is negative, the array size is fixed and cannot be
lowered nor raised.

.. code:: python

   b = bytes(-8)
   print(b)   # bytes('0000000000000000')

   b = bytes("AA", -4)
   print(b)   # bytes('AA000000')

   b = bytes("1122334455", -4)
   attribute_error: bytes object size if fixed and cannot be resized

**Option 4: memory mapping**

**Caution, use with great care**

In this mode, the bytes array is mapped to a specific region in memory.
You must provide the base address as ``comptr`` and the size. Size is
always fixed whether it is positive or negative. This feature is
**dangerous** since you can access any memory location, causing a crash
if the location is protected or invalid. Use with care.

In this case ``b.ismapped()`` returns ``true`` indicating a mapped
memory buffer. In all other cases ``b.ismapped()`` returns ``false``.
This is typically used to know if Berry allocated the buffer or not, and
if sub-elements need to be explicitly deallocated.

Example:

.. code:: python

   import introspect
   def f() return 0 end

   addr = introspect.toptr(f)
   print(addr)   # <ptr: 0x3ffeaf88>

   b = bytes(addr, 8)
   print(b)   # bytes('F8EAFE3F24000000')
   # this example shows the first 8 bytes of the function object in memory

.. _size-method-2:

``size`` method
^^^^^^^^^^^^^^^

Returns the number of bytes in the bytes array

.. code:: python

   b = bytes("1122334455")
   b.size()   # 5
   size(b)    # 5

.. _tostring-method-2:

``tostring`` method
^^^^^^^^^^^^^^^^^^^

Shows a human readable form of the bytes array in hex. By default, it
shows only the first 32 characters. You can request more characters by
adding an int argument with the maximum number of bytes you want to
convert. ``tostring`` is internally used when you print an object.
``print(b)`` is equivalent to ``print(b.tostring())``. It is different
from ``asstring`` which turns a bytes array to the equivalent low-level
string object without any encoding.

.. code:: python

   b = bytes("1122334455")
   b.tostring()   # 'bytes(\'1122334455\')'

   b = bytes()
   b.resize(64)   # resize to 64 bytes
   b.tostring()   # 'bytes(\'0000000000000000000000000000000000000000000000000000000000000000...\')'
   b.tostring(500) # 'bytes(\'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\')'

``tohex`` method
^^^^^^^^^^^^^^^^

Converts the bytes array in an hex string, similar to the one returned
by ``tostring()`` but without decorators.

.. code:: python

   b = bytes("1122334455")
   b.tohex()   # '1122334455'

``fromhex`` method
^^^^^^^^^^^^^^^^^^

Updates the content of the bytes array from a new hex string. This
allows to load a new hex string without allocating a new bytes object.

.. code:: python

   b = bytes("1122334455")
   b.fromhex("AABBCC")  # bytes('AABBCC')

``clear`` method
^^^^^^^^^^^^^^^^

Sets back the bytes array to empty

.. code:: python

   b = bytes("1122")
   b.clear()
   print(b)   # bytes()

.. _resize-method-1:

``resize`` method
^^^^^^^^^^^^^^^^^

Shrink or expand the bytes array to match the specified size. If
expanded, NULL (0x00) bytes are added at the end of the buffer.

.. code:: python

   b = bytes("11223344")
   b.resize(6)
   print(b)   # bytes('112233440000')
   b.resize(2)
   print(b)   # bytes('1122')

Concatenation ``+`` and ``..`` methods
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can use ``+`` to concatenate two ``bytes`` list, creating a new
``bytes`` object. ``..`` changes the list in place and can be used to
add an ``int`` (1 bytes) or a ``bytes`` object

::

   b = bytes("1122")
   c = bytes("3344")
   d = b + c           # b and c are unchanged
   print(d)            # bytes('11223344')
   print(b)            # bytes('1122')
   print(c)            # bytes('3344')

   e = b..c            # now b is changed
   print(e)            # bytes('11223344')
   print(b)            # bytes('11223344')
   print(c)            # bytes('3344')

bytes access ``[]`` method
^^^^^^^^^^^^^^^^^^^^^^^^^^

You can access individual bytes as integers, to read and write. Values
not in the range 0..255 are silently choped.

.. code:: python

   b = bytes("010203")
   print(b[0])         # 1

   # negative indices count from the end
   print(b[-1])        # 3

   # out of bounds generate an exception
   print(b[5])         # index_error: bytes index out of range

   b[0] = -1
   print(b)            # bytes('FF0203')

   b[1] = 256
   print(b)            # bytes('FF0003')

range access ``[]`` method
^^^^^^^^^^^^^^^^^^^^^^^^^^

You can use the ``[]`` accessor with a range to get an sub-list of
``bytes``. If an index is negative, it is taken from the end of the
array.

This construct cannot be used a an *lvalue*, i.e. you can’t splice like
``b[1..2] = bytes("0011")  # not allowed``.

.. code:: python

   b = bytes("001122334455")
   print(b[1..2])      # bytes('1122')

   # remove first 2 bytes
   print(b[2..-1])     # bytes('22334455')

   # remove last 2 bytes
   print(b[0..-3])     # bytes('00112233')

   # overshooting is allowed
   print(b[4..10])     # bytes('4455')

   # inversed indices return an empty array
   print(b[5..4])      # bytes('')

The standard ``item`` and ``setitem`` methods are implemented, and
transparently mapped to ``[]`` operator.

``copy`` method
^^^^^^^^^^^^^^^

Creates a fresh new copy of the ``bytes`` object. A new memory buffer is
allocated and data is duplicated.

.. code:: python

   b = bytes("1122")
   print(b)          # bytes('1122')

   c = b.copy()
   print(c)          # bytes('1122')

   b.clear()
   print(b)          # bytes('')
   print(c)          # bytes('1122')bytes('1122')

``get, geti`` methods
^^^^^^^^^^^^^^^^^^^^^

Read a 1/2/4 bytes value from any offset in the bytes array. The
standard mode is little endian, if you specify a negative size it
enables big endian. ``get`` returns unsigned values, while ``geti``
returns signed values.

::

   b.get(<offset>, <size>) -> bytes object

If the offset is out of range, ``0`` is returned (no exception raised).

Example:

.. code:: python

   b = bytes("010203040506")
   print(b.get(2,2))         # 1027 - 0x0403 read 2 bytes little endian
   print(b.get(2,-2))        #  772 - 0x0304 read 2 bytes big endian

   print(b.get(2,4))         # 100992003 - 0x06050403 - little endian
   print(b.get(2,-4))        #  50595078 - 0x03040506 - big endian

   b = bytes("FEFF")
   print(b.get(0, 2))        # 65534 - 0xFFFE
   print(b.geti(0, 2))       # -2 - 0xFFFE

``set, seti`` methods
^^^^^^^^^^^^^^^^^^^^^

Similar to ``get`` and ``geti``, allows to set a 1/2/4 bytes value at
any offset. ``seti`` uses signed integers, ``set`` unsigned (actually it
does not make a difference).

If the offset is out of range, no change is done (no exception raised).

::

   bytes.set(<offset>, <value>, <size>)

``add`` method
^^^^^^^^^^^^^^

This methods adds value of 1/2/4 bytes (little endian or big endian) at
the end of the buffer. If size is negative, the value is treated as big
endian.

::

   b.add(<value>, <size>)

Example:

.. code:: python

   b = bytes("0011")
   b.add(0x22, 1)
   print(b)             # bytes('001122')
   b.add(0x2233, 2)
   print(b)             # bytes('0011223322')
   b.add(0x22334455, 4)
   print(b)             # bytes('001122332255443322')
   b.add(0x00)
   print(b)             # bytes('00112233225544332200')
   b.clear()
   b.add(0x0102, -2)
   print(b)             # bytes('0102')
   b.add(0x01020304, -4)
   print(b)             # bytes('010201020304')

``asstring`` method
^^^^^^^^^^^^^^^^^^^

Converts a bytes buffer to a string. The buffer is converted as-is
without any encoding considerations. If the buffer contains NULL
characters, the string will be truncated.

.. code:: python

   b=bytes("3344")
   print(b.asstring())   # '3D'

``fromstring`` method
^^^^^^^^^^^^^^^^^^^^^

Converts a bytes buffer to a string. The buffer is converted as-is
without any encoding considerations. If the buffer contains NULL
characters, the string will be truncated.

.. code:: python

   b=bytes().fromstring("Hello")
   print(b)              # bytes('48656C6C6F')

bits manipulation ``setbits, getbits`` methods
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can read and write at sub-byte level, specifying from which bit to
which bit. The offset is in bits, not bytes. Add the number of bytes \*
8.

::

   b.setbits(<offset_bits>, <len_bits>, <value>)
   b.getbits(<offset_bits>, <len_bits>)

base64 encode ``tob64`` method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Converts a bytes array to a base64 string.

.. code:: python

   b = bytes('deadbeef0011')
   s = b.tob64()
   print(s)               # 3q2+7wAR

base64 decode ``fromb64`` method
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Converts a base64 string into a bytes array.

.. code:: python

   s = '3q2+7wAR'
   b = bytes().fromb64(s)
   print(b)               # bytes('DEADBEEF0011')

``getfloat`` and ``setfloat`` methods
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Similar to ``get/set``, allows to read or write a 32 bits float value.

::

   b.getfloat(<offset>)
   b.getfloat(<offset>, <number>)

.. code:: berry

   b = bytes("00000000")
   b.getfloat(0)     # 0
   b.setfloat(0, -1.5)
   print(b)  # bytes('0000C0BF')
   b.getfloat(0)  # -1.5

``_buffer`` method
^^^^^^^^^^^^^^^^^^

**Advanced feature**: returns the address of the buffer in memory, to be
used with C code.

.. code:: berry

   b = bytes('1122')
   b._buffer()  # <ptr: 0x600000c283c0>

``_change_buffer`` method
^^^^^^^^^^^^^^^^^^^^^^^^^

**Advanced feature**: works only for mapped buffers
(i.e. ``b.ismapped() == true``), allows to remap the buffer to a new
memory address. This allows to reuse the bytes() object without
reallocating a new instance.

.. code:: berry

   # this example uses pointer allocation, use with great care
   b1 = bytes("11223344")
   b2 = bytes("AABBCCDD")
   b1._buffer()  # <ptr: 0x600000c2c390>
   b2._buffer()  # <ptr: 0x600000c24270>

   # now we create c as a mapped buffer of 4 bytes to the address of b1
   c = bytes(b1._buffer(), 4)
   print(c)  # bytes('11223344') -- mapped to b1
   c._buffer()   # <ptr: 0x600000c2c390>

   # let's change a byte to prove it
   c[0] = 254
   print(c)  # bytes('FE223344')
   print(b1) # bytes('FE223344') -- b1 was changed

   # reallocate c to map b2
   c._change_buffer(b2._buffer())
   print(c)  # bytes('AABBCCDD')
   c._buffer()  # <ptr: 0x600000c24270>

Expansion Modules
-----------------

JSON Module
~~~~~~~~~~~

JSON is a lightweight data exchange format. It is a subset of
JavaScript. It uses a text format that is completely independent of the
programming language to represent data. Berry provides a JSON module to
provide support for JSON data. The JSON module only contains two
functions ``load`` and ``dump``, which are used to parse JSON strings
and multiply Berry objects and serialize a Berry object into JSON text.

``load`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   load(text)

**Description**

This function is used to convert the input JSON text into a Berry object
and return it. The conversion rules are shown in Table 1.1. If there is
a syntax error in the JSON text, the function will return ``nil``.

============= =======================
**JSON type** **Berry type**
============= =======================
``null``      ``nil``
``number``    ``integer`` or ``real``
``string``    ``string``
``array``     ``list``
``object``    ``map``
============= =======================

JSON type to Berry type conversion rules

**Example**

.. code:: python

   import json
   json.load('0') # 0
   json.load('[{"name": "liu", "age": 13}, 10.0]') # [{'name':'liu','age': 13}, 10]

``dump`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   dump(object, ['format'])

**Description**

This function is used to serialize the Berry object into JSON text. The
conversion rules for serialization are shown in Table 1.2.

=============== =============
**Berry type**  **JSON type**
=============== =============
``nil``         ``null``
``integer``     ``number``
``real``        ``number``
``list``        ``array``
``map``         ``object``
``map``\ Key of ``string``
other           ``string``
=============== =============

Berry type to JSON type conversion rules

**Example**

.. code:: python

   import json
   json.dump('string') #'"string"'
   json.dump('string') #'"string"'
   json.dump({0:'item 0','list': [0, 1, 2]}) #'{"0":"item 0","list":[0,1,2]}'
   json.dump({0:'item 0','list': [0, 1, 2],'func': print},'format')
   #-
   {
       "0": "item 0",
       "list": [
           0,
           1,
           2
       ],
       "func": "<function: 00410310>"
   }
   -#

Math Module
~~~~~~~~~~~

This module is used to provide support for mathematical functions, such
as commonly used trigonometric functions and square root functions. To
use the math module, first use the ``import math`` statement to import.
All examples in this section assume that the module has been imported
correctly.

``pi`` constant
^^^^^^^^^^^^^^^

The approximate value of Pi *π*, a real number type, approximately equal
to 3.141592654.

**Example**

.. code:: python

   math.pi # 3.14159

``abs`` function
^^^^^^^^^^^^^^^^

.. code:: python

   abs(value)

**Description**

This function returns the absolute value of the parameter, which can be
an integer or a real number. If there are no parameters, the function
returns ``0``, if there are multiple parameters, only the first
parameter is processed. ``abs`` The return type of the function is a
real number.

**Example**

.. code:: python

   math.abs(-1) # 1
   math.abs(1.5) # 1.5

``ceil`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   ceil(value)

**Description**

This function returns the rounded up value of the parameter, that is,
the smallest integer value greater than or equal to the parameter. The
parameter can be an integer or a real number. If there are no
parameters, the function returns ``0``, if there are multiple
parameters, only the first parameter is processed. ``ceil`` The return
type of the function is a real number.

**Example**

.. code:: python

   math.ceil(-1.2) # -1
   math.ceil(1.5) # 2

``floor`` function
^^^^^^^^^^^^^^^^^^

.. code:: python

   floor(value)

**Description**

This function returns the rounded down value of the parameter, which is
not greater than the maximum integer value of the parameter. The
parameter can be an integer or a real number. If there are no
parameters, the function returns ``0``, if there are multiple
parameters, only the first parameter is processed. ``floor`` The return
type of the function is a real number.

**Example**

.. code:: python

   math.floor(-1.2) # -2
   math.floor(1.5) # 1

``sin`` function
^^^^^^^^^^^^^^^^

.. code:: python

   sin(value)

**Description**

This function returns the sine function value of the parameter. The
parameter can be an integer or a real number, and the unit is radians.
If there are no parameters, the function returns ``0``, if there are
multiple parameters, only the first parameter is processed. ``sin`` The
return type of the function is a real number.

**Example**

.. code:: python

   math.sin(1) # 0.841471
   math.sin(math.pi * 0.5) # 1

``cos`` function
^^^^^^^^^^^^^^^^

.. code:: python

   cos(value)

**Description**

This function returns the value of the cosine function of the parameter.
The parameter can be an integer or a real number in radians. If there
are no parameters, the function returns ``0``, if there are multiple
parameters, only the first parameter is processed. ``cos`` The return
type of the function is a real number.

**Example**

.. code:: python

   math.cos(1) # 0.540302
   math.cos(math.pi) # -1

``tan`` function
^^^^^^^^^^^^^^^^

.. code:: python

   tan(value)

**Description**

This function returns the value of the tangent function of the
parameter. The parameter can be an integer or a real number, in radians.
If there are no parameters, the function returns ``0``, if there are
multiple parameters, only the first parameter is processed. ``tan`` The
return type of the function is a real number.

**Example**

.. code:: python

   math.tan(1) # 1.55741
   math.tan(math.pi / 4) # 1

``asin`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   asin(value)

**Description**

This function returns the arc sine function value of the parameter. The
parameter can be an integer or a real number. The value range is [−1,1].
If there are no parameters, the function returns ``0``, if there are
multiple parameters, only the first parameter is processed. ``asin`` The
return type of the function is a real number and the unit is radians.

**Example**

.. code:: python

   math.asin(1) # 1.5708
   math.asin(0.5) * 180 / math.pi # 30

``acos`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   acos(value)

**Description**

This function returns the arc cosine function value of the parameter.
The parameter can be an integer or a real number. The value range is
[−1,1]. If there are no parameters, the function returns ``0``, if there
are multiple parameters, only the first parameter is processed. ``acos``
The return type of the function is a real number and the unit is
radians.

**Example**

.. code:: python

   math.acos(1) # 0
   math.acos(0) # 1.5708

``atan`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   atan(value)

**Description**

This function returns the arctangent function value of the parameter.
The parameter can be an integer or a real number. The value range is
[−∞,+∞]. If there are no parameters, the function returns ``0``, if
there are multiple parameters, only the first parameter is processed.
``atan`` The return type of the function is a real number and the unit
is radians.

**Example**

.. code:: python

   math.atan(1) * 180 / math.pi # 45

``sinh`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   sinh(value)

**Description**

This function returns the hyperbolic sine function value of the
parameter. If there are no parameters, the function returns ``0``, if
there are multiple parameters, only the first parameter is processed.
``sinh`` The return type of the function is a real number.

**Example**

.. code:: python

   math.sinh(1) # 1.1752

``cosh`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   cosh(value)

**Description**

This function returns the hyperbolic cosine function value of the
parameter. If there are no parameters, the function returns ``0``, if
there are multiple parameters, only the first parameter is processed.
``cosh`` The return type of the function is a real number.

**Example**

.. code:: python

   math.cosh(1) # 1.54308

``tanh`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   tanh(value)

**Description**

This function returns the hyperbolic tangent function value of the
parameter. If there are no parameters, the function returns ``0``, if
there are multiple parameters, only the first parameter is processed.
``tanh`` The return type of the function is a real number.

**Example**

.. code:: python

   math.tanh(1) # 0.761594

``sqrt`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   sqrt(value)

**Description**

This function returns the square root of the argument. The parameter of
this function cannot be negative. If there are no parameters, the
function returns ``0``, if there are multiple parameters, only the first
parameter is processed. ``sqrt`` The return type of the function is a
real number.

**Example**

.. code:: python

   math.sqrt(2) # 1.41421

``exp`` function
^^^^^^^^^^^^^^^^

.. code:: python

   exp(value)

**Description**

This function returns the value of the parameter’s exponential function
based on the natural constant *e*. If there are no parameters, the
function returns ``0``, if there are multiple parameters, only the first
parameter is processed. ``exp`` The return type of the function is a
real number.

**Example**

.. code:: python

   math.exp(1) # 2.71828

``log`` function
^^^^^^^^^^^^^^^^

.. code:: python

   log(value)

**Description**

This function returns the natural logarithm of the argument. The
parameter must be a positive number. If there are no parameters, the
function returns ``0``, if there are multiple parameters, only the first
parameter is processed. ``log`` The return type of the function is a
real number.

**Example**

.. code:: python

   math.log(2.718282) # 1

``log10`` function
^^^^^^^^^^^^^^^^^^

.. code:: python

   log10(value)

**Description**

This function returns the logarithm of the parameter to the base 10. The
parameter must be a positive number. If there are no parameters, the
function returns ``0``, if there are multiple parameters, only the first
parameter is processed. ``log10`` The return type of the function is a
real number.

**Example**

.. code:: python

   math.log10(10) # 1

``deg`` function
^^^^^^^^^^^^^^^^

.. code:: python

   deg(value)

**Description**

This function is used to convert radians to angles. The unit of the
parameter is radians. If there are no parameters, the function returns
``0``, if there are multiple parameters, only the first parameter is
processed. ``deg`` The return type of the function is a real number and
the unit is an angle.

**Example**

.. code:: python

   math.deg(math.pi) # 180

``rad`` function
^^^^^^^^^^^^^^^^

.. code:: python

   rad(value)

**Description**

This function is used to convert angles to radians. The unit of the
parameter is angle. If there are no parameters, the function returns
``0``, if there are multiple parameters, only the first parameter is
processed. ``rad`` The return type of the function is a real number and
the unit is radians.

**Example**

.. code:: python

   math.rad(180) # 3.14159

``pow`` function
^^^^^^^^^^^^^^^^

.. code:: python

   pow(x, y)

**Description**

The return value of this function is the result of the expression
*x*\ \ *y*\ , which is the parameter ``x`` to the ``y`` power. If the
parameters are not complete, the function returns ``0``, if there are
extra parameters, only the first two parameters are processed. ``pow``
The return type of the function is a real number.

**Example**

.. code:: python

   math.pow(2, 3) # 8

``srand`` function
^^^^^^^^^^^^^^^^^^

.. code:: python

   srand(value)

**Description**

This function is used to set the seed of the random number generator.
The type of the parameter should be an integer.

**Example**

.. code:: python

   math.srand(2)

``rand`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   rand()

**Description**

This function is used to get a random integer.

**Example**

.. code:: python

   math.rand()

Time Module
~~~~~~~~~~~

This module is used to provide time-related functions.

``time`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   time()

**Description**

Returns the current timestamp. The timestamp is the time elapsed since
Unix Epoch (1st January 1970 00:00:00 UTC), in seconds.

.. _dump-function-1:

``dump`` function
^^^^^^^^^^^^^^^^^

.. code:: python

   dump(ts)

**Description**

The input timestamp ``ts`` is converted into a time ``map``, and the
key-value correspondence is shown in Table below:

+----------+----------+----------+----------+----------+----------+
| **key**  | **value**| **key**  | **value**| **key**  | **value**|
+==========+==========+==========+==========+==========+==========+
| ``       | Year     | ``’      | Month    | `        | Day      |
| ’year’`` | (from    | month’`` | (1-12)   | `’day’`` | (1-31)   |
|          | 1900)    |          |          |          |          |
+----------+----------+----------+----------+----------+----------+
| ``       | Hour     | `        | Points   | `        | Seconds  |
| ’hour’`` | (0-23)   | `’min’`` | (0-59)   | `’sec’`` | (0-59)   |
+----------+----------+----------+----------+----------+----------+
| ``’we    | Week     |          |          |          |          |
| ekday’`` | (1-7)    |          |          |          |          |
+----------+----------+----------+----------+----------+----------+

``time.dump`` The key-value relationship of the function return value

``clock`` function
^^^^^^^^^^^^^^^^^^

.. code:: python

   clock()

**Description**

This function returns the elapsed time from the start of execution of
the interpreter to when the function is called in seconds. The return
value of this function is of type ``real``, and its timing accuracy is
determined by the specific platform.

String Module
~~~~~~~~~~~~~

The String module provides string processing functions.

To use the string module, first use the ``import string`` statement to
import. All examples in this section assume that the module has been
imported correctly.

``count`` function
^^^^^^^^^^^^^^^^^^

::

   string.count(s, sub[, begin[, end]])

Count the number of occurrences of the sub string in the string s.
Search from the position between ``begin`` and ``end`` of ``s`` (default
is 0 and size(s)).

``split`` function
^^^^^^^^^^^^^^^^^^

::

   string.split(s, pos)

Split the string s into two substrings at position ``pos``, and returns
the list of those strings.

::

   string.split(s, sep[, num]) 

Splits the string s into substrings wherever sep occurs, and returns the
list of those strings. Split at most num times (default is
string.count(s, sep)).

``find`` function
^^^^^^^^^^^^^^^^^

::

   string.find(s, sub[, begin[, end]])

Check whether the string s contains the substring sub. If the begin and
end (default is 0 and size(s)) are specified, they will be searched in
this range.

``hex`` function
^^^^^^^^^^^^^^^^

::

   hex(number)

Convert number to hexadecimal string.

``byte`` function
^^^^^^^^^^^^^^^^^

::

   byte(s)

Get the code value of the first byte of the string s.

``char`` function
^^^^^^^^^^^^^^^^^

::

   char(number)

Convert the number used as the code to a character.

``format`` function
^^^^^^^^^^^^^^^^^^^

::

   string.format(fmt[, args])

Returns a formatted string. The pattern starting with ‘%’ in the
formatting template fmt will be replaced by the value of [args]:
%[flags][fieldwidth][.precision]type

+-----------------------------------+-----------------------------------+
| Type                              | Description                       |
+===================================+===================================+
| %d                                | Decimal integer                   |
+-----------------------------------+-----------------------------------+
| %o                                | Octal integer                     |
+-----------------------------------+-----------------------------------+
| %x                                | Hexadecimal integer lowercase     |
+-----------------------------------+-----------------------------------+
| %X                                | Hexadecimal integer uppercase     |
+-----------------------------------+-----------------------------------+
| %x                                | Octal integer lowercase           |
+-----------------------------------+-----------------------------------+
| %X                                | Octal integer uppercase           |
+-----------------------------------+-----------------------------------+
| %f                                | Floating-point in the form        |
|                                   | [-]nnnn.nnnn                      |
+-----------------------------------+-----------------------------------+
| %e %E                             | Floating-point in exp. form       |
|                                   | [-]n.nnnn e [+|-]nnn, uppercase   |
|                                   | if %E                             |
+-----------------------------------+-----------------------------------+
| %g %G                             | Floating-point as %f if −4 < exp. |
|                                   | ≤ precision, else as %e;          |
|                                   | uppercase if %G                   |
+-----------------------------------+-----------------------------------+
| %c                                | Character having the code passed  |
|                                   | as integer                        |
+-----------------------------------+-----------------------------------+
| %s                                | String with no embedded zeros     |
+-----------------------------------+-----------------------------------+
| %q                                | String between double quotes,     |
|                                   | with special characters escaped   |
+-----------------------------------+-----------------------------------+
| %%                                | The ‘%’ character (escaped)       |
+-----------------------------------+-----------------------------------+

+-----------------------------------+-----------------------------------+
| Type                              | Description                       |
+===================================+===================================+
| -                                 | Left-justifies, default is        |
|                                   | right-justify                     |
+-----------------------------------+-----------------------------------+
| +                                 | Prepends sign (applies to         |
|                                   | numbers)                          |
+-----------------------------------+-----------------------------------+
| (space)                           | Prepends sign if negative, else   |
|                                   | space                             |
+-----------------------------------+-----------------------------------+
| #                                 | Adds “0x” before %x, force        |
|                                   | decimal point; for %e, %f, leaves |
|                                   | trailing zeros for %g             |
+-----------------------------------+-----------------------------------+

+-----------------------------------+-----------------------------------+
| Field width and precision         | Description                       |
+===================================+===================================+
| n                                 | Puts at least n characters, pad   |
|                                   | with blanks                       |
+-----------------------------------+-----------------------------------+
| 0n                                | Puts at least n characters,       |
|                                   | left-pad with zeros               |
+-----------------------------------+-----------------------------------+
| .n                                | Use at least n digits for         |
|                                   | integers, rounds to n decimals    |
|                                   | for floating-point or no more     |
|                                   | than n chars. for strings         |
+-----------------------------------+-----------------------------------+

Module ``os``
~~~~~~~~~~~~~

The OS module provides system-related functions, such as file and
path-related functions. These functions are platform-related. Currently,
Windows VC and POSIX style codes are implemented in the Berry
interpreter. If it runs on other platforms, the functions in the OS
module are not guaranteed to be provided.

TODO

Module ``global``
~~~~~~~~~~~~~~~~~

Module ``global`` provides a way to access global variables via a
module. The Berry compiler checks that a global exists when compiling
code. However there are cases when globals are created dynamically by
code and are not yet known at compile time. Using the module ``global``
gives complete freedom to access statically or dynamically global
variables.

Accessing a global is simplay made with ``global.<name>`` for reading
and writing. You can also use the special syntax ``global.(name)`` if
``name`` is a variable containing the name of the global as string.

Example:

.. code:: berry

   > import global
   > a = 1
   > global.a
   1
   >
   > b
   syntax_error: stdin:1: 'b' undeclared (first use in this function)
   > global.b = 2
   > b
   2
   > global.b
   2
   > var name = "b"
   > global.(name)
   2

Calling ``global()`` returns the list of all global names currently
defined (builtins are not included).

.. code:: berry

   > import global
   > a = 1
   > global.b = 2
   > global()
   ['_argv', 'b', 'global', 'a']

``global.contains(<name)> -> bool`` provides an easy way to know if a
global name is already defined.

.. code:: berry

   > import global
   > global.contains("g")
   false
   > g = 1
   > global.contains("g")
   true

Module ``introspect``
~~~~~~~~~~~~~~~~~~~~~

Module ``introspect`` provides primitives to dynamically access
variables or modules. Use with ``import introspect``.

``introspect.members(object: class or module or instance or nil) -> list``
returns the list of names of members for the ``class``, ``instance`` or
``module``. Keep in mind that it does not include potential virtual
members created via ``member`` and ``setmember``.

``introspect.members()`` returns the list of global variables (not
including builtins) and is equivalent to ``global()``

``introspect.get(object: class or instance or module, name:string) -> any``
and
``introspect.set(object: class or instance or module, name:string, value:any) -> nil``
allows to read and write any member by name.

``introspect.get(o, "a")`` is equivalent ot ``o.a``,
``introspect.set(o, "a", 1)`` is equivalent to ``o.a = 1``. There is
also an alternative syntax: ``o.("a")`` is equivalent to ``o.a`` and
``o.("a) = 1`` is equivalent to ``o.a = 1``.

``introspect.module(name:string) -> any`` is equivalent to
``import name`` except that it does not create the global or local
variable, but returns the module. This is the only way to load a module
with a dynamic name, ``import name`` only takes a static name.

``introspect.toptr(addr:int) -> comptr`` converts an integer to a comptr
pointer. ``introspect.fromptr(addr:comptr) -> int`` does the reverse and
converts a pointer to an int. Warning: use with care. On platforms where
``int`` and ``void*`` don’t have the same size, these functions will
most certainly give unusable results.

``introspect.ismethod(f:function) -> bool`` checks if the provided
function is a method of an instance (taking self as first argument), or
a plain function. This is mainly use to prevent a common mistake of
passing an instance method as callbakc, where you should use a closure
capturing the instance like ``/ -> self.do()``.

Module ``solidify``
~~~~~~~~~~~~~~~~~~~

This module allows to solidify Berry bytecode into flash. This allows to
save RAM since the code is in Flash, makes it a good alternative to
native C functions.

See 8.4 Solidification
