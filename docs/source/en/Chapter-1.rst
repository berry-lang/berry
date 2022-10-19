Basic Information
=================

1.1 Introduction
----------------

Berry is an ultra-lightweight dynamic type embedded scripting language.
The language mainly supports procedural programming, as well as
object-oriented programming and functional programming. An important
design goal of Berry is to be able to run on embedded devices with very
small memory, so the language is very streamlined. Nevertheless, Berry
is still a feature-rich scripting language.

1.2 Start using
---------------

Get Interpreter
~~~~~~~~~~~~~~~

Readers can go to the project’s GitHub page
https://github.com/berry-lang/berry to get the source code of the Berry
interpreter. Readers need to compile the Berry interpreter by
themselves. The specific compilation method can be found in the
README.md document in the root directory of the source code, which can
also be viewed on the GitHub page of the project.

First, you must install software such as GCC, git, and make. If you do
not use version control, you can download the source code directly on
GitHub without installing git. Readers can use search engines to
retrieve information about these software. Readers using Linux and macOS
systems should also install the GNU Readline library[1]. Use ``git``
command [2] Clone the interpreter source code from the remote warehouse
to the local:

.. code:: bash

   git clone https://github.com/berry-lang/berry

Enter the berry directory and use the ``make`` command to compile the
interpreter:

.. code:: bash

   cd berry
   make

Now you should be able to find the executable file of the interpreter in
the *berry* directory (in Windows systems, the file name of the
interpreter program is “*berry.exe*”, while in Linux and macOS systems
the file name is “*berry*”), you can run the executable file directly
[3] To start the interpreter. In Linux or macOS, you can use the command
``sudo make install`` to install the interpreter, and then you can start
the interpreter with the ``berry`` command in the terminal.

REPL environment
~~~~~~~~~~~~~~~~

REPL (Read Eval Print Loop) is generally translated as an interactive
interpreter, and this article has also become the interactive mode of
the interpreter. This mode consists of four elements: **Read**, read the
source code input by the user from the input device; **evaluate**,
compile and execute the source code input by the user; **print**, output
the result of the evaluation process; **cycle**, cycle the above
operations.

Start the interpreter directly (enter ``berry`` in the terminal or
command window without parameters, or double-click berry.exe in Windows)
to enter the REPL mode, and you will see the following interface:

.. code:: berry

   Berry 1.0.0 (build in Feb 1 2022, 13:14:04)
   [GCC 8.1.0] on Windows (default)
   >

The first two lines of the interface display the version, compilation
time, compiler and operating system of the Berry interpreter. The symbol
“``>``” in the third line is called the prompt, and the cursor is
displayed behind the prompt. When using the REPL mode, after inputting
the source code, pressing the “Enter” key will immediately execute the
code and output the result. Press the ``Ctrl + C`` key combination to
exit the REPL. In the case of using the Readline library, use the
``Ctrl + D`` key combination to exit the REPL when the input is empty.
Since there is no need to edit script files, REPL mode can be used for
quick development or idea verification.

Hello World Program
^^^^^^^^^^^^^^^^^^^

Take the classic “Hello World” program as an example. Enter
``print(’Hello World’)`` in the REPL and execute it. The results are as
follows:

.. code:: berry

   Berry 1.0.0 (build in Feb 1 2022, 13:14:04)
   [GCC 8.1.0] on Windows (default)
   > print('Hello World')
   Hello World
   >

The interpreter output the text “``Hello World``”. This line of code
realizes the output of the string ``’Hello World’`` by calling the
``print`` function. In REPL, if the return value of the expression is
not ``nil``, the return value will be displayed. For example, entering
the expression ``1 + 2`` will display the calculation result ``3``:

.. code:: berry

   > 1 + 2
   3

Therefore, the simplest “Hello World” program under REPL is to directly
enter the string ``’Hello World’`` and run:

.. code:: berry

   > 'Hello World'
   Hello World

More usage of REPL
^^^^^^^^^^^^^^^^^^

You can also use the interactive mode of the Berry interpreter as a
scientific calculator. However, some mathematical functions cannot be
used directly. Instead, use the ``import math`` statement to import the
mathematical library, and then use the functions in the mathematical
library. “``math.``” as a prefix, for example ``sin`` function should be
written as ``math.sin``:

.. code:: berry

   > import math
   > math.pi
   3.14159
   > math.sin(math.pi / 2)
   1
   > math.sqrt(2)
   1.41421

Script file
~~~~~~~~~~~

Berry’s script file is a file that stores Berry code, and the script
file can be executed by an interpreter. Usually, the script file is a
text file with the extension “.be”. The command to execute the script
using the interpreter is:

.. code:: bash

   berry script_file

``script_file`` is the file name of the script file. Using this command
will run the interpreter to execute the Berry code in the
``script_file`` script file, and the interpreter will exit after
execution.

If you want the interpreter to enter the REPL mode after executing the
script file, you can add the ``-i`` parameter to the command to call the
interpreter:

.. code:: bash

   berry -i script_file

This command will first execute the code in the ``script_file`` file and
then enter the REPL mode.

1.3 Words
---------

Before introducing Berry’s syntax, let’s take a look at a simple code
(you can run this code in REPL mode):

.. code:: berry

   def func(x) # a function example
       return x + 1.5
   end
   print('func(10) =', func(10))

This code defines a function ``func`` and calls it later. Before
understanding how this code works, we first introduce the syntax
elements of the Berry language.

In the above code, the specific classification of grammatical elements
is: ``def``, ``return`` and ``end`` are keywords of Berry language; and
“``# a function example``” in the first line is called a comment;
``print`` , ``func`` and ``x`` are some identifiers, they are usually
used to represent a variable; ``1.5`` and ``10`` these numbers are
called numerical literals, they are equivalent to the numbers used in
daily life; ``’func(10) =’`` It is a string literal, they are used in
places where you need to represent text; ``+`` is an addition operator,
here the addition operator can be used to add the variable ``x`` and the
value ``1.5``.

The above classification is actually done from the perspective of a
lexical analyzer. Lexical analysis is the first step in Berry source
code analysis. In order to write the correct source code, we start with
the most basic lexical introduction.

Comment
~~~~~~~

Comments are some text that does not generate any code. They are used to
make comments in the source code and be read by people, while the
compiler will not interpret their content. Berry supports single-line
comments and cross-line block comments. Single-line comments start with
the character “``#``\ ’ until the end of the newline character. The
quick note starts with the text “\ ``#-``” and ends with the text
“``-#``”. The following is an example of using annotations:

.. code:: berry

   # This is a line comment
   #- This is a
      block comment
   -#

Similar to C language, quick comments do not support nesting. The
following code will terminate the analysis of comments at the first
“``-#``” text:

.. code:: berry

   #- Some comments -# ... -#

literal value
~~~~~~~~~~~~~

The literal value is a fixed value written directly in the source code
during programming. Berry’s literals are integers, real numbers,
booleans, strings, and nil. For example, the value ``34`` is an integer
denomination.

Numerical Literal Value
^^^^^^^^^^^^^^^^^^^^^^^

Numerical literals include **Integer** (integer) literals and **Real
number** (real) literals.

.. code:: berry

   40 # Integer literal
   0x80 # Hexadecimal literal (integer)
   3.14 # Real literal
   1.1e-6 # Real literal

Numeric literals are written similarly to everyday writing. Berry
supports hexadecimal integer denominations. Hexadecimal literals start
with the prefix ``0x`` or ``0X``, followed by a hexadecimal number.

Boolean literal value
^^^^^^^^^^^^^^^^^^^^^

Boolean values (boolean) are used to represent true and false in the
logic state. You can use the keywords ``true`` and ``false`` to
represent Boolean literals.

String literal
^^^^^^^^^^^^^^

A string is a piece of text, and its literal writing is to use a pair of
``’`` or ``"`` to surround the string text:

.. code:: berry

   'this is a string'
   "this is a string"

String literals provide some escape sequences to represent characters
that cannot be input directly. The escape sequence starts with the
character ``’\’``, and then follows a specific sequence of characters to
achieve escape. The escape sequences specified by Berry are

.. container::
   :name: tab::escape_character

   +----------------------+------------------+----------------------+------------------+
   | **Escape character** | **significance** | **Escape character** | **significance** |
   +======================+==================+======================+==================+
   | ``\a``               | Ring the bell    | ``\b``               | Backspace        |
   +----------------------+------------------+----------------------+------------------+
   | ``\f``               | Form feed.       | ``\n``               | Newline          |
   +----------------------+------------------+----------------------+------------------+
   | ``\r``               | Carriage return  | ``\t``               | Horizontal tab   |
   +----------------------+------------------+----------------------+------------------+
   | ``\v``               | Vertical tab     | ``\\``               | Backslash        |
   +----------------------+------------------+----------------------+------------------+
   | ``\’``               | apostrophe       | ``\"``               | Double quotes    |
   +----------------------+------------------+----------------------+------------------+
   | ``\?``               | question mark    | ``\0``               | Null character   |
   +----------------------+------------------+----------------------+------------------+

   .. raw:: html

      <p align="center">

   Escape character sequence

   .. raw:: html

      </p>

Escape sequences can be used in strings, for example

.. code:: berry

   print('escape character LF\n\tnew line')

The result of the operation is

::

   escape character LF
           new line

You can also use generalized escape sequences, in the form of ``\x``
followed by 2 hexadecimal digits, or ``\`` 3 octal digits, using this
escape sequence can represent any character. Here are some examples of
using the ASCII character set:

.. code:: berry

   '\115' #-'M' -#'\x34' #- '4' -#'\064' #- '4' -#

Nil literal value
^^^^^^^^^^^^^^^^^

Nil represents a null value, and its literal value is represented by the
keyword ``nil``.

identifier
~~~~~~~~~~

Identifier (identifier) is a user-defined name, which starts with an
underscore or letter, and then consists of a combination of several
underscores, letters or numbers. Similar to most languages, Berry is
case-sensitive, so identifiers ``A`` and identifiers ``a`` will be
resolved into two identifiers.

.. code:: berry

   a
   TestVariable
   Test_Var
   _init
   baseCass
   _

Keywords
~~~~~~~~

Berry reserves the following tokens as language keywords:

.. code:: berry

   if elif else while for def
   end class break continue return true
   false nil var do import as static

The specific usage of keywords will be introduced in the relevant
chapters. Note that keywords cannot be used as identifiers. Because
Berry is case sensitive, ``If`` can be used for identifiers.

[1] For GNU Readline, the installation command for the Debian series of
Linux distributions is ``sudo apt install libreadline-dev``, and the
installation command for the RedHat series of Linux distributions is
``yum install readline-devel``, under macOS The installation command is
``brew install readline``. In addition, it is easy to find GNU Readline
documentation and related materials in search engines.

[2] commands need to be used in the “command line interface” after the
preparation work is completed. The command line environment in Windows
systems is usually a command prompt (CMD) window, while the command line
environment in Unix-like systems is usually Called “Terminal”
(Terminal). This is not very accurate, but it will not be expanded here.

[3] In Windows, you can directly double-click to run the executable
file. In Linux or macOS, use the terminal to run it. You can also run
the interpreter in the Windows command prompt window. Please refer to
the README.md file for specific usage.
