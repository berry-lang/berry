.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

Información básica
==================

1.1 Introducción
----------------

Berry es un lenguaje de script integrado de tipo dinámico ultraligero.
El lenguaje admite principalmente programación procedural, así como
programación orientada a objetos y programación funcional. Un objetivo
de diseño importante de Berry es poder ejecutarse en dispositivos
integrados con muy poca memoria, por lo que el lenguaje está muy
optimizado. Sin embargo, Berry sigue siendo un lenguaje de secuencias de
comandos rico en funciones.

1.2 Empezar a usar
------------------

Obtener el intérprete
~~~~~~~~~~~~~~~~~~~~~

Los lectores pueden ir a la página de GitHub del proyecto:
https://github.com/berry-lang/berry para obtener el código fuente del
intérprete de Berry. Los lectores necesitan compilar el intérprete de
Berry por sí mismos. El método de compilación específico se puede
encontrar en el documento README.md en el directorio raíz del código
fuente, que también se puede ver en la página de GitHub del proyecto.

Primero, debe instalar un software como GCC, git y make. Si no usa el
control de versiones, puede descargar el código fuente directamente en
GitHub sin instalar git. Los lectores pueden usar motores de búsqueda
para recuperar información sobre este software. Los lectores que
utilizan sistemas Linux y macOS también deben instalar la biblioteca GNU
Readline[1]. Use el comando ``git`` [2] Clone el código fuente del
intérprete del almacén remoto al local:

.. code:: bash

   git clone https://github.com/berry-lang/berry

Ingrese al directorio berry y use el comando ``make`` para compilar el
intérprete:

.. code:: bash

   cd berry
   make

Ahora debería poder encontrar el archivo ejecutable del intérprete en el
directorio *berry* (en los sistemas Windows, el nombre del archivo del
programa intérprete es “*berry.exe*”, mientras que en los sistemas Linux
y macOS el nombre del archivo es “*berry*”), puede ejecutar el archivo
ejecutable directamente. Para iniciar el intérprete: En Linux o macOS,
puede usar el comando ``sudo make install`` para instalar el intérprete,
y luego puede iniciar el intérprete con el comando ``berry`` en la
terminal.

Entorno REPL
~~~~~~~~~~~~

REPL (Leer Evaluar Imprimir Bucle) generalmente se traduce como un
intérprete interactivo, y este elemnto también se ha convertido en el
modo interactivo del intérprete. Este modo consta de cuatro elementos:
**Leer**, lee el código fuente ingresado por el usuario desde el
dispositivo de entrada; **Evaluar**, compila y ejecuta el código fuente
ingresado por el usuario; **Imprimir**, muestra el resultado del proceso
de evaluación; **Bucle**, realiza un ciclo de las operaciones
anteriores.

Inicie el intérprete directamente (ingrese ``berry`` en la terminal o en
la ventana de comandos sin parámetros, o haga doble clic en berry.exe en
Windows) para ingresar al modo REPL y verá la siguiente interfaz:

.. code:: berry

   Berry 1.0.0 (build in Feb 1 2022, 13:14:04)
   [GCC 8.1.0] on Windows (default)
   >

Las dos primeras líneas de la interfaz muestran la versión, el tiempo de
compilación, el compilador y el sistema operativo del intérprete de
Berry. El símbolo “``>``” en la tercera línea se llama indicador y el
cursor se muestra detrás del indicador. Al usar el modo REPL, después de
ingresar el código fuente, al presionar la tecla “Enter” se ejecutará
inmediatamente el código y se generará el resultado. Presione la
combinación de teclas ``Ctrl + C`` para salir de REPL. En el caso de
usar la biblioteca Readline, use la combinación de teclas ``Ctrl + D``
para salir de REPL cuando la entrada esté vacía. Dado que no hay
necesidad de editar archivos de secuencias de comandos, el modo REPL se
puede utilizar para un desarrollo rápido o verificación de ideas.

Programa Hola Mundo
^^^^^^^^^^^^^^^^^^^

Tomemos como ejemplo el clásico programa “Hola Mundo”. Introduzca
``print('Hola Mundo')`` en el REPL y ejecútelo. Los resultados son los
siguientes:

.. code:: berry

   Berry 1.0.0 (build in Feb 1 2022, 13:14:04)
   [GCC 8.1.0] on Windows (default)
   > print('Hola Mundo')
   Hola Mundo
   >

El intérprete emite el texto “``Hola Mundo``”. Esta línea de código
realiza la salida de la cadena ``'Hola Mundo'`` llamando a la función
``print``. En REPL, si el valor de retorno de la expresión no es
``nil``, se mostrará el valor de retorno. Por ejemplo, ingresar la
expresión ``1 + 2`` mostrará el resultado del cálculo ``3``:

.. code:: berry

   > 1 + 2
   3

Por lo tanto, el programa “Hola Mundo” más simple bajo REPL es ingresar
directamente la cadena ``'Hola Mundo'`` y ejecutar:

.. code:: berry

   > 'Hola Mundo'
   Hola Mundo

Más uso de REPL
^^^^^^^^^^^^^^^

También puede utilizar el modo interactivo del intérprete de Berry como
calculadora científica. Sin embargo, algunas funciones matemáticas no se
pueden utilizar directamente. En su lugar, use la declaración
``import math`` para importar la biblioteca matemática y luego use las
funciones en la biblioteca. Utilice “``math.``” como prefijo, por
ejemplo, la función ``sin`` debe escribirse como ``math.sin``:

.. code:: berry

   > import math
   > math.pi
   3.14159
   > math.sin(math.pi / 2)
   1
   > math.sqrt(2)
   1.41421

Archivo de comandos
~~~~~~~~~~~~~~~~~~~

El archivo de secuencia de comandos de Berry es un archivo que almacena
el código de Berry, y un intérprete puede ejecutar el archivo de script.
Normalmente, el archivo de script es un archivo de texto con la
extensión “.be”. El comando para ejecutar el script usando el intérprete
es:

.. code:: bash

   berry script_file

``script_file`` es el nombre de archivo del archivo de script. El uso de
este comando ejecutará el intérprete para ejecutar el código Berry en el
archivo de script ``script_file`` y el intérprete se cerrará después de
la ejecución.

Si desea que el intérprete ingrese al modo REPL después de ejecutar el
archivo de script, puede agregar el parámetro ``-i`` al comando para
llamar al intérprete:

.. code:: bash

   berry script_file

Este comando primero ejecutará el código en el archivo ``script_file`` y
luego ingresará al modo REPL.

1.3 Palabras (Words)
--------------------

Antes de presentar la sintaxis de Berry, echemos un vistazo a un código
simple (puede ejecutar este código en modo REPL):

.. code:: berry

   def func(x) # una función ejemplo
       return x + 1.5
   end
   print('func(10) =', func(10))

Este código define una función ``func`` y la llama más tarde. Antes de
entender cómo funciona este código, primero presentaremos los elementos
de sintaxis del lenguaje Berry.

En el código anterior, la clasificación específica de los elementos
gramaticales es: ``def``, ``return`` y ``end``. Estas son palabras clave
del lenguaje Berry; y “``# una función ejemplo``” en la primera línea se
llama comentario; ``print``, ``func`` y ``x`` son algunos
identificadores, generalmente se usan para representar una variable;
``1.5`` y ``10`` estos números se llaman literales numéricos, son
equivalentes a los números usados en la vida diaria; ``'func(10) ='`` Es
un literal de cadena, se usan en lugares donde necesitas representar
texto; ``+`` es un operador de suma, aquí el operador de suma se puede
usar para sumar la variable ``x`` y el valor ``1.5``.

La clasificación anterior se realiza en realidad desde la perspectiva de
un analizador lexicográfico. El análisis lexicográfico es el primer paso
en el análisis del código fuente de Berry. Para escribir el código
fuente correcto, comenzamos con la introducción lexicográfica más
básica.

Comentario
~~~~~~~~~~

Los comentarios son textos que no genera ningún código. Se utilizan para
hacer comentarios en el código fuente y ser leídos por personas,
mientras que el compilador no interpretará su contenido. Berry admite
comentarios de una sola línea y comentarios de bloque de líneas
cruzadas. Los comentarios de una sola línea comienzan con el carácter
“``#``\ ’ hasta el final del carácter de nueva línea. La nota rápida
comienza con el texto “\ ``#-``” y termina con el texto “``-#``”. El
siguiente es un ejemplo del uso de anotaciones:

.. code:: berry

   #  Este es un comentario de línea
   #- Este es un
      bloque de comentario
   -#

Similar al lenguaje C, los comentarios rápidos no admiten el
anidamiento. El siguiente código terminará el análisis de los
comentarios en el primer texto “``-#``”:

.. code:: berry

   #- - Algunos comentarios -# ... -#

Valor literal
~~~~~~~~~~~~~

El valor literal es un valor fijo escrito directamente en el código
fuente durante la programación. Los literales de Berry son números
enteros, números reales, booleanos, cadenas y nil. Por ejemplo, el valor
``34`` es una denominación entera.

Valor literal numérico
^^^^^^^^^^^^^^^^^^^^^^

Los literales numéricos incluyen literales **Integer** (entero) y
literales **Número real** (real).

.. code:: berry

   40 #  Literal entero
   0x80 # Literal hexadecimal (entero)
   3.14 #  Literal real
   1.1e-6 #  Literal real

Los literales numéricos se escriben de manera similar a la escritura
cotidiana. Berry admite denominaciones enteras hexadecimales. Los
literales hexadecimales comienzan con el prefijo ``0x`` o ``0X``,
seguido de un número hexadecimal.

Valor literal booleano
^^^^^^^^^^^^^^^^^^^^^^

Los valores booleanos (booleanos) se utilizan para representar verdadero
y falso en el estado lógico. Puede utilizar las palabras clave ``true``
y ``false`` para representar literales booleanos.

Literal de cadena
^^^^^^^^^^^^^^^^^

Una cadena es un fragmento de texto, y su escritura literal consiste en
usar un par de ``'`` o ``"`` para rodear el texto de la cadena:

.. code:: berry

   'esto es una cadena'
   "esto es una cadena"

Los literales de cadena proporcionan algunas secuencias de escape para
representar caracteres que no se pueden ingresar directamente. La
secuencia de escape comienza con el carácter ``'\'`` y luego sigue una
secuencia específica de caracteres para lograr el escape. Las secuencias
de escape especificadas por Berry son

.. container::
   :name: tab::escape_character

   +------------------------+-----------------+------------------------+-----------------+
   | **Caracter de Escape** | **significado** | **Caracter de Escape** | **significado** |
   +========================+=================+========================+=================+
   | ``\a``                 | Suena la        | ``\b``                 | Retroceso       |
   |                        | campana         |                        |                 |
   +------------------------+-----------------+------------------------+-----------------+
   | ``\f``                 | Alimentación    | ``\n``                 | Nueva línea     |
   |                        | de formulario.  |                        |                 |
   +------------------------+-----------------+------------------------+-----------------+
   | ``\r``                 | Retorno de      | ``\t``                 | Tabulación      |
   |                        | carro           |                        | Horizontal      |
   +------------------------+-----------------+------------------------+-----------------+
   | ``\v``                 | Tabulación      | ``\\``                 | Barra           |
   |                        | Vertical        |                        | invertida       |
   +------------------------+-----------------+------------------------+-----------------+
   | ``\’``                 | Apóstrofe       | ``\"``                 | Comillas        |
   |                        |                 |                        | dobles          |
   +------------------------+-----------------+------------------------+-----------------+
   | ``\?``                 | Signo de        | ``\0``                 | Caracter Null   |
   |                        | interrogación   |                        |                 |
   +------------------------+-----------------+------------------------+-----------------+

   *Tabla 1: Secuencia de Caracter de Escape*

Las secuencias de escape se pueden usar en cadenas, por ejemplo

.. code:: berry

   print('caracter de escape LF\n\tnueva línea')

El resultado de la operación es:

::

   caracter de escape LF
           nueva línea

También puede usar secuencias de escape generalizadas, en forma de
``\x`` seguido de 2 dígitos hexadecimales, o ``\`` 3 dígitos octales,
usando esta secuencia de escape puede representar cualquier carácter.
Estos son algunos ejemplos del uso del conjunto de caracteres ASCII:

.. code:: berry

   '\115' #-'M' -#'\x34' #- '4' -#'\064' #- '4' -#

Valor literal nulo
^^^^^^^^^^^^^^^^^^

Nil representa un valor nulo, y su valor literal está representado por
la palabra clave ``nil``.

Identificador
~~~~~~~~~~~~~

El identificador es un nombre definido por el usuario, que comienza con
un guión bajo o una letra, y luego consiste en una combinación de varios
guiones bajos, letras o números. Al igual que la mayoría de los
lenguajes, Berry distingue entre mayúsculas y minúsculas, por lo que los
identificadores ``A`` y los identificadores ``a`` se resolverán en dos
identificadores diferentes.

.. code:: berry

   a
   TestVariable
   Test_Var
   _init
   baseCass
   _

Palabras clave
~~~~~~~~~~~~~~

Berry reserva los siguientes tokens como palabras clave del lenguaje:

.. code:: berry

   if elif else while for def
   end class break continue return true
   false nil var do import as static

El uso específico de palabras clave se presentará en los capítulos
correspondientes. Tenga en cuenta que las palabras clave no se pueden
utilizar como identificadores. Debido a que Berry distingue entre
mayúsculas y minúsculas, ``If`` puede usarse para identificadores.

[1] Para GNU Readline, el comando de instalación para la serie Debian de
distribuciones de Linux es ``sudo apt install libreadline-dev``, y el
comando de instalación para la serie RedHat de distribuciones de Linux
es ``yum install readline-devel``, bajo macOS el comando de instalación
es ``brew install readline``. Además, es fácil encontrar documentación
de GNU Readline y materiales relacionados en los motores de búsqueda.

[2] Los comandos deben usarse en la “interfaz de línea de comandos”
después de completar el trabajo de preparación. El entorno de la línea
de comandos en los sistemas Windows suele ser una ventana del símbolo
del sistema (CMD), mientras que el entorno de la línea de comandos en
los sistemas similares a Unix suele llamarse “Terminal”. Esto no es muy
preciso, pero no se ampliará aquí.

[3] En Windows, puede hacer doble clic directamente para ejecutar el
archivo ejecutable. En Linux o macOS, use la terminal para ejecutarlo.
También puede ejecutar el intérprete en la ventana del símbolo del
sistema de Windows. Consulte el archivo README.md para un uso
específico.
