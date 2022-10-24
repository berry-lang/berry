4. Statement
============

Berry is an imperative programming language. This paradigm assumes that
programs are executed step by step. Normally, Berry statements are
executed sequentially, and this program structure is called sequential
structure. Although the sequence structure is very basic, branch
structures and loop structures are usually used in actual programs.
Berry provides several control statements to realize this complex flow
structure, such as conditional statements and iteration statements.

Except for line comments, carriage returns or line feeds (“``\r``” and
“``\n``”) are only used as blank characters, so statements can be
written across lines. In addition, you can write multiple statements on
the same line.

You can add a semicolon at the end of the statement to indicate the end
of the statement, but the interpreter can usually split the statement
automatically without using a semicolon. You can use semicolons to tell
the interpreter how to parse the code for the code that will be
ambiguous. However, it is better not to write ambiguous code.

4.1 Simple sentence
-------------------

4.1.1 Expression statement
~~~~~~~~~~~~~~~~~~~~~~~~~~

Expression statements are mainly statements composed of assignment
expressions or function call expressions. Other expressions can also
form sentences, but they have no meaning. For example, expression
``1+2`` is a sentence written alone, but it has no effect. The following
routines give examples of expression statements and function statements:

.. code:: berry

   a = 1 # Assignment statement
   print(a) # Call statement

Line 2 is a simple assignment statement that assigns the literal value
``i`` to the variable ``a``. The statement in line 2 is a function call
statement, which prints the value of variable ``a`` by calling the
``print`` function.

Cross-line expressions are written in the same way as single-line
expressions, and no special line continuation symbols are required. E.g:

.. code:: berry

   a = 1 +
       func() # Wrap line

You can also write multiple expression statements on one line, and
various types of statements can be written on one line. This example
puts two expression statements on the same line:

.. code:: berry

   b = 1 c = 2 # Multiple statements

Sometimes the programmer wants to write two statements, but the
interpreter may mistakenly think it is one statement. This problem is
caused by the ambiguity in the process of grammatical analysis. Take
this code as an example:

.. code:: berry

   a = c
   (b) = 1 # Be regarded as a function call

Suppose the 4th and 5th lines are intended to be two expression
sentences: ``a = c`` and ``(b) = 1``, but the interpreter will interpret
them as a sentence: ``a = c(b) = 1``. The cause of this problem is that
the interpreter incorrectly parses ``c`` and ``(b)`` into function
calls. To avoid ambiguity, we can add a semicolon at the end of the
statement to clearly separate the statement:

.. code:: berry

   a = c; (b) = 1;

A better way is not to use parentheses on the left side of the
assignment number. Obviously, there is no reason to use parentheses
here. Under normal circumstances, complex expressions should not appear
on the left side of the assignment operator, but only simple expressions
composed of variable names, domain operation expressions, and subscript
operation expressions:

.. code:: berry

   a = c b = 1

Using simple expressions only on the left side of the assignment sign
will not cause ambiguity in sentence segmentation. Therefore, in most
cases, there is no need to use semicolons to separate expressions, and
we do not recommend this way of writing.

Block
~~~~~

A **Block** is a collection of several sentences. A block is a scope, so
the variables defined in the block can only be accessed inside the block
and its sub-blocks. There are many places where blocks are used, such as
``if`` statements, ``while`` statements, function declarations, etc.
These statements will contain a block through a pair of keywords. For
example, the block used in the ``if`` statement:

.. code:: berry

   if isOpen
       close()
       print('the device was closed')
   end

The statements in lines 2 to 3 constitute a block, which is sandwiched
between the pair of keywords ``if`` and ``end`` (the conditional
expression of the statement in ``if`` is not in the block). The block
does not need to contain any statements, which constitutes an empty
block, or it can be said to be a block containing an empty statement.
Broadly speaking, any number of consecutive sentences can be called a
block, but we prefer to expand the scope of the block as much as
possible, which can ensure that the area of the block is consistent with
the scope of the scope. In the above example, we tend to think that rows
2 to 3 are a whole block, which is the largest range between ``if``
keywords and ``end`` keywords.

``do`` Statement
^^^^^^^^^^^^^^^^

Sometimes we just want to open up a new scope, but don’t want to use any
control statements. In this case, we can use the ``do`` statement to
encapsulate the block. ``do`` The statement has no control function.
``do`` The sentence has the form

``do`` **block** ``end``

Among them **block** is the block we need. This statement uses a pair of
``do`` and ``end`` keywords to contain blocks. ``do`` The statement has
no control function, nor does it generate any runtime instructions.

conditional statement
---------------------

Berry provides ``if`` statements to realize the function of conditional
control execution. This kind of program structure is generally called
branch structure. ``if`` The statement will determine the branch of
execution based on the true (``true``) or false (``false``) conditional
expression. In some languages, there are other options for implementing
conditional control. For example, languages such as C and C++ provide
``switch`` statements, but in order to simplify the design, Berry does
not support ``switch`` statements.

``if`` Statement
~~~~~~~~~~~~~~~~

``if`` statement is used to implement the branch structure, which
selects the branch of the program according to the true or false of a
certain judgment condition. The statement can also include ``else``
branch or ``elif`` branch. The simple ``if`` statement form without
branches is

``if`` **condition**    **block** ``end``

**condition** is a conditional expression. When the value of
**condition\* is ``true``,**\ block*\* in the second line will be
executed, otherwise the **block** will be skipped and the statement
following ``end`` will be executed. In the case of **block** being
executed, after the last statement in the block is executed, it will
leave the ``if`` statement and start executing the statement following
``end``.

Here is an example to illustrate the usage of the ``if`` statement:

.. code:: berry

   if 8 % 2 == 0
       print('this number is even')
   end

This code is used to judge whether the number ``8`` is even, and if it
is, it will output ``this number is even``. Although this example is
very simple, it is enough to illustrate the basic usage of ``if``
sentences.

If you want to have a corresponding branch for execution when the
condition is met and not met, use the ``if`` statement with the ``else``
branch. ``if else`` The form of the sentence is

| ``if`` **condition**    **block**
| ``else``
| **block**
| ``end``

Different from the simple ``if`` statement, the ``if else`` statement
will execute **block** under the ``else`` branch when the value of
**condition** is ``false``. No matter which branch is executed under
**block**, after the last statement in the block is executed, the
``if else`` statement will pop out, that is, the statement after ``end``
will be executed. In other words, no matter whether the value of
**condition** is ``true`` or ``false``, one **block** will be executed.

Continue to use the judgment of parity as an example, this time change
the demand to output corresponding information according to the parity
of the input number. The code to achieve this requirement is:

.. code:: berry

   if x % 2 == 0
       print('this number is even')
   else
       print('this number is odd')
   end

Before running this code, we must first assign an integer value to the
variable ``x``, which is the number we want to check for parity. If
``x`` is an even number, the program will output
``this number is even``, otherwise it will output
``this number is odd``.Sometimes we need to nest ``if`` statements. One
way is to nest a ``if`` statement under the ``else`` branch. This is a
very common requirement because many conditions need to be judged
consecutively. For this kind of demand, use the ``if else`` statement to
write:

.. code:: berry

   if expr
       block
   else
       if expr
           block
       end
   end

Obviously, this way of writing will increase the indentation level of
the code, and it is more cumbersome to use multiple ``end`` at the end.
As an improvement, Berry provides the ``elif`` branch to optimize the
above writing. Using the ``elif`` branch is equivalent to the above
code, in the form

| ``if`` **condition**
| **block**
| ``elif`` **condition**
| **block**
| ``else``
| **block**
| ``end``

``elif`` The branch must be used after the ``if`` branch and before the
branch, and the ``elif`` branch can be used multiple times in
succession. If the **condition** corresponding to the ``elif`` branch is
satisfied, the **block** under the branch will be executed. ``elif``
Branching is suitable for situations that require multiple conditions to
be judged in sequence.

We use a piece of code that judges positive, negative, and 0 to
demonstrate the ``elif`` branch:

.. code:: berry

   if x> 0
       print('positive')
   elif x == 0
       print('zero')
   else
       print('negative')
   end

Here too, the variable ``x`` must be assigned first. This code is very
simple and will not be explained.

Some languages have a problem called dangling “``else``”, which refers
to when a ``if`` sentence is nested inside another ``if`` sentence,
where does the ``else`` branch belong? Problem with the sentence ``if``.
When using C/C++, we must consider the problem of dangling ``else``. In
order to avoid ambiguity on the problem of ``if else``, C/C++
programmers often use curly braces to make a branch into a block. In
Berry, the branch of the ``if`` statement must be a block, which also
determines that Berry does not have the problem of overhanging ``else``.

Iteration Statement
-------------------

Iterative statements are also called loop statements, which are used to
repeat certain operations until the termination condition is met. Berry
provides ``while`` statement and ``for`` two iteration statements. Many
languages also provide these two statements for iteration. Berry’s
``while`` statement is similar to the ``while`` statement in C/C++, but
Berry’s ``for`` statement is only used to traverse the elements in the
container, similar to the ``foreach`` statement provided by some
languages and the one introduced by C++11 New ``for`` sentence style.
The C-style ``for`` statement is not supported.

``while`` Statement
~~~~~~~~~~~~~~~~~~~

``while`` statement is a basic iterative statement. ``while``
statement uses a judgment condition. When the condition is true, the
loop body is executed repeatedly, otherwise the loop is ended. The
pattern of the statement is

``while`` **condition**    **block** ``end``

When the program runs to the ``while`` statement, it will check whether
the expression **condition** is true or false. If it is true, execute
the loop body **block**, otherwise end the loop. After executing the
last statement in **block**, the program will jump to the beginning of
the statement ``while`` and start the next round of detection. If the
**condition** expression is false when it is first evaluated, the loop
body **block** will not be executed at all (same as the **condition**
expression of the ``if`` statement is false).Generally speaking, the
value of **condition** expression should be able to change during the
loop, rather than a constant or a variable modified outside the loop,
which will cause the loop to not execute or fail to terminate. A loop
that never ends is called an endless loop. Usually we usually expect the
loop to execute a specified number of times and then terminate. For
example, when using the ``while`` loop to access all elements in the
array, we hope that the number of loop executions is the length of the
array, for example:

.. code:: berry

   i = 0
   l = ['a','b','c']
   while i < l.size()
       print(l[i])
       i = i + 1
   end

This loop gets the elements from the array ``l`` and prints them. We use
a variable ``i`` as the loop counter and array index. We let the value
of ``i`` reach the length of the array ``l`` to end the loop. In the
last line of the loop body, we add ``1`` to the value of ``i`` to ensure
that the next element of the array is accessed in the next loop, and the
``while`` loop ends when the number of loops reaches the length of the
array.

``for`` Statement
~~~~~~~~~~~~~~~~~

Berry’s ``for`` statement is used to traverse the elements in the
container, and its form is

| ``for`` **variable** ``:`` **expression**
| **block** ``end``

**expression** The value of the expression must be an iterable container
or function, such as the ``range`` class. ``for`` The statement obtains
an iterator from the container, and obtains an element in the container
every time through the call to the iterator.

**variable** is called an iteration variable, which is always defined in
the statement ``for``. Therefore **variable** must be a variable name
and not an expression. The container element obtained from the iterator
in each loop will be assigned to the iteration variable. This process
occurs before the first statement in **block**.

The ``for`` statement will check whether there are any unvisited
elements in the iterator for iteration. If there are, the next iteration
will start, otherwise it will end the ``for`` statement and execute the
statement following ``end``. Currently, Berry only provides read-only
iterators, which means that the elements in the container cannot be
modified through the iteration variables in the ``for`` statement.

The scope of the iteration variable **variable** is limited to the loop
body **block**, and the variable will not have any relationship with the
variable with the same name outside the scope. To illustrate this point,
let’s use an example to illustrate. In this example, we use the ``for``
statement to access all the elements in the ``rang`` instance and print
them out. Of course, we also use this example to demonstrate the scope
of loop variables.

.. code:: berry

   i = "Hi, I'm fine." # Outer variable
   for i: 0 .. 2
       print(i) # Iteration variable
   end
   print(i)

In this example, relative to the iteration variable ``i`` defined in
line 2, the variable ``i`` defined in line 1 is an external variable.
Running this example will get the following result

::

   0
   1
   2
   Hi, I'm fine

It can be seen that the iteration variable ``i`` and the external
variable ``i`` are two different variables. They just have the same name
but different scopes.

``for`` Principle of Statement
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Unlike the traditional iterative statement ``while``, the ``for``
statement uses iterators to traverse the container. If you need to use
the ``for`` statement to traverse a custom class, you need to understand
its implementation mechanism. When using the ``for`` statement, the
interpreter hides a lot of implementation details. In fact, for such
code:

.. code:: berry

   for i: 0 .. 2
       print(i)
   end

Will be translated into the following equivalent code by the
interpreter:

.. code:: berry

   var it = __iterator__(0 .. 2)
   try
       while true
           var i = it()
           print(i)
       end
   except 'stop_iteration'
       # do nothing
   end

To some extent, the ``for`` statement is just a syntactic sugar, it is
essentially just a simple way of writing a piece of complex code. In
this equivalent code, an intermediate variable ``it`` is used. The value
of the variable is an iterator. In this example, it is an iterator of
the ``range`` container ``0..2``. When processing the ``for`` statement,
the interpreter hides the intermediate variable of the iterator, so it
cannot be accessed in the code.

The parameter of function ``__iterator__`` is a container, and the
function returns an iterator of parameters. This function gets the
iterator by calling the parameter method. Therefore, if the return value
of the ``iter`` method is an instance (``instance``) type, this instance
must have a ``next`` method and a ``hasnext`` method.

The parameter of function ``__hasnext__`` is an iterator, which checks
whether the iterator has the next element by calling the ``hasnext``
method of the iterator. ``hasnext`` The return value of the method is of
type ``boolean``. The parameter of function ``__next__`` is also an
iterator, which gets the next element in the iterator by calling the
``next`` method of the iterator.

So far, the ``__iterator__``, ``__hasnext__`` and ``__next__`` functions
simply call some methods of the container or iterator and then return
the return value of these methods. Therefore, the equivalent writing of
the ``for`` statement can also be simplified into this form:

.. code:: berry

   do
       var it = (0 .. 2).iter()
       while (it.hasnext())
           var i = it.next()
           print(i)
       end
   end

This code is easier to read. It can be seen from the effective code that
the scope of the iterator variable ``it`` is the entire ``for``
statement, but it is not visible outside the ``for`` statement, while
the scope of the iteration variable ``i`` is in the loop body, so every
time Iterations will define new iteration variables.

Jump Statement
--------------

The jump statement provided by Berry is used to realize the jump of the
program flow in the loop process. Jump statements are divided into
``break`` statements and ``continue`` statements. These two statements
must be used inside iterative statements and can only be used inside
functions to jump. Some languages provide ``goto`` statements to realize
arbitrary jumps within functions, which Berry does not provide, but the
effects of ``goto`` statements can be replaced by conditional statements
and iteration statements.

``break`` Statement
~~~~~~~~~~~~~~~~~~~

``break`` Used to terminate the iteration statement and jump out. After
the execution of the ``break`` statement, the nearest level of the
iteration statement will be terminated immediately and execution will
continue from the position of the first statement after the iteration
statement. In order to illustrate the execution flow of the ``break``
statement, we use an example to demonstrate:

.. code:: berry

   while true
       print('before break')
       break
       print('after break')
   end
   print('out of the loop')

In this code, the ``break`` statement is in a ``while`` loop. Before and
after the ``break`` statement and after the ``while`` statement, we have
placed a print statement to test the execution flow of the program. The
result of this code is:

::

   before break
   out of the loop

This shows that the ``while`` statement ends the loop at the ``break``
statement position on the 3rd line and the program continues to execute
from the 6th line.

``continue`` Statement
~~~~~~~~~~~~~~~~~~~~~~

``continue`` The statement is also used inside an iteration statement.
Its function is to end an iteration and immediately start the next
round. Therefore, after the execution of the ``continue`` statement, the
remaining code in the iteration statement of the nearest layer will no
longer be executed, but a new round of iteration will start. Here we use
a ``for`` statement to demonstrate the function of the ``continue``
statement:

.. code:: berry

   for i: 0 .. 5
       if i >= 2
           continue
       end
       print('i =', i)
   end
   print('out of the loop')

Here, the ``for`` statement will iterate 6 times. When the iteration
variable ``i`` is greater than or equal to ``2``, the ``continue``
statement on line 3 will be executed, and the print statement on line 5
will not be executed thereafter. In other words, line 5 will only be
executed in the first two iterations (at this time ``i<2``). The running
result of this routine is:

::

   i = 0
   i = 1
   out of the loop

It can be seen that the value of the variable ``i`` is only printed
twice, which is in line with expectations. Readers can try to print the
value of the variable ``i`` before the ``continue`` statement. You will
find that the ``for`` statement does iterate 6 times, indicating that
the ``continue`` statement does not terminate the iteration.

``import`` Statement
--------------------

Berry has some predefined modules, such as the ``math`` module for
mathematical calculations. These modules cannot be used directly, but
must be imported with the ``import`` statement. There are two ways to
import a module:

``import`` **name**

``import`` **name** ``as`` **variable**

**name** For the name of the module to be imported, when using the first
writing method to import the module, the imported module can be called
directly by using the module name. The second way of writing is to
import a module named **name** and modify the module name when calling
it to **variable**. For example, a module named ``math``, we use the
first method to import and use:

.. code:: berry

   import math
   math.sin(0)

Here directly use ``math`` to call the module. If the name of a module
is relatively long and it is not convenient to write, you can use the
``import as`` statement. Here, assume a module named ``hardware``. We
want to call the function ``setled`` of the module, we can import the
module ``hardware`` into the variable named ``hw`` and use:

.. code:: berry

   import hardware as hw
   hw.setled(true)

To find modules, all paths in ``sys.path()`` are explored sequentially.
If you want to add a specific path before the import (like reading from
SD card) you can use the following helper function:

.. code:: berry

   def push_path(p)
     import sys
     var path = sys.path()
     if path.find(p) == nil  # append only if it's not already there
       path.push(p)
     end
   end

Exception Handling
------------------

The mechanism allows the program to capture and handle exceptions that
occur during runtime. Berry supports an exception capture mechanism,
which allows the exception capture and handling process to be separated.
That is, part of the program is used to detect and collect exceptions,
and the other part of the program is used to handle exceptions.

First of all, the problematic program needs to throw an exception first.
When these programs are in an exception handling block, a specific
program will catch and handle the exception.

Raise an exception
~~~~~~~~~~~~~~~~~~

Using the ``raise`` statement raises an exception. ``raise`` The
statement will pass a value to indicate the type of exception so that it
can be identified by a specific exception handler. Here is how to use
the ``raise`` statement:

``raise`` **exception**

``raise`` **exception**\ ``,``\ **message**

The value of the expression **exception** is the thrown **Outliers**;
the optional **message** expression is usually a string describing the
exception information, and this expression is called **Abnormal
parameter**. Berry allows any value to be used as an abnormal value, for
example, a string can be used as an abnormal value:

.. code:: berry

   raise 'my_error','an example of raise'

After the program executes to the ``raise`` statement, it will not
continue to execute the statements following it, but will jump to the
nearest exception handling block. If the most recent exception handling
block is in other functions, the functions along the call chain will
exit early. If there is no exception handling block, **Abnormal exit**
will occur, and the interpreter will print the exception error message
and the call stack of the error location.When the ``raise`` statement is
in the ``try`` statement block, the exception will be caught by the
latter. The caught exception will be handled by the ``except`` block
associated with the ``try`` block. If the thrown exception can be
handled by the ``except`` block, the execution of this block will
continue from the statement after the last ``except`` block. If all
``except`` statements cannot handle the exception, the exception will be
rethrown until it can be handled or the exception exits.

Outliers
^^^^^^^^

In Berry, you can use any value as an outlier, but we usually use short
strings. Berry may also throw some exceptions internally. We call these
exceptions **Standard exception**. All standard exception values are of
string type.

+----------------------+----------------------+----------------------+
| **Outliers**         | **Description**      | **Parameter          |
|                      |                      | Description**        |
+======================+======================+======================+
| ``assert_failed``    | Assertion failed     | Specific exception   |
|                      |                      | information          |
+----------------------+----------------------+----------------------+
| ``index_error``      | (usually out of      | Specific exception   |
|                      | bounds)              | information          |
+----------------------+----------------------+----------------------+
| ``io_error``         | IO Malfunction       | Specific exception   |
|                      |                      | information          |
+----------------------+----------------------+----------------------+
| ``key_error``        | Key error            | Specific exception   |
|                      |                      | information          |
+----------------------+----------------------+----------------------+
| ``runtime_error``    | VM runtime exception | Specific exception   |
|                      |                      | information          |
+----------------------+----------------------+----------------------+
| ``stop_iteration``   | End of iterator      | no                   |
+----------------------+----------------------+----------------------+
| ``syntax_error``     | Syntax error         |                      |
+----------------------+----------------------+----------------------+
| by the compiler      |                      |                      |
+----------------------+----------------------+----------------------+
| ``unrealized_error`` | Unrealized function  | Specific exception   |
|                      |                      | information          |
+----------------------+----------------------+----------------------+
| ``type_error``       | Type error           | Specific exception   |
|                      |                      | information          |
+----------------------+----------------------+----------------------+

Standard exception list

Catch exceptions
~~~~~~~~~~~~~~~~

Use the ``except`` statement to catch exceptions. It must be paired with
the ``try`` statement, that is, a ``try`` statement block must be
followed by one or more ``except`` statement blocks. ``try-except`` The
basic form of the sentence is

| ``try`` **block**
| ``except`` **…**   **block**   ``end``

The ``except`` branch can have the following forms

| ``except ..`` ``except`` **exceptions**
| ``except`` **exceptions**\ ``as`` **variable**
| ``except`` **exceptions**\ ``as`` **variable** ``,`` **message**
| ``except .. as`` **variable**
| ``except .. as`` **variable** ``,`` **message**

The most basic ``except`` statement does not use parameters, this
``except`` branch will catch all exceptions; **Catch exception list**
**exceptions** is a list of outliers that can be matched by the
corresponding ``except`` branch, used between multiple values in the
list Separate by commas; **variable** is **Abnormal variable**, if the
branch catches an exception, the outlier will be bound to the variable;
**message** is **Abnormal parameter variable**, if the branch catches an
exception, the abnormal parameter value will be bound To the variable.

When an exception is caught in the ``try`` statement block, the
interpreter will check the ``except`` branch one by one. If the
exception value exists in the capture list of a branch, the code block
under the branch will be called to handle the exception, and the entire
``try-except`` statement will exit after the code block is executed. If
all the ``except`` branches do not match, the exception will be
re-thrown and caught and handled by the outer exception handler.
