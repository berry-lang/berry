.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

Intérprete del compilador
=========================

1. Visión general
-----------------

El código fuente del intérprete de Berry está escrito con el estándar
ISO C99 y el código central no depende de bibliotecas de terceros, por
lo que tiene una gran versatilidad. Tome el sistema Ubuntu como ejemplo,
ejecute el siguiente comando en la terminal para instalar el intérprete
de Berry:

.. code:: sh

   apt install git gcc g++ make libreadline-dev
   git clone https://github.com/berry-lang/berry
   cd berry
   make
   make install

El Makefile proporcionado en el repositorio de GitHub se crea con el
compilador GCC. Otros compiladores también pueden compilar correctamente
el intérprete de Berry. Los compiladores actualmente probados y
disponibles incluyen GCC, Clang, MSVC, ARMCC e ICCARM. El compilador que
compila el intérprete de Berry debe tener las siguientes
características:

-  Compilador C que soporta el estándar C99

-  Compilador C++ compatible con el estándar C++11 (solo para
   compilación nativa)

-  Plataforma de destino de 32 o 64 bits

El compilador de C++ solo se usa para compilar las herramientas
*map_build*, por lo que no es necesario proporcionar un compilador
cruzado de C++ para el intérprete de Berry al realizar la compilación
cruzada, pero el usuario debe preparar un compilador nativo de C++ (a
menos que el usuario pueda obtener el \* archivo ejecutable de la
herramienta map_build*).

2. portabilidad
---------------

Lo siguiente es cómo portar el intérprete de Berry al proyecto del
usuario:

1. Agregue todos los archivos fuente en el directorio *src* al proyecto
   de usuario y el directorio debe agregarse a la ruta de inclusión

2. Los usuarios deben implementar por sí mismos archivos
   *predeterminados* que no sean *berry.c* en el directorio. Si las
   condiciones lo permiten, no necesitan modificarlos

3. Utilice la herramienta *map_build* para generar código de objeto
   constante y luego compilar

3. Soporte de plataforma
------------------------

Actualmente, el intérprete de Berry ha sido probado en algunas
plataformas. Los sistemas operativos Windows, Linux y MacOS que se
ejecutan en CPU X86 pueden funcionar normalmente. Las plataformas
integradas que se han probado incluyen Cortex M3/M0/M4/M7. El intérprete
de Berry debería poder funcionar bien sobre la base de la biblioteca de
tiempo de ejecución C necesaria. En la actualidad, cuando solo se
compila el núcleo del lenguaje Berry, el código del intérprete generado
por el compilador ARMCC tiene solo alrededor de 40 KiB, y el intérprete
puede ejecutarse en un dispositivo con solo 8 KiB de RAM.
