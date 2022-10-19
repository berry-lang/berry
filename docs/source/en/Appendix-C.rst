Porting Guide
=============

Configuration File
------------------

The configuration header file of the Berry interpreter is
*berry_conf.h*. This file includes a batch of macros for configuration
and defines some platform-related content.

*berry_conf.h* File
~~~~~~~~~~~~~~~~~~~

Configuration Macro Switch
^^^^^^^^^^^^^^^^^^^^^^^^^^

The configuration macros introduced in this section are usually used for
compiling switches of some source codes. For the convenience of
description, we call this macro “macro switch”. For the macro switch,
“on” refers to setting the macro switch to a non-zero value, and “off”
refers to setting the value of the macro switch to ``0``.

Some macro switches have multiple states, not just “on” or “off”. These
macro switches are generally used for configurations with multiple
options. There are also some configuration macros that are not macro
switches. No matter what the value of these macros is, there will be no
code and therefore will not participate in compilation. These macros are
generally used to configure the quantity value.

[section::BE_DEBUG]

This macro switch is used to turn on or off the debugging function of
the interpreter itself. When the value of ``BE_DEBUG`` is ``0``,
debugging is turned off, otherwise it will be turned on. The debugging
function mentioned here refers to the debugging of the interpreter, not
the debugging function of the Berry program. The default value of
``BE_DEBUG`` is ``0``. If you use *Makefile* that comes with the
interpreter project to compile, this macro switch will be turned on
automatically when you use the ``make debug`` command.

When this macro is opened, some assertions will be turned on, and an
error message will be output when the interpreter encounters an error
that the assertion can catch. You can open ``BE_DEBUG`` when debugging
the interpreter, and close it when compiling the release.

This macro switch configures the floating point type used by the
``breal`` type. When the value of the macro is ``0``, the ``double``
type will be used to implement ``breal``, otherwise the ``float`` type
will be used to implement ``breal``. This macro switch can be turned on
in some environments with low performance or memory configuration. In
the default implementation, this macro switch is turned off.

This macro configures the implementation of the ``bint`` type. When the
value of the macro is ``0``, the ``int`` type will be used to implement
``bint``, when the value is ``1``, the ``long`` type will be used to
implement ``bint``, and when the value is ``2``, 64 will be used ] Bit
signed integer type (``__int64`` under Windows, ``long long`` on other
platforms) implements ``bint``. The default value of this macro is
``2``. If you want to reduce memory usage, you can set this macro to
``0`` or ``1`` to enable 32-bit integer type.

This macro is used to configure runtime debugging information of Berry
code. It has 3 available values: set to ``0`` to turn off the output of
the file name and line number of the runtime debugging information, and
set to ``1`` to output the file name and line number in the runtime
debugging information, set to ``2`` When using ``uint16_t`` (16-bit
integer) type to store the row number information. Its default value is
``1``.

Setting this macro to ``0`` will not store the file name and line number
information, so the memory consumption is minimal. When set to ``2``, it
consumes less memory, but if the program is too long, ``uint16_t`` will
overflow.

This macro switch configures the function of constructing objects at
compile time. Turning on this macro means that constructing objects at
compile time is enabled. This macro is turned on by default. When this
macro is turned on, the native objects in the standard library will be
generated at compile time, and when this macro is turned off, the
objects in the standard library will be built at runtime.

``be_regfunc`` and ``be_regclass`` functions will be affected by this
macro. The built-in object table cannot be modified when using
compile-time object construction. At this time, these two functions
cannot register objects in the built-in scope, but register objects in
the global scope.

The objects constructed during the compile time are stored together with
the code and will not occupy RAM (or the readable and writable area in
the memory) resources. The construction technology during the compile
time can also reduce the startup time of the interpreter, so it is
recommended to open this macro. Please refer to section
[section::precompiled_build] for more details on compile-time
construction techniques.

This macro defines the maximum Berry stack capacity, which refers to the
number of Berry objects. When the Berry code uses more than this amount
of stack, it will stop executing the program and return an error
message. The default value of this macro is ``2000``, which can be
modified according to the memory capacity of the system.

This value does not affect the memory usage of the Berry stack, because
the capacity of the Berry stack is dynamically adjusted, so no matter
how much it is set to, it cannot help reduce memory usage. Its main
function is to terminate execution when the Berry program consumes too
much stack. These programs are very likely to be incorrect, for example,
recursive function calls without return conditions will continue to
consume the stack.

This macro defines the minimum available space in the Berry stack, and
its default value is ``10``. The native function may push a value into
the Berry stack. At this time, the stack will not automatically grow, so
make sure that there is enough space in the stack for the native
function to use. It is not recommended to modify this value, but to use
the ``be_stack_require`` function where more stack space is really
needed.

In order to detect stack overflow errors when debugging the interpreter,
you can open the ``BE_DEBUG`` macro (section [section::BE_DEBUG]).

When this macro is opened, the short string object will save the hash
value of the string to improve the running speed, but the size of each
string object will increase by 4 bytes. This macro is turned off by
default, and the current tests have not found that opening this macro
will bring significant improvement.

This macro switch is used to enable or disable the ``string`` module,
which is turned on by default.

This macro switch is used to enable or disable the ``json`` module,
which is turned on by default.

This macro switch is used to enable or disable the ``math`` module,
which is turned on by default.

This macro switch is used to enable or disable the ``time`` module,
which is turned on by default.

This macro switch is used to enable or disable the ``os`` module, which
is turned on by default.

This macro determines the ``abort`` function used internally by the
Berry interpreter. By default or when the macro is not defined, the
``abort`` function in the C standard library will be used. This macro is
defined as ``abort`` by default. If the user needs to explicitly specify
the ``abort`` function used by the interpreter, then replace the macro
definition with the function required by the user. This function should
be in the same form as the ``abort`` function declaration in the
standard library.

This macro determines the ``exit`` function used internally by the Berry
interpreter. By default or when the macro is not defined, the ``exit``
function in the C standard library will be used. This macro is defined
as ``exit`` by default. If the user needs to explicitly specify the
``exit`` function used by the interpreter, then replace the macro
definition with the function required by the user. This function should
be in the same form as the ``exit`` function declaration in the standard
library.

This macro determines the ``malloc`` function used internally by the
Berry interpreter. By default or when the macro is not defined, the
``malloc`` function in the C standard library will be used. This macro
is defined as ``malloc`` by default. If the user needs to explicitly
specify the function ``malloc`` used by the interpreter, then replace
the macro definition with the function required by the user. This
function should be in the same form as the ``malloc`` function
declaration in the standard library.

This macro determines the ``free`` function used internally by the Berry
interpreter. By default or when the macro is not defined, the ``free``
function in the C standard library will be used. This macro is defined
as ``free`` by default. If the user needs to explicitly specify the
``free`` function used by the interpreter, then replace the macro
definition with the function required by the user. This function should
be in the same form as the ``free`` function declaration in the standard
library.

This macro determines the ``realloc`` function used internally by the
Berry interpreter. By default or when the macro is not defined, the
``realloc`` function in the C standard library will be used. This macro
is defined as ``realloc`` by default. If the user needs to explicitly
specify the ``realloc`` function used by the interpreter, then replace
the macro definition with the function required by the user. This
function should be in the same form as the ``realloc`` function
declaration in the standard library.

This macro is used to define the implementation of the assertion
function. By default, the ``assert`` function in the C standard library
is used to implement the assertion. If the target system is inconvenient
to use the ``assert()`` function in the standard library to make an
assertion, you can modify the definition of the ``be_assert`` macro. A
correct assertion function should use the following declaration:

.. code:: c

   void assert(int condition);

Among them, ``condition`` is the assertion condition. If the condition
is not met, an error message will be output and the program will be
terminated. Of course, the “assert” function is usually implemented
using a macro.

*berry_port.c* File
-------------------

This file implements the low-level IO functions of the Berry
interpreter, including standard input and output and file system
support. The *berry_port.c* file in the *default* directory contains a
set of portable IO support. File operations and standard input and
output are implemented using APIs in the C standard library. Path and
folder operations support both Windows and POSIX standard APIs. This
file also implements a set of FatFs-based IO operation functions for
users to use directly. If you need to use the Berry interpreter in other
environments, then these functions must be implemented separately (may
only need to be implemented partially).

This section will introduce the functions of the functions implemented
in the *berry_port.c* file and guide users to implement their own
version.

.. code:: c

   void be_writebuffer(const char *buffer, size_t length);

Output a piece of data to the standard output device, the parameter
``buffer`` is the first address of the output data block, and ``length``
is the length of the output data block. This function outputs to the
``stdout`` file by default. Inside the interpreter, this function is
usually used as a character stream output, not a binary stream.

``be_writebuffer`` Functions are very versatile and must be implemented.

.. code:: c

   char* be_readstring(char *buffer, size_t size);

Input a piece of data from the standard input device, and read at most
one row of data each time this function is called. The parameter
``buffer`` is the data buffer passed in by the caller, and the capacity
of the buffer is ``size``. This function will stop reading and return
when the buffer capacity is used up, otherwise it will return when a
newline character or end of file character is read. If the function
executes successfully, it will directly use the ``buffer`` parameter as
the return value, otherwise it will return ``NULL``.

This function will add the read line breaks to the read data, and each
time the ``be_readstring`` function is called, it will continue to read
from the current position. This function is only called in the
implementation of the native function ``input``, and the
``be_readstring`` function may not be implemented when it is not
necessary.

.. code:: c

   void* be_fopen(const char *filename, const char *modes);

To open a file, ``filename`` is the name of the file to be opened, and
``modes`` is the opening method. The function will return a file handle
or a pointer to the file operation structure. The usage of this function
is similar to the ``fopen`` function in the C standard library. The file
name is a C-style string (ending with a ``\0`` character), and the
pattern should at least support the following conditions:

-  ``r``, ``rt``: To open a text file in read-only mode, the file must
   exist.

-  ``r+``, ``rt+``: Open a text file in read-write mode, and create a
   new file if the file does not exist.

-  ``rb``: Open a binary file in read-only mode, the file must exist.

-  ``rb+``: Open a binary file in read-write mode, and create a new file
   if the file does not exist.

-  ``w``, ``wt``: Create and open a text file in write-only mode, and
   the existing file will be deleted.

-  ``w+``, ``wt+``: Create and open a text file in read-write mode, and
   the existing file will be deleted.

-  ``wb``: Create and open a binary file in write-only mode, and the
   existing file will be deleted.

-  ``wb+``: Create and open a binary file in read-write mode, and the
   existing file will be deleted.

By default, the ``fopen`` function in the C standard library is used to
implement ``be_fopen``. If you use other methods to achieve, you should
ensure that the above operating modes can be achieved. If no file
operations are required, this function can be left blank. The file
operations here include all scenarios such as using the ``open``
function in the script, loading the script from a file (using the
``be_loadfile`` function), etc.

.. code:: c

   int be_fclose(void *hfile);

Close a file, ``hfile`` is the closed file handle. The function of this
function is similar to the function ``fclose`` in the C standard
library.

.. code:: c

   size_t be_fwrite(void *hfile, const void *buffer, size_t length);

Write a piece of data to the specified file. Parameter ``hfile`` is the
file handle to be written, ``buffer`` is the pointer of the data to be
written, ``length`` is the number of data to be written (in bytes).

.. code:: c

   size_t be_fread(void *hfile, void *buffer, size_t length);

Read a piece of data from the specified file. The parameter ``hfile`` is
the file handle to be read, ``buffer`` is the pointer to the read
buffer, and ``length`` is the number of bytes to be read.

.. code:: c

   char* be_fgets(void *hfile, void *buffer, int size);

Read a line from the file, similar to the ``fgets`` function in the C
standard library. Parameter ``hfile`` is the file handle to be read,
``buffer`` is the pointer of the read buffer, and ``size`` is the
capacity of the read buffer. This function will return when ``size - 1``
bytes, newline characters and end of file characters are read, and the
return value is ``buffer``.

.. code:: c

   int be_fseek(void *hfile, long offset);

Set the position of the file read and write pointer. The parameter
``hfile`` is the file handle to be operated, and ``offset`` is the value
to be set.

.. code:: c

   long int be_ftell(void *hfile);

Get the current read and write pointer of the file, the parameter
``hfile`` is the handle of the file to be operated, and the return value
of this function is the read and write pointer of the file.

.. code:: c

   long int be_fflush(void *hfile);

Write the data in the file buffer to the file. Parameter ``hfile`` is
the file to be operated.

.. code:: c

   size_t be_fsize(void *hfile);

Get the size of the file. Parameter ``hfile`` is the file to be
operated.
