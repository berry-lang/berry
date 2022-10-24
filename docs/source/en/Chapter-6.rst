6. Object Oriented Function
===========================

For optimization considerations, Berry did not consider simple types as
objects. These simple types include ``nil`` types, numeric types,
boolean types, and string types. But Berry provides classes to implement
the object mechanism. Among Berry’s basic data types, ``list``, ``map``
and ``range`` are class objects. An object is a collection containing
data and methods, where data is composed of some variables, and methods
are functions. The type of an object is called a class, and the entity
of an object is called an instance.

6.1 Class and instance
----------------------

6.1.1 Class declaration
~~~~~~~~~~~~~~~~~~~~~~~

To use a class, you must first declare it. The declaration of a class
starts with the keyword ``class``. The member variables and methods of
the class must be specified in the declaration. This is an example of
declaring a class:

.. code:: berry

   class person
       static var majority = 18
       var name, age
       def init(name, age)
           self.name = name
           self.age = age
       end
       def tostring()
           return 'name: '+ str(self.name) + ', age:' + str(self.age)
       end
       def isadult()
           return self.age >= self.majority
       end
   end

Class member variables are declared with keyword ``var``, while member
methods are declared with keyword ``def``. Currently, Berry does not
support initializing member variables at the time of definition, so the
initialization of member variables should be done by the constructor.
The properties of the class cannot be modified after the declaration is
completed, so the class is a read-only object.

This design is to ensure that the class can be statically constructed in
the C language when the interpreter is implemented and the ``const``
property can be used Modified to save RAM

Berry’s class does not support access restrictions, and all properties
of the class are visible to the outside. In native classes, you can use
some tricks to make properties invisible to Berry code (usually let the
member name start with dot “.”). You can use some conventions to
restrict access to the members of the class, such as the convention that
the attributes starting with an underscore are private attributes. This
convention does not have any use at the grammatical level, but is
conducive to the logical structure of the code.

Instantiate
~~~~~~~~~~~

The class itself is just an abstract description. Taking cars as an
example, I know the concept of cars, and when we really want to use
cars, we need real cars. The use of classes is similar. We will not only
use this abstract description, but need to produce a concrete object
based on this description. This process is called **Instantiation of the
class**, abbreviated as instantiation, and the concrete object produced
by instantiation is called **Instance**. The class itself does not have
data, and instantiation produces an instance based on the information
described by the class and gives the instance specific data.

Method and ``self`` Parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Class methods are essentially functions. Unlike ordinary functions,
methods implicitly pass in a ``self`` parameter, and is always the first
parameter, which stores a reference to the current instance. Due to the
existence of ``self`` parameters, the number of parameters of the method
will be one more than the number of parameters defined in the
declaration. Here we use a simple example to demonstrate:

.. code:: berry

   class Test
       def method()
           return self
       end
   end
   object = Test()
   print(object)
   print(object.method())

This example defines a ``Test`` class, which has a ``method`` method,
which returns its ``self`` parameter. The last two lines in the routine
print the value of the instance ``object`` of the ``Test`` class and the
return value of the method ``method`` respectively. The running result
of this example is:

::

   <instance: Test()>
   <instance: Test()>

It can be seen that the ``self`` parameter of the method and the name of
the use instance (``object`` in the example) both represent the same
object, and they are both instance references. Use ``self`` to access
the members or attributes of the instance in the method.

Synthetic methods
~~~~~~~~~~~~~~~~~

You can declare synthetic dynamic members and methods using the
**Virtual members** as described in Chapter 8.2.

Class Variables ``static``
~~~~~~~~~~~~~~~~~~~~~~~~~~

Variables or functions can be declared ``static``. Static variables have
the same value for all instances of the same class. They are declare as
``static a = 1`` or ``static var a = 1``. Static variables are
initialized right after the creation of the class.

Class Methods ``static``
~~~~~~~~~~~~~~~~~~~~~~~~

Methods can be declared ``static`` which means that they act like
regular function and do not take ``self`` as first argument. Within
static methods, there is no implicit ``self`` variable declared. Static
methods can be called via the class or via an instance.

.. code:: berry

   class static_demo
       static def increment_static(i)
           return i + 1
       end
       def increment_instance(i)
           return i + 1
       end
   end
   a = static_demo()
   static_demo.increment_static(1)    # call via class

2

.. code:: berry

   a.increment_static(1)              # call via instance
   static_demo.increment_instance(1)

| type_error: unsupported operand type(s) for +: 'nil' and 'int'
|     stack traceback:
|     stdin:6: in function `increment_instance`
| stdin:1: in function `main`
|

.. code:: berry
   
   a.increment_instance(1)

2

Constructor and Destructor
~~~~~~~~~~~~~~~~~~~~~~~~~~

Constructor
^^^^^^^^^^^

The constructor of the class is the ``init`` method. The constructor is
called when the class is instantiated. Therefore, the constructor is
generally used for member initialization, for example:

.. code:: berry

   class Test
       var a
       def init()
           self.a ='this is a test'
       end
   end

The constructor in this example initializes the ``a`` member of the
``Test`` class to the string ``’this is a test’``. If we instantiate the
class, we can get the value of member ``a``:

.. code:: berry

   print(Test().a) # this is a test

Destructor
^^^^^^^^^^

The destructor of the class is the ``deinit`` method. The destructor is
called when the instance is destroyed. The destructor is generally used
to complete some cleanup work. Because the garbage collection mechanism
automatically releases the memory of useless objects, there is no need
to release the memory in the destructor (and there is no way to release
the memory in the destructor). In most cases, there is no need to use a
destructor, unless a certain class requires certain processing when it
is destroyed. A typical example is that a file object must close the
file when it is destroyed.

Class inheritance
-----------------

Berry only supports single inheritance, that is, a class can only have
one base class, and the base class uses the operator ``:`` to declare:

.. code:: berry

   class Test: Base
       ...
   end

Here the ``Test`` class inherits from the ``Base`` class. The subclass
will inherit all the methods and properties of the base class, and you
can override them in the subclass. This mechanism is called
**Overload**. Under normal circumstances, we will only overload methods,
not properties.

The inheritance mechanism of the Berry class is relatively simple.
Subclasses will contain references to the base class, and instance
objects are similar. When instantiating a class with a base class,
multiple objects are actually generated. These objects will be chained
together according to the inheritance relationship, and finally we will
get the instance object at the end of the inheritance chain.

Method Overload
---------------

**Overload** means that the subclass and the base class use the same
name method, and the subclass method will override the mechanism of the
base class method. To be precise, member variables can also be
overloaded, but this overloading has no meaning. Method overloading is
divided into ordinary method overloading and operator overloading.

Common method overload
~~~~~~~~~~~~~~~~~~~~~~

Operator Overloading
~~~~~~~~~~~~~~~~~~~~

You can use the operator overloading of the class to make the instance
support the operation of the built-in operator. For example, for a class
overloaded with the addition operator, we can use the addition operator
to perform operations on the instance. An overloaded operator is a
method with a special name, and the overloaded function form of a binary
operator is

.. code::

   ´def’ operator ´(´ other ´)´
        block
   ´end’

**operator** is an overloaded binary operator. The left operand of the
binary operator is the ``self`` object, and the right operand is the
value of the parameter **other**. The overloaded function form of the
unary operator is

.. code::

   ´def’ operator ´()´
        block
   ´end’

**operator** is an overloaded unary operator. To distinguish it from the
subtraction operator, the unary minus sign is written as ``-*`` when
overloaded. Operator overloaded functions should have a return value,
because the default ``nil`` return value is usually not the expected
result. Let’s take an integer class as an example to illustrate the use
of operator overloading. First define the ``integer`` class:

.. code:: berry

   class integer
       var value
       def init(v)
           self.value = v
       end
       def +(other)
           return integer(self.value + other.value)
       end
       def *(other)
           return integer(self.value * other.value)
       end
       def -*()
           return integer(-self.value)
       end
       def tostring(other)
           return str(self.value)
       end
   end

The ``integer`` class overloads the plus, multiplication, and symbolic
operators, and the ``tostring`` method is to make the instance use the
``print`` function to output the result. We can use a simple line of
code to test the operator overloading function of the class:

.. code:: berry

   integer(1) + integer(2) * -integer(3) # -5

The result of this line of code is an instance of ``integer``. The value
of the ``value`` member of this instance is ``-5``, which is the same as
the result of the same four arithmetic operations on integers.

Logical operators cannot be overloaded directly. If you need an instance
to support logical operations, you must implement the ``tobool`` method.
The method has no parameters and the return value must be of Boolean
type. The logic operation of the instance is actually realized by
converting the instance into a Boolean value, so the logic operation of
the instance is completely in line with the nature of the general logic
operation. The subscript operator is not directly overloaded, but is
implemented by the methods ``item`` and ``setitem``. ``item`` The method
is used for subscript reading, its first parameter is the subscript
value, and the return value is the result of the subscript operation;
``setitem`` is used for subscript writing, and its first parameter is
the subscript Value, the second parameter is the value to be written,
this method does not use the return value.

The overloaded operator can be assigned any meaning, even not satisfying
the usual properties of operators. Considering the versatility of the
code and the difficulty of understanding, it is not recommended that
users give overloaded operators a function far from the general meaning.

Overload of compound assignment operator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The compound assignment operator cannot be directly overloaded, but we
can achieve the purpose of “overloading” the compound assignment
operator by overloading the binary operator corresponding to the
compound assignment operator. For example, after overloading the “``+``”
operator, you can use the “``+=``” operator for instances of related
classes. It is worth noting that the use of compound assignment
operations on the instance will cause the variables of the bound
instance to lose their reference to the instance.

.. code:: berry

   class integer
       var value
       def init(x)
           self.value = x
       end
       def +(other)
           return integer(self.value + other.value)
       end
   end
   a = integer(4) # a: <instance: 0x55edff400a78>
   a += integer(5) # a: <instance: 0x55edff4011b8>
   print(a.value) # 9

After the 11th line of code is executed, the instance bound in the
variable ``a`` has actually changed. This line of code is equivalent to
``a = integer(4) + integer(5)``. If the binary operator of the class
overload does not modify the state of the instance, then the
corresponding compound assignment operator will not modify any instance
(it may generate new instances).

Instance
--------

**Instance** is an object generated after class instantiation. A class
can be instantiated multiple times to generate different instances.
Berry instances are referenced by the class they belong to and the
corresponding data fields. All instances of a class will refer to this
class, but the data fields of these instances are independent of each
other.

Access base class object
~~~~~~~~~~~~~~~~~~~~~~~~

The built-in function ``super`` is used to access super class objects.
It can be used on classes or instances.

Magic happens when you call a method from the superclass so that it
behaves like you intuitively think it would. For example, the common
pattern for ``init()`` is as follows:

.. code:: berry

   def init(<args>)
       # do stuff before super init
       super(self).init(<args>)
       # do stuff after super init
   end

Note that classes always contains an implicit ``init()`` methods that
does nothing, so you can always call init from super class even if no
``init()`` method was declared.

Full example:

.. code:: berry

   class A
       var val
       def init(val)
           # super(self).init(val)    # this would be valid but useless
           self.val = val
       end
       def tostring()
           return "val=" + str(self.val)
       end
   end

   class B: A
       var magic    # true if value is 42
       def init(val)
           super(self).init(val)     # call super init
           self.magic = (val == 42)
       end
       def tostring()
           if self.magic
               return "magic!"
           else
               return super(self).tostring()
           end
       end
   end

   ####### Example of usage

   > b1 = B(1)
   > b1
   val=1
   > b42 = B(42)
   > b42
   magic!

**Advanced features** When calling ``super(self).<method>(<args>)`` some
magic happens. When the super-method is called, the ``self`` arguments
refers to the lowest more specific class. However the ``<method>`` if
searched not from the class of ``self`` (which is always the lowest),
but from the super class of the class containing the method currently
running.

Example:

.. code:: berry

   > class A
         def init()
             print("In A::init, self is of type", classname(self))
         end
     end
   > class B:A
         def init()
             print("In B::init, self is of type", classname(self))
             super(self).init()
         end
     end
   > class C:B
         def init()
             print("In C::init, self is of type", classname(self))
             super(self).init()
         end
     end
   > c = C()
   In C::init, self is of type C
   In B::init, self is of type C
   In A::init, self is of type C
   > 

Explanation:

-  calling ``C:init()`` on ``instance<C>``
-  in ``C:init()`` ``self`` is ``instance<C>``, ``super(self).init()``
   refers to the super class of ``C`` (current method) i.e. ``B``, so
   ``B:init()`` is called with ``instance<C>`` argument
-  in ``B:init()`` ``self`` is ``instance<C>``, ``super(self).init()``
   refers to the super class of ``B`` (current method) i.e. ``A``, so
   ``A:init()`` is called with ``instance<C>`` argument
-  in ``A:init()`` ``self`` is ``instance<C>``, print and return

Note: for backwards compatibility, super can take a second argument
``super(instance, class)`` to specify the class where to resolve the
method. This feature should not be used anymore as it is error-prone.
