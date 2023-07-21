3. Expression
=============

3.1 Basics
----------

An expression (Statement) is composed of one to more operands and
operators, and a result can be obtained by evaluating the expression.
This result is called the value of the expression. The operand can be a
literal value, variable, function call or sub-expression, etc. Simple
expressions and operators can also be combined into more complex
expressions. Similar to the four arithmetic operations, the precedence
of operators affects the evaluation order of expressions. The higher the
precedence of the operator, the earlier the expression is evaluated.

Operators and expressions
~~~~~~~~~~~~~~~~~~~~~~~~~

Berry provides some unary operators (Unary Operator) and binary
operators (Binary Operator). For example, the logical AND operator
``&&`` is a binary operator, and the logical negation operator ``!`` is
a unary operator. Some operators can be either unary operators or binary
operators. The specific meaning of such operators depends on the
context. For example, operator ``-`` is a unary symbol in expression
``-1``, but it is a binary minus sign in expression ``1-2``.

Operator combination expression
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Both the left and right sides of a binary operator can be
subexpressions, so you can use binary operators to combine expressions.
A more complex expression often has multiple operators and operands. At
this time, the order of evaluation of each sub-expression in the
expression may affect the value of the expression. The precedence and
associativity of operators guarantee the uniqueness of the expression
evaluation order. For example, a combined expression:

.. code:: python

   1 + 10/2 * 3

The four arithmetic operations in daily use will first calculate the
division expression ``10/2``, then the multiplication expression, and
finally the addition expression. This is because multiplication and
division have higher priority than addition.

operand type
^^^^^^^^^^^^

In the operation of expressions, the operands may have types that do not
match the operators. In addition, binary operators usually require the
left and right operands to be of the same type. The expression
``’10’+10`` is wrong. You cannot add a string to an integer. The problem
with the expression ``-’b’`` is that you cannot take a negative value on
a string. Sometimes a binary operator has different operand types but
can perform operations. For example, when adding an integer to a real
number, the integer object will be converted to a real number and added
to another real number object. The logical AND and logical OR operators
allow the operands on both sides of the operator to be of any type. In
logical expressions, they will always be converted to the ``boolean``
type according to certain rules.

Another situation is that operators can be overloaded when using custom
classes. In essence, you can interpret this operator arbitrarily, and it
is up to you to decide what type of its operand should be.

3.1.1 Priority and associativity
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In a compound expression composed of multiple operators, the precedence
and associativity of the operators determine the order of evaluation of
the expressions. The precedence and associativity of each operator are
given in Table 1.1.

The precedence specifies the order of evaluation between different
operators, and expressions with higher precedence operators will be
evaluated first. For example, the process of evaluating the expression
``1+2*3`` will first calculate the result of ``2*3``, and then the
result of the addition expression. Using parentheses can improve the
evaluation order of low-priority expressions. For example, in the
evaluation of expression ``(1+2)*3``, the result of expression ``1+2``
in the parentheses is calculated first, and then the multiplication
expression outside the parentheses is calculated.

Associativity refers to the evaluation order of the operands on both
sides of the operator, where the operands may be subexpressions. For
example, in the addition expression ``expr1 + expr2``, the value of
``expr1`` is calculated first and then the value of ``expr2``, because
the addition operator is left-associative.

.. container::
   :name: tab::operator_list

   +--------------+----------------+----------------+-----------------+
   | **priority** | **Operator**   | **Description**|**Associativity**|
   +==============+================+================+=================+
   | 1            | ``()``         | Grouping       | -               |
   |              |                | symbol         |                 |
   +--------------+----------------+----------------+-----------------+
   | 2            | ``() [] .``    | Field          | left            |
   |              |                | operation      |                 |
   +--------------+----------------+----------------+-----------------+
   | 3            | ``- ! ~``      | Negative sign, | left            |
   |              |                | logical        |                 |
   |              |                | negation, bit  |                 |
   |              |                | flip           |                 |
   +--------------+----------------+----------------+-----------------+
   | 4            | ``* / %``      | M              | left            |
   |              |                | ultiplication, |                 |
   |              |                | division, and  |                 |
   |              |                | remainder      |                 |
   +--------------+----------------+----------------+-----------------+
   | 5            | ``+ -``        | Addition,      | left            |
   |              |                | subtraction    |                 |
   +--------------+----------------+----------------+-----------------+
   | 6            | ``<< >>``      | Move left,     | left            |
   |              |                | move right     |                 |
   +--------------+----------------+----------------+-----------------+
   | 7            | ``&``          | Bitwise AND    | left            |
   +--------------+----------------+----------------+-----------------+
   | 8            | ``^``          | Bitwise XOR    | left            |
   +--------------+----------------+----------------+-----------------+
   | 9            | ``\|``         | Bitwise OR     | left            |
   +--------------+----------------+----------------+-----------------+
   | 10           | ``..``         | Concatenation  | left            |
   |              |                | operator       |                 |
   +--------------+----------------+----------------+-----------------+
   | 11           | ``< <= > >=``  | Greater than,  | left            |
   |              |                | greater than   |                 |
   |              |                | or equal to    |                 |
   +--------------+----------------+----------------+-----------------+
   | 12           | ``== !=``      | Equal to, not  | left            |
   |              |                | equal to       |                 |
   +--------------+----------------+----------------+-----------------+
   | 13           | ``&&``         | Logical AND    | left            |
   +--------------+----------------+----------------+-----------------+
   | 14           | ``\|\|``       | Logical OR     | left            |
   +--------------+----------------+----------------+-----------------+
   | 15           | ``? :``        | Conditional    | right           |
   |              |                | operator       |                 |
   +--------------+----------------+----------------+-----------------+
   | 16           | ``&= \|        | Assignment     | right           |
   |              | = ^= <<= >>=`` |                |                 |
   +--------------+----------------+----------------+-----------------+

   Operator list

Use brackets to increase priority
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Parentheses can be used when we need operators with lower precedence to
be evaluated first. During expression evaluation, the value of the
expression in parentheses is calculated first. In other words, for the
entire expression, the expression in parentheses is equivalent to an
operand, regardless of the composition of the expression in parentheses.

3.2 Operator
------------

3.2.1 Arithmetic Operators
~~~~~~~~~~~~~~~~~~~~~~~~~~

Arithmetic operators are used to implement arithmetic operations. These
operators are similar to the mathematical symbols we usually use. The
arithmetic operators provided by Berry are shown in Table 1.2.

.. container::
   :name: tab::arthmetic_operator

   ============ ========================= ===============
   **Operator** **Description**           **Example**
   ============ ========================= ===============
   ``-``        Unary minus               ``- expr``
   ``+``        Plus/string concatenation ``expr + expr``
   ``-``        Minus sign                ``expr - expr``
   ``*``        Multiplication sign       ``expr * expr``
   ``/``        Division sign             ``expr / expr``
   ``%``        Take the remainder        ``expr % expr``
   ============ ========================= ===============

   Arithmetic Operator

Binary operator ``+`` In addition to being a plus sign, it is also a
string concatenation. When the operand of this operator is a string,
string concatenation will be performed to concatenate two strings into a
longer string. To be precise, ``+`` as a string concatenation is no
longer in the category of arithmetic operators.

The binary operator ``%`` is the remainder symbol. Its operands must be
integers. The result of the remainder operation is the remainder after
dividing the left operand by the right operand. For example, the result
of ``11%4`` is ``3``. The real number type cannot do divisible, so the
remainder is not supported.

In general, arithmetic operators do not satisfy the commutative law. For
example, the values of the expressions ``2/4`` and ``4/2`` are not the
same.

All arithmetic operators can be overloaded in the class. The overloaded
operators are not necessarily limited to their original functional
design, but are determined by the programmer.

3.2.2 Relational operators
~~~~~~~~~~~~~~~~~~~~~~~~~~

Relational operators are used to compare the magnitude of the operands.
The six relational operators supported by Berry are given in Table 1.3.

.. container::
   :name: tab::relop_operator

   ============ ===================== ================
   **Operator** **Description**       **Example**
   ============ ===================== ================
   ``<``        Less than             ``expr < expr``
   ``<=``       Less than or equal to ``expr <= expr``
   ``==``       equal                 ``expr == expr``
   ``!=``       not equal to          ``expr != expr``
   ``>=``       greater or equal to   ``expr >= expr``
   ``>``        more than the         ``-expr``
   ============ ===================== ================

   Relational operator

By comparing the magnitude relationship of the operands or judging
whether the operands are equal, evaluating the relational expression
will produce a Boolean result. When the relationship is satisfied, the
value of the relationship expression is ``true``, otherwise it is
``false``. Relational operators ``==`` and ``!=`` can use any type of
operand, and allow the left and right operands to have different types.
Other relational operators allow the use of the following combinations
of operands:

   **integer** relop **integer**
   
   **real** relop **real**
   
   **integer** relop **real**
   
   **real** relop **integer**
   
   **string** relop **string**

In relational operations, the equal sign ``==`` and inequality sign
``!=`` satisfy the commutative law. If the left and right operands are
of the same type or are both numeric types (integer and real number),
the operands are judged to be equal according to the value of the
operands, otherwise the operands are considered unequal. Equality and
inequality are reciprocal operations: if ``a==b`` is true, then ``a!=b``
is false, and vice versa. Other relational operators do not satisfy the
commutative law, but have the following properties: ``<`` and ``>=`` are
reciprocal operations, and ``>`` and ``<=`` are reciprocal operations.
Relational operations require that the operands must be of the same
type, otherwise it is an incorrect expression.

Instances can overload operators as methods. If the relational operator
is overloaded, the program needs to ensure the above properties.

Among the relational operators, ``==`` and ``!=`` operators have more
relaxed requirements than ``<``, ``<=``, ``>`` and ``>=``, which only
allow comparisons between the same types. In actual program development,
the judgment of equality or inequality is usually simpler than the
judgment of size. Some operation objects may not be able to judge the
size but can only judge the equality or inequality. This is the case
with the Boolean type.

logical operators
~~~~~~~~~~~~~~~~~

Logical operators are divided into three types: logical AND, logical OR
and logical NOT. As shown in Table 1.4.

.. container::
   :name: tab::logic_operator

   ============ ================ ==================
   **Operator** **Description**  **Example**
   ============ ================ ==================
   ``&&``       Logical AND      ``expr && expr``
   ``\|\|``     Logical OR       ``expr \|\| expr``
   ``!``        Logical negation ``!expr``
   ============ ================ ==================

   Logical Operators

For the logical AND operator, when the values of both operands are
``true``, the value of the logical expression is ``true``, otherwise it
is ``false``.

For the logical OR operator, when the values of both operands are
``false``, the value of the logical expression is ``false``, otherwise
it is ``true``.

The role of the logical negation operator is to flip the logical state
of the operand. When the operand value is ``true``, the logical
expression value is ``false``, otherwise the value is ``true``.

Logical operators require that the operand is of Boolean type, and if
the operand is not of Boolean type, it will be converted. See section
[section::type_bool] for conversion rules.

Logic operations use an evaluation strategy called **Short-circuit
evaluation** (short-circuit evaluation). This evaluation strategy is:
for the logical AND operator, the second operand will be evaluated if
and only if the left operand is true; for the logical OR operator, if
and only if the left operand is false Will evaluate the right operand.
The nature of short-circuit evaluation causes the code in the logical
expression to not all run.

Bitwise Operator
~~~~~~~~~~~~~~~~

Bit operators can implement some binary bit operations, and bit
operations can only be used on integer types. The detailed information
of bit operators is shown in Table 1.5. Bit operation refers to the
operation of binary bits directly on integers. Logical operations can be
extended to bit operations. Taking logical AND as an example, we can
perform this operation on each binary bit to achieve bitwise AND, such
as 110\ :sub:`b`\  AND 011\ :sub:`b`\  = 010\ :sub:`b`\ . Bit operations also
support shift operations, which move numbers on a binary basis.

.. container::
   :name: tab::bitwise_operator

   ============ ==================== ================
   **Operator**                      **Example**
   ============ ==================== ================
   ``~``        Bit flip             ``~expr``
   ``&``        Bitwise and          ``expr & expr``
   ``\|``       Bitwise or           ``expr \| expr``
   ``^``        Bitwise exclusive or ``expr ^ expr``
   ``<<``       Shift left           ``expr << expr``
   ``>>``       Shift right          ``expr >> expr``
   ============ ==================== ================

   Bitwise operator

Although it can only be used for integers, bit operations are still
versatile. Bit operations can implement many optimization techniques. In
many algorithms, using bit operations can save a lot of code. For
example, to determine whether a number ``n`` is a power of 2, we can
judge whether the result of ``n & (n - 1)`` is ``0``. In some languages
with high execution efficiency, shift operations can also be used to
optimize multiplication and division (usually there is no obvious effect
in scripting languages).

The bitwise AND operator "``&``" is a binary operator, which performs
the binary AND operation of two integer operands: only when the binary
bits corresponding to the operands are all ``1``, the result It was
``1``. For example, 1110\ :sub:`b`\  & 0111\ :sub:`b`\  = 0110\ :sub:`b`\ .

The bitwise OR operator "``|``" is a binary operator, which performs a
binary-bit OR operation on two integer operands: only when the binary
bits corresponding to the operands are both ``0``, the bit of the result
It was ``0``. For example, 1000\ :sub:`b`\  \| 0001\ :sub:`b`\  = 1001\ :sub:`b`\ .

The bitwise exclusive OR operator "``^``" is a binary operator, which
performs binary exclusive OR operation on two integer operands: when the
binary bits corresponding to the operands are different, the bit value
of the result is ``1``. For example,
1100\ :sub:`b`\  \^ 0101\ :sub:`b`\  = 1001\ :sub:`b`\ .

The left shift operator "``<<``" is a binary operator, which moves the
left operand to the left by the number of bits specified by the right
operand on a binary basis. For example
00001010\ :sub:`b`\  << 3 = 01010000\ :sub:`b`\ .The right shift operator "``>>``"
is a binary operator, which shifts the left operand to the right by the
number of bits specified by the right operand on a binary basis. For
example, 10100000\ :sub:`b`\  >> 3 = 00010100\ :sub:`b`\ .

The bitwise flip operator "``~``" is a unary operator, and the result of the
expression is to flip the value of each binary bit of the operand. For
example, ``∼``\ 10100011\ :sub:`b`\  = 01011100\ :sub:`b`\ .

The following are some examples of using bit operations. Usually we
don’t use binary directly. The results in the examples have been
converted into common bases.

.. code:: berry

   1 << 1 # 2
   168 >> 4 # 10
   456 & 127 # 72
   456 | 127 # 511
   0xA5 ^ 0x5A # 255
   ~2 # -3

Assignment operator
~~~~~~~~~~~~~~~~~~~

The assignment operator only appears in the assignment expression, and
the operand of the operator must be a writable object. The assignment
expression has no result, so continuous assignment operations cannot be
used.

Simple assignment operator
^^^^^^^^^^^^^^^^^^^^^^^^^^

The simple assignment operator ``=`` can be used for variable
assignment. If the left operand variable is not defined, the variable
will be defined. The assignment operator is used to bind the value of
the right operand with the left operand. This process is also called
“assignment”. Therefore, the left operand cannot be a constant, nor can
it be any object that cannot be written. These are some legal assignment
expressions:

.. code:: berry

   a = 45 b ='string' c = 0

And the following assignment expression is wrong:

.. code:: berry

   1 = 5 # Trying to assign a constant 1
   a = b = 0 # Continuous assignment

When assigning ``nil``, integer, real and Boolean types to variables,
the value of the object will be passed to the left operand, but for
other types, the assignment operation just passes the reference of the
object to the left operand. Since strings, functions, and class types
are read-only, all passing references will not have side effects, but
you must be extra careful with instance types.

Compound Assignment Operator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Compound assignment operators are operators that combine binary
operators and assignment operators. They are practical extensions to
simple assignment operators. Compound assignment operators can simplify
the writing of some expressions. Table 1.6 lists all the compound
assignment operators

.. container::
   :name: tab::compound_assign

   ============ =========================
   **Operator** **Description**
   ============ =========================
   ``+=``       Addition assignment
   ``-=``       Subtraction assignment
   ``*=``       Multiplication assignment
   ``/=``       Preliminary assignment
   ``%=``       Remainder assignment
   ``&=``       Bitwise AND assignment
   ``\|=``      Bitwise OR assignment
   ``^=``       Bitwise XOR assignment
   ``<<=``      Left shift assignment
   ``>>=``      Right shift assignment
   ============ =========================

   Bit operator

The compound assignment expression performs the binary operation
corresponding to the compound assignment operator on the left operand
and the right operand, and then assigns the result to the left operand.
Taking ``+=`` as an example, the expression ``a += b`` is equivalent to
``a = a + b``. The compound assignment operator is also an assignment
operator, so it has a lower priority. The binary operator corresponding
to the compound assignment operator is always evaluated after the right
operand, so an expression like ``a *= 1 + 2`` should be equivalent to
``a = a * (1 + 2)``.

Unlike the simple assignment operator, the left operand of the compound
assignment operator must participate in the evaluation, so the compound
assignment expression does not have the function of defining variables.
The assignment operator itself cannot be overloaded in the class. Users
can only overload the binary operator corresponding to the compound
assignment operator. This also ensures that the compound assignment
operator will always conform to the basic characteristics of assignment
operations.

domain operator and subscript operator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Domain operator ``.`` is used to access an attribute or member of an
object. You can use domain operators for both types of modules and
instances:

.. code:: berry

   l = list[]
   l.push('item 0')
   s = l.item(0) #'item 0'

The subscript operator ``[]`` is used to access the elements of an
object, for example

.. code:: berry

   l[2] = 10 # Read by index
   n = l[2] # Write by index

Classes that support subscript reading must implement the ``item``
method, and classes that support subscript writing must implement the
``setitem`` method. The map and list in the standard container implement
these two methods, so they support reading and writing using the
subscript operator. The string supports subscript reading, but does not
support subscript writing (strings are read-only values):

.. code:: berry

   'string'[2] #'r'
   'string'[2] ='a' # error: value'string' does not support index assignment

Currently, strings support integer subscripts, and the range of
subscripts cannot exceed the length of the string.

Conditional Operator
~~~~~~~~~~~~~~~~~~~~

The conditional operator (``? :``) is similar to the **if else**
statement, but the former can be used in expressions. The usage form of
the conditional operator is:

.. code::

   cond ? expr1 : expr2

**cond** is the expression used to judge the condition. The evaluation
process of the conditional operator is: first find the value of
**cond**, if the condition is true, evaluate **expr1** and return the
value, otherwise, the value of **expr2** ] Evaluate and return the
value. **expr1** and **expr2** can have different types, so the
following is correct:

.. code:: berry

   result = scope <6?'bad': scope

This expression first determines whether ``scope`` is less than ``6``,
if it is, it returns ``bad``, otherwise it returns the value of
``scope``. Regardless of the condition of the conditional expression,
only one of **expr1** or **expr2** will be executed, similar to the
short-circuit characteristic of logical AND and logical OR operations.

Nested Condition Operators
^^^^^^^^^^^^^^^^^^^^^^^^^^

One conditional operator can be nested in another conditional operator,
that is, the conditional expression can be used as **cond** or **expr**
of another conditional expression. For example, use conditional
expressions to divide scores into three levels: excellent, good, and
bad:

.. code:: berry

   result = scope >= 9?'excellent': scope >= 6?'good':'bad'

The first condition checks whether the score is not lower than ``9``
points. If it is, execute the branch after ``?`` and return
``’excellent’``; otherwise, execute the branch after ``:``, which is
also a conditional expression. The condition checks whether the score is
not lower than ``6``, if it is, it returns ``’good’``, otherwise it
returns ``’bad’``.

The conditional operator satisfies the right associativity, so the value
of the branch expression must be evaluated first to get the value of the
conditional expression. Therefore, in a nested conditional expression,
the nested conditional expression is evaluated first, and then the outer
conditional expression is evaluated.

Priority of conditional operators
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Since the precedence of conditional expressions is very low (second only
to assignment operators), it is often necessary to add parentheses
outside the conditional expressions. For example, when a conditional
expression is used as an operand of an arithmetic expression,
parentheses will have different effects on the result:

.. code:: berry

   result = 10 * (sign <0? -1: 1) # the result is -10 or 10
   result = 10 * sign <0? -1: 1 # the result is -1 or 1

The result of the first expression is correct, and the second expression
takes ``10 * sign < 0`` as a condition to judge, which does not meet the
expectation of the conditional expression as the right operand of the
multiplication.

Concatenation Operator
~~~~~~~~~~~~~~~~~~~~~~

.. _operator-1:

``+`` operator
^^^^^^^^^^^^^^

When the left and right operands are both strings, the ``+`` operator is
used to connect the two strings, and the new string obtained is the
value of the expression. Therefore, this operator is often used for
string concatenation:

.. code:: berry

   result ='abc' + '123' # the result is'abc123'

``+`` Operators can also be used to connect two list instances:

.. code:: berry

   result = [1, 2] + [3, 4] # the result is [1, 2, 3, 4]

Unlike the ``list.push`` method, the ``+`` operator merges two lists
into a larger list object, with the elements of the left operand at the
head of the result list, and the elements of the right operand at the
end of the result list.

.. _operator-2:

``..`` operator
^^^^^^^^^^^^^^^

``..`` is a special operator. If the left operand is a string, the
behavior of the expression is to concatenate the left and right operands
into a new string (automatic conversion if the right operand is not a
string):

.. code:: berry

   result ='abc' .. 123 # the result is'abc123'

The ``..`` operator is often used when concatenating a string and a
non-string value.If the left operand is a list instance, the ``..``
operator will append the right operand to the end of the list, and then
use this list as the value of the expression:

.. code:: berry

   result = [1, 2] .. 3 # the result is [1, 2, 3]

This process will directly modify the left operand, which is very
similar to the ``push`` method of ``list`` (except for strings which are
immutable objects). The join operation of list can be executed in chain:

.. code:: berry

   result = [1, 2] .. 3 .. 4 # the result is [1, 2, 3, 4]

All values in this process will be appended to the leftmost list object.

If the left and right operands are both integers, use the ``..``
operator to get an integer range object:

.. code:: berry

   result = 1 .. 10 # the result is (1..10)

This object is used to represent a closed interval of integers, where
the left operand is the lower limit and the right operand is the upper
limit. Such integer range objects are often used for iteration.
