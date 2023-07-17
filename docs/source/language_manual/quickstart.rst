****************
Berry quickstart
****************

Online interpreter
==================

You don't need to install anything as Berry runs in the browser too!

Go to `Online editor <https://berry-lang.github.io/try-online/>`_ and see for yourself!

Docker
======

Berry has also been packed to Docker. In order to use it run:

.. code:: bash

   docker run --rm -it berry-lang/berry

Local install
=============

Preparing the environment
-------------------------

.. code:: bash

   # Ubuntu:
   apt-get install build-essential python3 libreadline-dev

   # MacOS:
   brew install readline

Building Berry
--------------

.. code:: bash

   git clone https://github.com/berry-lang/berry
   cd berry
   make

Now you should be able to find the executable file of the interpreter in
the *berry* directory (in Windows systems, the file name of the
interpreter program is “*berry.exe*”, while in Linux and macOS systems
the file name is “*berry*”), you can run the executable file directly
[3] To start the interpreter.

Installing Berry
----------------

This is a purely optional step as Berry can be run directly from the compiler output directory, but for the ease of use one can:

.. code:: bash

   make install

Please have in mind that it will only work on Linux and MacOS.

Running Berry
-------------

.. code:: bash

   ./berry # Bash or PowerShell
   berry   # Windows CMD

