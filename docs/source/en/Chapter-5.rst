5. Function
===========

**function** is a “subroutine” that can be called by external code. As a
part of the program, the function itself is also a piece of code. A
function can have 0 or more parameters and will return a result, which
is called the function’s **return value**.

In Berry, the function is **first class value**. Therefore, in addition
to calling functions, you can also pass functions as values, for
example, bind functions to variables, use functions as return values,
and so on.

5.1 Basic Information
---------------------

The use of functions includes two parts: function definition and call.
The function definition statement uses the ``def`` keyword as the
beginning. The function definition is the process of packaging and
naming the code of the function body. This process only generates the
function structure and does not execute the function. The execution
function must use **call operator**, which is a pair of parentheses. The
call operator acts on an expression whose result is a function type. The
parameters passed to the function are written in parentheses, and
multiple parameters are separated by commas. The result of the calling
expression is the return value of the function.

5.1.1 Function Definition
~~~~~~~~~~~~~~~~~~~~~~~~~

Named Function
^^^^^^^^^^^^^^

**named function** is a function given a name when it is defined. Its
definition statement consists of the following parts: ``def`` keywords,
function names, lists consisting of 0 to multiple parameters, and
function bodies ( function body), multiple parameters in the parameter
list are separated by commas, and all parameters are written in a pair
of parentheses. We call the parameter when the function is defined as
**Formal parameters**, and the parameter when calling the function as
**Arguments**. The general form of the function definition is:

.. code::

      ’def’ name ´(´ arguments ´)´
        block
      ´end’

The function name **name** is an identifier; **arguments** is the formal
parameter list; **block** is the function body. If the function body is
an empty statement, the function is called an “empty function”. The
function return value statement is contained in the function body. If
there is no return statement in **block**, the function will return
``nil`` by default. The function name is actually the variable name of
the bound function object. If the name already exists in the current
scope, defining the function is equivalent to binding the function
object to this variable.

The following example defines a function named ``add``. The function of
this function is to sum two numbers and return.

.. code:: berry

   def add(a, b)
       return a + b
   end

``add`` The function has two parameters ``a`` and ``b``, and the two
addends are passed into the function through these parameters for
calculation. ``return`` The statement returns the result of the
calculation.

A function as a class attribute is called a method. This part of the
content will be explained in the object-oriented chapter.

Anonymous Function
^^^^^^^^^^^^^^^^^^

Unlike named functions, **anonymous function** has no name, and its
definition expression has the form:

.. code::

      ´def’ ´(´ arguments ´)´
        block
      ´end’

It can be seen that compared with named functions, there is no function
name in the definition of anonymous functions **name**. The definition
of an anonymous function is essentially an expression, which is called
**Function literal**. In order to use anonymous functions, we can bind
the function literal value to a variable:

.. code:: berry

   add = def (a, b)
       return a + b
   end

The function of this code is exactly the same as that of the function
``add`` in the previous section. An anonymous function can be used to
conveniently pass the function value as a literal value. Like other
types of literals, function literals are also the smallest unit of
expressions. Therefore, between ``def`` keywords and ``end`` are an
indivisible whole.

Call function
~~~~~~~~~~~~~

Take the ``add`` function as an example. To call this function, you need
to provide two values, and you can get the sum of the two numbers by
calling the function:

.. code:: berry

   res = add(5, 3)
   print(res) # 8

We call the called function (the ``add`` function in the example) as
**Called function**, and the function that calls the called function
(the ``main`` function in the example) as **Key function**. The function
call process is as follows: First, the interpreter will (implicitly)
initialize the formal parameter list of the called function with the
argument list, and at the same time suspend the calling function and
save its state, then create an environment for the called function and
execute the called function. function.

The function will end its execution when it encounters the ``return``
statement and pass the return value to the calling function. The
interpreter will destroy the environment of the called function after
the called function returns, then restore the environment of the calling
function and continue to execute the calling function. The return value
of the function is also the result of the function call expression. The
following example defines a function ``square`` and binds this function
to a variable ``f``, and then calls the function ``square`` through the
variable ``f``. This usage is similar to function pointers in C
language.

.. code:: berry

   def square(n)
       return n * n
   end
   f = square
   print(f(5)) # 25

It should be noted that the function object is only bound to these
variables (refer to section [section::assign_operator]) and cannot be
modified, so reassigning the variable corresponding to the function name
will not make the function lose:

.. code:: berry

   f = square
   square = nil
   print(f(5)) # 25

It can be seen that the function can still be called normally after
reassigning ``square``. Only after the function object is no longer
bound to any variable will it be lost, and the resources occupied by
this type of function object will be recycled by the system.

Forward call
^^^^^^^^^^^^

The call of the function must be in the scope of the function variable,
so it usually cannot be called before the function is defined. In order
to solve this problem, you can use this method to compromise:

.. code:: berry

   var func1
   def func2(x)
       return func1(x)
   end
   def func1(x)
       return x * x
   end
   print(func2(4)) # 16

In this example, ``func2`` calls ``func1``, but the function ``func1``
is defined after ``func2``. After executing this code, the program will
output the correct result ``16``. This routine uses the mechanism that
the function will not be called when the function is defined. Define the
variable ``func1`` before defining ``func2`` to ensure that the symbol
``func1`` will not be found during compilation. Then we define the
function ``func1`` after ``func2`` so that the function will be used to
overwrite the value of the variable ``func1``. When the function
``func2`` is called in the last line ``print(func2(4))``, the variable
``func1`` is already the function we need, so the correct result will be
output.

Recursive call
^^^^^^^^^^^^^^

**recursive function** refers to functions that call themselves directly
or indirectly. Recursion refers to a strategy that divides the problem
into similar sub-problems and then solves them. Taking factorial as an
example, the recursive definition of factorial is
0! = 1, *n*! = *n* ⋅ (*n*\ −1)!, we can write the recursive function for
calculating factorial according to the definition:

.. code:: berry

   def fact(n)
       if n == 0
           return 1
       end
       return n * fact(n-1)
   end

Take the factorial of 5 as an example, the process of manually
calculating the factorial of 5 is: 5! = 5 × 4 × 3 × 2 × 1 = 120 The
result of calling the ``fact`` function is also 120:

.. code:: berry

   print(fact(5)) # 120

In order to ensure that the depth of the recursive call is limited (too
deep recursion level will exhaust the stack space), the recursive
function must have an end condition. ``fact`` The ``if`` statement in
the second line of the function definition is used to detect the end
condition, and the recursive process ends when ``n`` is calculated as
``0``. The above factorial formula does not apply to non-integer
parameters. Executing an expression like ``fact(5.1)`` will cause a
stack overflow error due to the inability to end the recursion.

There is another situation ``Indirect recursion``, that is, the function
is not called by itself but by another function (directly or indirectly)
called by it. Indirect recursion usually requires the use of forward
function call techniques. Take the functions ``is_odd`` and ``is_even``
for calculating odd and even numbers as examples:

.. code:: berry

   var is_odd
   def is_even(n)
       if n == 0
           return true
       end
       return is_odd(n-1)
   end
   def is_odd(n)
       if n == 0
           return false
       end
       return is_even(n-1)
   end

These two functions call each other. In order to ensure that this name
is in the scope when calling the function ``is_odd`` on line 6, the
variable ``is_odd`` is defined on line 1.

Anonymous function call
^^^^^^^^^^^^^^^^^^^^^^^

If an anonymous function will only be called once, the easiest way is to
call it when it is defined, for example:

.. code:: berry

   res = def (a, b) return a + b end (1, 2) # 3

In this routine, we use the call expression directly after the function
literal to call the function. This usage is very suitable for functions
that will only be called in one place.

You can also bind an anonymous function to a variable and call it:

.. code:: berry

   add = def (a, b) return a + b end
   res = add(1, 2) # 3

This usage is similar to the call of a named function, essentially
calling the variable bound to the function value. It should be noted
that it is more difficult to make recursive calls to anonymous
functions, unless you use forward call techniques.

Formal and actual parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The function uses actual parameters to initialize the formal parameters
when it is called. Under normal circumstances, the actual parameter and
the shape parameter are equal and the positions correspond to each
other, but Berry also allows the actual parameter to be unequal to the
formal parameter: if the actual parameter is more than the formal
parameter, the extra actual parameter will be discarded. Less than the
formal parameters will initialize the remaining formal parameters to
``nil``.

The process of parameter passing is similar to assignment operation. For
``nil``, ``boolean`` and numeric types, parameter passing is by value,
while other types are by reference. For the writable pass-by-reference
type such as instance, modifying them in the called function will also
modify the object in the calling function. The following example
demonstrates this feature:

.. code:: berry

   var l = [], i = 0
   def func(a, b)
       a.push(1)
       b ='string'
   end
   func(l, i)
   print(l, i) # [1] 0

It can be seen that the value of variable ``l`` has changed after
calling function ``func``, but the value of variable ``i`` has not
changed.

Function with variable number of arguments (vararg)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can define a function to take any arbitrary number of arguments and
iterate on them. For example ``print()`` takes any number of arguments
and prints each of them separated by spaces. You need to define the last
argument as a capture-all-arguments using ``*`` before its name.

All arguments following the formal arguments are grouped at runtime in a
``list`` instance. If no arguments are captured, the list is empty.

Example:

.. code:: berry

   def f(a, b, *c) return size(c) end
   f(1,2) # returns 0, c is []
   f(1,2,3) # returns 1, c is [3]
   f(1,2,3,4) # returns 2, c is [3,4]

Calling a function with dynamic number of arguments
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Berry syntax allows only to call with a fixed number of arguments. Use
the ``call(f, [args])`` function to pass any arbitrary number or
arguments.

You can statically add any number of arguments to ``call()``. If the
last argument is a ``list``, it is automatically expanded to discrete
arguments.

Example:

.. code:: berry

   def f(a,b) return nil end

   call(f,1)        # calls f(1)
   call(f,1,2)      # calls f(1,2)
   call(f,1,2,3)    # calls f(1,2,3), last arg is ignored by f
   call(f,1,[2,3])  # calls f(1,2,3), last arg is ignored by f
   call(f,[1,2])    # calls f(1,2)
   call(f,[])       # calls f()

You can combine ``call`` and vararg. For example let’s create a function
that acts like ``print()`` but converts all arguments to uppercase.

Full example:

.. code:: berry

   def print_upper(*a)  # take arbitrary number of arguments, args is a list
       import string
       for i:0..size(a)-1
           if type(a[i]) == 'string'
               a[i] = string.toupper(a[i])
           end
       end
       call(print, a)   # call print with all arguments
   end

   print_upper("a",1,"Foo","Bar")  # prints: A 1 FOO BAR

Functions and local variables
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The function body itself is a scope, so the variables defined in the
function are all local variables. Unlike directly nested blocks, every
time a function is called, space is allocated for local variables. The
space for local variables is allocated on the stack, and the allocation
information is determined at compile time, so this process is very fast.
When multiple levels of scope are nested in a function, the interpreter
allocates stack space for the scope nesting chain with the most local
variables, rather than the total number of local variables in the
function.

``return`` Statement
~~~~~~~~~~~~~~~~~~~~

``return`` The statement is used to return the result of a function,
that is, the return value of the function. All functions in Berry have a
return value, but you can not use any ``return`` statement in the
function body. At this time, the interpreter will generate a default
``return`` statement to ensure that the function returns. ``return``
There are two ways to write sentences:

.. container:: algorithm

   ::

      ´return’
      ´return’ expression

The first way of writing is to write only the ``return`` keyword and not
the expression to be returned. In this case, the default ``nil`` value
is returned. The second way of writing is to follow the expression
**expression** after the ``return`` keyword, and the value of the
expression will be used as the return value of the function. When the
program executes to the ``return`` statement, the currently running
function will end execution and return to the code that called the
function to continue running.

When using a separate keyword ``return`` as the return statement of a
function, it is easy to cause ambiguity. At this time, it is recommended
to add a semicolon after ``return`` to prevent errors:

.. code:: berry

   def func()
       return;
       x = 1
   end

In this example, the ``x = 1`` statement after the ``return`` statement
will not be executed, so it is redundant. If this kind of redundant code
is avoided, the ``return`` statement is usually followed by keywords
such as ``end``, ``else`` or ``elif``. In this case, even if a separate
``return`` statement is used, there is no need to worry about ambiguity.

closure
-------

Basic Concepts
~~~~~~~~~~~~~~

As mentioned earlier, functions are the first type of value in Berry.
You can define functions anywhere, and you can also pass functions as
parameters or return values. When another function is defined in a
function, the nested function can access the local variables of any
outer function. We call the “local variables of the outer function” used
in the function the function **Free variable**. The generalized free
variables also include global variables, but there is no such rule in
Berry.\ **Closure** is a technique that binds functions to
**environments**. The environment is a mapping that associates each free
variable of a function with a value. In terms of implementation,
closures associate the function prototype with its own variables.
Function prototypes are generated at compile time, and environment is a
runtime concept, so closures are also dynamically generated at runtime.
Each closure binds the function prototype to the environment when it is
generated, for example, in the following example:

.. code:: berry

   def func(i) # The outer function
       def foo() # The inner function (closure)
           print(i)
       end
       foo()
   end

The inner function ``foo`` is a closure, which has a free variable
``i``, which is a parameter of the outer function ``func``. When the
closure ``foo`` is generated, its function prototype is bound to the
environment containing the free variable ``i``. When the variable
``foo`` leaves the scope, the closure will be destroyed. Usually, the
inner function will be the return value of the outer function, for
example:

.. code:: berry

   def func(i) # The outer function
       return def () # Return a closure (anonymous function)
           print(i)
           i = i + 1
       end
   end

The closure returned here is an anonymous function. When the closure is
returned by the outer function, the local variables of the outer
function will be destroyed, and the closure will not be able to directly
access the variables in the original outer function. The system will
copy the value of the free variable to the environment when the free
variable is destroyed. The life cycle of these free variables is the
same as the closure, and can only be accessed by the closure. The
returned function or closure will not be executed automatically, so we
need to call the closure returned by the function ``func``:

.. code:: berry

   f = func(0)
   f()

This code will output ``0``. If we continue to call the closure ``f``,
we will get the output ``1``, ``2``, ``3``\ … This may not be well
understood: variable [2.198 ] Is destroyed after the function ``func``
returns, and as a free variable of the closure ``f``, ``i`` will be
stored in the closure environment, so every time ``f`` is called, the
value of ``i`` will be added to 1 (``func`` function definition line 4).

Use of closures
^^^^^^^^^^^^^^^

Closures have many uses. Here are a few common uses:

Lazy evaluation
'''''''''''''''

The closure does not do anything until it is called.

Function private communication
''''''''''''''''''''''''''''''

You can let some closures share free variables, which are only visible
to these closures, and communicate between functions by changing the
values of these free variables. This can avoid the use of external
variables.

Generate multiple functions
'''''''''''''''''''''''''''

Sometimes we may need to use multiple functions, these functions may
only have different values of some variables. We can implement a
function and then use these different variables as function parameters.
A better way is to return the closure through a factory function, and
use these possibly different variables as free variables of the closure,
so that you don’t always have to write those parameters when calling the
function, and any number of similar functions can be generated.

Simulate private members
''''''''''''''''''''''''

Some languages support the use of private members in objects, but
Berry’s class does not support private members. We can use the free
variables of closures to simulate private members. This use is not the
original intention of designing closures, but nowadays, this “misuse” of
closures is very common.

Cache result
''''''''''''

If there is a function that is very time-consuming to run, it will take
a lot of time to call it every time. We can cache the result of this
function, look it up in the cache before calling the function, and
return the cached value if found, otherwise call the function and update
the cached value. We can use closures to save the cached value so that
it will not be exposed to the outer scope, and the cached result will be
retained (until the closure is destroyed).

Binding free variables
~~~~~~~~~~~~~~~~~~~~~~

If multiple closures bind the same free variable, all closures will
always share this free variable. E.g:

.. code:: berry

   def func(i) # The outer function
       return [# Return a closure list
           def () # The closure #1
               print("closure 1 log:", i)
               i = i + 1
           end,
           def () # The closure #2
               print("closure 2 log:", i)
               i = i + 1
           end
       ]
   end

The function ``func`` in this example returns two closures through a
list, and these two closures share free variables ``i``. If we call
these closures:

.. code:: berry

   f = func(0)
   f[0]() # closure 1 log: 0
   f[1]() # closure 2 log: 1

As you can see, we updated the free variable ``i`` when we called the
closure ``f[0]``, and this change affected the result of calling the
closure ``f[1]``. This is because if a free variable is used by multiple
closures, there is only one copy of the free variable, and all closures
have a reference to the free variable entity. Therefore, any
modification to the free variable is visible to all closures that use
the free variable.

Similarly, before the local variables of the outer function are
destroyed, modifying the value of the free variable will also affect the
closure:

.. code:: berry

   def func()
       i = 0
       def foo()
           print(i)
       end
       i = 1
       return foo
   end

In this example, we change the value of the variable ``i`` (which is the
free variable of the closure ``foo``) from ``0`` to ``1`` before the
outer function ``func`` returns, then we call the closure afterwards The
value of the free variable ``i`` when the package ``foo`` is also ``1``:

.. code:: berry

   func()() # 1

Create closure in loop
~~~~~~~~~~~~~~~~~~~~~~

When constructing a closure in the loop body, you may not want the free
variables of the closure to change with the loop variables. Let’s first
look at an example of creating a closure in a loop ``while``:

.. code:: berry

   def func()
       l = [] i = 0
       while i <= 2
           l.push(def () print(i) end)
           i = i + 1
       end
       return l
   end

In this example, we construct a closure in a loop and put this closure
in a ``list``. Obviously, when the loop ends, the value of the variable
``i`` will be ``3``, and all the closures in the list ``l`` are also
references using this variable. If we execute the closure returned by
``func`` we will get the same result:

.. code:: berry

   res = func()
   res[0]() # 3
   res[1]() # 3
   res[2]() # 3

If we want each closure to refer to different free variables, we can
define another layer of functions, and then bind the current loop
variables with the function parameters:

.. code:: berry

   def func()
       l = [] i = 0
       while i <= 2
           l.push(def (n)
               return def () print(n) end
           end (i))
           i = i + 1
       end
       return l
   end

To help understand this seemingly incomprehensible code, we focus on the
code from lines 4 to 6:

.. code:: berry

   def (n)
       return def ()
           print(n)
       end
   end (i)

Here actually defines an anonymous function and calls it immediately.
The function of this temporary anonymous function is to bind the value
of the loop variable ``i`` to its parameter ``n``, and the variable
``n`` is also what we need to close The free variables of the package,
so that the free variables bound to the closure constructed during each
loop are different. Now we will get the desired output:

.. code:: berry

   res = func()
   res[0]() # 0
   res[1]() # 1
   res[2]() # 2

There are some ways to solve the problem of loop variables as free
variables. A slightly simpler way is to define a temporary variable in
the loop body:

.. code:: berry

   def func()
       l = [] i = 0
       while i <= 2
           temp = i
           l.push(def () print(temp) end)
           i = i + 1
       end
       return l
   end

Here ``temp`` is a temporary variable. The scope of this variable is in
the loop body, so it will be redefined every time it loops. We can also
use the ``for`` statement to solve the problem:

.. code:: berry

   def func()
       l = []
       for i: 0 .. 2
           l.push(def () print(i) end)
       end
       return l
   end

This may be the simplest way. ``for`` The iteration variable of the
statement will be created in each loop. The principle is similar to the
previous method.

Lambda expression
-----------------

**Lambda expression** is a special anonymous function. Lambda expression
is composed of parameter list and function body, but the form is
different from general function:

.. code::

   ´/´ args ´->´ expr ´end’

**args** is the parameter list, the number of parameters can be zero or
more, and multiple parameters are separated by commas or spaces (cannot
be mixed at the same time); **expr** is the return expression, the
lambda expression will return the expression value. Lambda expressions
are suitable for implementing functions with very simple functions. For
example, the lambda expression for judging the size of two numbers is:

.. code:: berry

   / a b -> a < b

This is easier than writing a function of the same function. In some
general sorting algorithms, this type of size comparison function may
need to be used extensively. Using lambda expressions can simplify the
code and improve readability.

Like general functions, lambda expressions can form closures. Lambda
expressions are called in the same way as ordinary functions. If you use
the immediate calling method similar to anonymous functions:

.. code:: berry

   lambda = / a b -> a < b
   result = lambda(1, 2) # normal calling
   result = (/ a b -> a < b)(1, 2) # direct calling

Since the function call operator has a higher priority, a pair of
parentheses should be added to the lambda expression when making a
direct call, so that it will be called as a whole.
