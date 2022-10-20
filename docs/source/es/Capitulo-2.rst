.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

2. Tipos y Variables
====================

**Tipo** es un atributo de datos, que define el significado de los datos
y las operaciones que se pueden realizar en los datos. Los tipos se
pueden dividir en tipos integrados y tipos definidos por el usuario. Los
tipos integrados se refieren a algunos tipos básicos integrados en el
lenguaje Berry, entre los cuales los tipos que no se basan en
definiciones de clase se denominan **Tipo simple**. Los tipos basados en
definiciones de clase se denominan **Tipo de clase**, algunos de los
tipos integrados son tipos de clase y los tipos definidos por el usuario
también son tipos de clase.

2.1 Tipo incorporado
--------------------

2.1.1 Tipo simple
~~~~~~~~~~~~~~~~~

``nil``
^^^^^^^

El tipo Nil es el tipo nulo, lo que significa que el objeto tiene un
valor no válido, o se puede decir que el objeto no tiene un valor
significativo. Este es un tipo muy especial. Aunque podríamos decir que
una variable es ``nil``, de hecho el tipo nil no tiene valor, entonces
de lo que estamos hablando aquí es que el tipo de la variable es nil (no
un valor).

El valor predeterminado de una variable antes de la asignación es
``nil``. Este tipo se puede utilizar en operaciones lógicas. En este
caso ‘nil’ es equivalente a ‘falso’.

Tipo entero
^^^^^^^^^^^

El tipo entero (integer) representa un entero con signo, denominado
entero. El número de bits del entero representado por este tipo depende
de la implementación específica y, por lo general, consta de un entero
de 32 bits con signo en una plataforma de 32 bits. Integer es un tipo
aritmético y admite todas las operaciones aritméticas. Preste atención
al rango de valores de los enteros cuando use este tipo. El rango de
valores típico de los enteros con signo de 32 bits está entre
−2147483648 y 2147483647.

Cualquier valor se puede convertir a ``int`` usando la función
``int()``; sin embargo ``int(nil) == nil``. Si el argumento es una
instancia, y si contiene un miembro ``toint()``, se nos llamará y el
valor devuelto se convertirá en ``int``.

Tipo de número real
^^^^^^^^^^^^^^^^^^^

El tipo real (real), para ser precisos, es un tipo de coma flotante. Los
tipos de números reales generalmente se implementan como números de
punto flotante de precisión simple o números de punto flotante de
precisión doble. El tipo de número real también es un tipo aritmético.
En comparación con el tipo de número entero, el tipo de número real
tiene mayor precisión y un mayor rango de valores, por lo que este tipo
es más adecuado para cálculos matemáticos. Cabe señalar que el tipo de
número real es en realidad un número de coma flotante, por lo que aun
existen problemas de precisión. Por ejemplo, no se recomienda comparar
dos valores de tipo ``real`` para la igualdad.

Cuando los enteros y los números reales participan en operaciones al
mismo tiempo, los enteros generalmente se convierten en números reales.

Tipo booleano
^^^^^^^^^^^^^

El tipo booleano (boolean) se utiliza para operaciones lógicas. Tiene
dos valores ``true`` y ``false``, que representan los dos valores
verdaderos (verdadero y falso) en lógica y álgebra booleana. El tipo
booleano se utiliza principalmente para el juicio condicional. Los
operandos y los valores devueltos de las expresiones lógicas y las
expresiones relacionales son todos de tipo booleano, y las sentencias
como ``if`` y ``while`` utilizan tipos booleanos como comprobaciones
condicionales.

En muchos casos, los valores no booleanos también se pueden usar como
tipos booleanos. Esto se debe a que el intérprete convertirá
implícitamente los parámetros. Esta es también la razón por la que las
expresiones de verificación condicional, como las declaraciones ``if``,
pueden usar cualquier tipo de parámetros. Las reglas para convertir
varios tipos a tipos booleanos son:

-  ``nil``: convertido a ``falso``.

-  **Entero**: cuando el valor es ``0``, se convierte en ``falso``, de
   lo contrario, se convierte en ``verdadero``.

-  **Número real**: cuando el valor es ``0.0``, se convierte en
   ``falso``, de lo contrario, se convierte en ``verdadero``.

-  **Cadena**: cuando el valor es "" (cadena vacía) se convierte en
   ``falso`` de lo contrario, se convierte en ``verdadero``.

-  **Comobj** y **Comptr**: cuando el puntero interno es ``NULL`` es
   convertido a ``falso``, de lo contrario se convierte a ``verdadero``.

-  **Instancia**: si la instancia contiene un método ``tobool()``, se
   utilizará el valor de retorno del método, de lo contrario, se
   convertirá en ``verdadero``.

-  Todos los demás tipos: converten a ``verdadero``.

Cualquier valor se puede convertir a ``bool`` usando la función
``bool()``.

Cadena
^^^^^^

Una cadena es una secuencia de caracteres. En términos de
almacenamiento, Berry divide las cadenas en cadenas largas y cadenas
cortas. Solo hay una instancia de la misma cadena de caracteres cortos
en la memoria, y todas las cadenas de caracteres cortos están vinculadas
en una tabla hash. Este diseño ayuda a mejorar el rendimiento de la
comparación de igualdad de cadenas y puede reducir el uso de memoria.
Dado que la frecuencia de uso de cadenas largas es baja y la sobrecarga
de la operación hash es bastante alta, no están vinculadas a la tabla
hash, por lo que puede haber varias instancias idénticas en la memoria.
La cadena es de solo lectura después de que se crea. Por lo tanto,
“modificar” la cadena generará una nueva cadena y la cadena original no
se modificará.

Berry no se preocupa por el formato o la codificación de los caracteres.
Por ejemplo, la cadena ``'abc'`` es en realidad el código ASCII de los
caracteres ``'a'``, ``'b'`` y ``'c'``. Por lo tanto, si hay caracteres
anchos en la cadena (la longitud de los caracteres es superior a 1
byte), el número de caracteres de la cadena no se puede contar
directamente. De hecho, usar la función ``size()`` solo puede obtener el
número de bytes en la cadena. Además, para facilitar la interacción con
el lenguaje C, la cadena de Berry siempre termina con los caracteres
``'\0'``. Esta función es transparente para el programa Berry.

El tipo de cadena se puede comparar en tamaño, por lo que se puede usar
en operaciones relacionales.

Función
^^^^^^^

Una función es una pieza de código que está encapsulada y disponible
para llamadas, generalmente utilizada para implementar una función
específica. La función es en realidad una categoría grande, que incluye
varios subtipos, como cierres, funciones nativas y cierres nativos. Para
el código Berry, todos los subtipos de funciones tienen el mismo
comportamiento. Las funciones pertenecen al primer tipo de valor en
Berry, por lo que se pueden pasar como valores. Además, se puede usar
directamente en expresiones a través de la forma “literal” de “funciones
anónimas”.

Una función es un objeto de solo lectura y no se puede modificar una vez
definida. Puede comparar si dos funciones son iguales (si son la misma
función), pero no se puede comparar el tipo de función. **Función
nativa** y **Cierre nativo** se refieren a funciones y cierres
implementados en lenguaje C. Uno de los propósitos principales de las
funciones nativas y los cierres nativos es proporcionar funciones que el
lenguaje Berry no proporciona, como operaciones de E/S y operaciones de
bajo nivel. Si un fragmento de código se usa con frecuencia y tiene
requisitos de rendimiento, se recomienda reescribirlo como una función
nativa o un cierre nativo.

Clase
^^^^^

En la programación orientada a objetos, una clase es una plantilla de
código de programa extensible. Las clases se utilizan para crear objetos
de instancia, por lo que se puede decir que la clase es el “tipo” de la
instancia. Todos los objetos de instancia son del tipo ``instancia`` y
todos tienen una clase correspondiente, que se llama instancia **Tipo de
clase**. En pocas palabras, una clase es un valor que representa el tipo
de un objeto de instancia y una clase es una abstracción de las
características de una instancia. Una clase también es un objeto de solo
lectura, una vez definida, no se puede modificar.

Las clases solo pueden comparar iguales y desiguales, pero no pueden
comparar tamaños.

Ejemplos
^^^^^^^^

Una instancia es un objeto materializado generado por una clase, y el
proceso de generar una instancia a partir de una clase se llama
``Instanciación``. En la programación orientada a objetos, “instancia”
suele ser sinónimo de “objeto”. Sin embargo, para distinguir los objetos
que no son de instancia, no usamos el término “objeto” solo, sino que
usamos “instancia” u “objeto de instancia”. Las instancias de Berry
siempre se asignan dinámicamente y deben usarse con un recolector de
basura. Además de la asignación de memoria, el proceso de creación de
instancias también necesita inicializar la instancia, este proceso lo
completa el ``Constructor``. Además, puedes completar la destrucción del
objeto a través del ``Destructor`` antes de recuperar la memoria del
objeto.

En la implementación interna, la instancia contendrá una referencia a la
clase, y la instancia en sí misma solo almacena variables miembro y no
métodos.

2.1.2 Tipo de clase
~~~~~~~~~~~~~~~~~~~

Algunos de los tipos incorporados son tipos de clase, son ``list``,
``map`` y ``range``. A diferencia de los tipos personalizados, los tipos
de clase integrados se pueden construir usando literales, por ejemplo,
``[1, 2, 3]`` es un literal de tipo ``list``.

Lista
^^^^^

La clase List es un contenedor que proporciona soporte para tipos de
datos de lista. La lista de Berry es una colección ordenada de
elementos, y cada elemento de la lista tiene un índice entero único, y
se puede acceder a cada elemento directamente según el índice. List
admite la inserción o eliminación de elementos en cualquier posición, y
el elemento puede ser de cualquier tipo. Además de usar índices, también
puede usar iteradores para acceder a los elementos de la lista.

La implementación de List es una matriz dinámica y esta estructura de
datos tiene un buen rendimiento de acceso aleatorio. La eficiencia de
agregar y eliminar elementos al final de la lista es muy alta, pero la
eficiencia de agregar y eliminar elementos en el medio de la lista es
baja.

El método de inicialización literal del contenedor List es usar una
lista de objetos entre corchetes y múltiples objetos separados por
comas, por ejemplo:

.. code:: berry

   []
   ['string']
   [0, 1, 2,'list']

Operaciones: ver capítulo 7.

Mapa
^^^^

El mapa también es un tipo de contenedor, el mapa es una colección de
pares clave-valor, y cada clave posible aparece como máximo una vez en
la colección. El contenedor Mapa proporciona las siguientes operaciones
básicas:

-  Agregar pares clave-valor a la colección

-  Eliminar pares clave-valor de la colección

-  Modificar el valor correspondiente a una clave existente

-  Encuentra el valor correspondiente por clave

El mapa se implementa utilizando una tabla hash y tiene una alta
eficiencia de búsqueda. La operación de agregar y eliminar pares
clave-valor consumirá más tiempo si se produce un “rehashing”.

El contenedor Map también se puede inicializar con valores literales,
escritos entre llaves para encerrar una lista de pares clave-valor,
separados por dos puntos entre claves y valores, y separados por comas
entre pares clave-valor. P.ej:

.. code:: berry

   {}
   {'str':'hola'}
   {'str':'hola','int': 45, 78: nil}

Operaciones: ver capítulo 7.

Rango
^^^^^

El contenedor Range representa un rango de enteros, que generalmente se
usa para iterar en un rango de enteros. Este tipo tiene un miembro
``__lower__`` y un miembro ``__upper__``, que representan los límites
inferior y superior del rango, respectivamente. El valor literal de
Range es un par de enteros conectados mediante el operador ``..``:

.. code:: berry

   0 .. 10
   -5 .. 5

Cuando la clase Range se usa para la iteración, los elementos de la
iteración son todos valores enteros desde el límite inferior hasta el
límite superior, incluidos los valores límite. Por ejemplo, el resultado
de la iteración de ``0..5`` es:

.. code:: berry

   0 1 2 3 4 5

Por tanto, cabe señalar que para un rango de *x* .. (*x*\ +\ *n*), el
número de iteraciones es *n* + 1. Una construcción común para iterar a
través de los elementos de una lista por elemento es:

.. code:: berry

   for i: 0..size(l)-1

Rango abierto: si omite el último rango, se reemplaza implícitamente con
MAXINT.

.. code:: berry

   > r = 10..
   > r
   (10..9223372036854775807)

Bytes
^^^^^

El objeto Bytes denota un búfer de bytes que se puede usar para
manipular búferes de bytes o para leer/escribir algunas áreas o
estructuras de memoria C.

Consulte el Capítulo 7.

2.2 Variables
-------------

Una variable es un espacio de almacenamiento con un nombre, y los datos
o la información almacenados en el espacio de almacenamiento se
denominan valor de la variable. Los nombres de variables se utilizan
para hacer referencia a las variables en el código fuente. En diferentes
ámbitos, un nombre de variable puede vincular varias variables
independientes, pero las variables no tienen alias. El valor de la
variable se puede acceder o cambiar en cualquier momento durante la
ejecución del programa. Berry es un lenguaje de tipo dinámico, por lo
que el tipo de valor de la variable se determina en tiempo de ejecución
y la variable puede almacenar cualquier tipo de valor.

2.2.1 Definir variables
~~~~~~~~~~~~~~~~~~~~~~~

La primera forma de definir una variable es usar una declaración de
asignación para asignar un valor a un nuevo nombre de variable:

.. raw:: html

   <div class="algorithm">

.. code::

   ’var’ =  expresión

**var** es el nombre de la variable, y el nombre de la variable es un
identificador (consulte la sección identificador). **expresión** es la
expresión para inicializar la variable.

.. code:: berry

   a = 1
   b ='str'

Sin embargo, este método de definición de variables tiene algunas
limitaciones. Tome el siguiente código como ejemplo:

.. code:: berry

   i = 0
   do
       i = 1
       print(i) # 1
   end
   print(i) # 1

La instrucción ``do`` en la rutina constituye el alcance interno.
Modificamos el valor de la variable ``i`` en la línea 3, y el valor de
``i`` sigue siendo ``1`` después de dejar el alcance interno en la línea
6. Si queremos que la variable ``i`` del ámbito interno sea una variable
independiente, el método de definir la variable mediante la asignación
directa al nuevo nombre de variable no puede cumplir el requisito,
porque el identificador ``i`` ya existe en el ámbito externo. En este
caso, la variable se puede definir mediante la palabra clave ``var``:

.. code::

   ’var’ variable
   ’var’ variable = expresión

Hay dos formas de usar ``var`` para definir una variable: la primera es
seguir el nombre de la variable **variable** después de la palabra clave
``var``, en este caso la variable se inicializará a ``nil``, y la otra
se escribe en la variable y se inicializa al mismo tiempo que se define
la variable. En este caso, se requiere una expresión de valor inicial
**expresión**. Usar ``var`` para definir una variable tiene dos posibles
resultados: si el alcance actual no define la variable de **variable**,
definir e inicializar la variable, de lo contrario, es equivalente a
reinicializar la variable. Por lo tanto, la variable definida con
``var`` protegerá a la variable con el mismo nombre en el ámbito
externo.

Ahora cambiamos el ejemplo anterior para usar la palabra clave ``var``
para definir variables:

.. code:: berry

   i = 0
   do
       var i = 1
       print(i) # 1
   end
   print(i) # 0

A partir de la rutina modificada, se puede encontrar que el valor de la
variable ``i`` en el ámbito interno es ``1``, y su valor en el ámbito
externo es ``0``. Esto prueba que después de usar la palabra clave
``var``, se define una nueva variable ``i`` en el ámbito interno y se
bloquea la variable con el mismo nombre en el ámbito externo. Una vez
que finaliza el ámbito interno, el identificador ``i`` vuelve a
vincularse a la variable ``i`` en el ámbito externo.

Al usar la palabra clave ``var`` para definir una variable, también
puede usar una lista de múltiples nombres de variables, separados por
comas. También puede inicializar una o más variables al definir
variables:

.. code:: berry

   var a = 0, b, c ='test'

2.2.2 Alcance y Ciclo de Vida
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Como se mencionó anteriormente, los nombres de las variables se pueden
vincular a varias entidades de variables (espacios de almacenamiento) y
los nombres de las variables se vinculan a una sola entidad en cada
posición. La entidad vinculada por el nombre de la variable debe
determinarse de acuerdo con la posición en la que aparece el nombre de
la variable.

**Ámbito** se refiere al área de código donde el nombre y la entidad
están vinculados de forma única. Fuera del alcance, el nombre puede
estar vinculado a otras entidades, o no estar vinculado a ninguna
entidad. La entidad solo es visible en el alcance asociado al nombre, es
decir, la variable solo es válida en su alcance. Un bloque de código
(ver bloque) es un alcance. Una variable solo está disponible dentro del
bloque, y los nombres en diferentes bloques pueden vincular diferentes
entidades variables. El siguiente ejemplo demuestra el alcance de las
variables:

.. code:: berry

   var i = 0
   do
       var j ='str'
       print(i, j) # 0 str
   end
   #  La variable j no está disponible aquí
   print(i) # 0

Los nombres ``i`` y ``j`` se definen en esta rutina. El nombre ``i`` se
define fuera de la oración ``do``, y el nombre definido en el bloque más
externo tiene **Alcance global**. El nombre con alcance global está
disponible en todo el programa después de la personalización. El nombre
``j`` se define en el bloque en la oración ``do``, y el nombre de este
tipo de definición en el bloque no exterior tiene **Ámbito local**. No
se puede acceder a un nombre con un ámbito local fuera del ámbito.

Berry tiene algunos objetos integrados, que están todos en el ámbito
global. Sin embargo, los objetos integrados y las variables globales
definidas en los scripts no están en el mismo ámbito global. Los objetos
integrados en realidad pertenecen al **Alcance integrado**. El alcance
es visible globalmente como el alcance global ordinario, pero puede
estar cubierto por el alcance global ordinario. Los objetos incorporados
incluyen funciones y clases en la biblioteca estándar. Estos objetos
incluyen funciones de ``impresión``, funciones de ``tipo`` y clases de
``mapa``. A diferencia de otros ámbitos, las variables en el ámbito
integrado son de solo lectura, por lo que la “asignación” a las
variables en el ámbito integrado en realidad define una variable con el
mismo nombre en el ámbito global, que anula los símbolos en el ámbito en
el alcance incorporado.

Alcance anidado
^^^^^^^^^^^^^^^

Ámbito anidado significa que el ámbito contiene otro ámbito. Llamamos al
ámbito contenido **Ámbito interno** y al ámbito que contiene el ámbito
interno **Ámbito externo**. Se puede acceder al nombre definido en el
ámbito externo en todos los ámbitos internos. El ámbito interno también
puede volver a vincular el nombre ya definido en el ámbito externo. El
ejemplo anterior usando ``var`` para definir variables describe este
escenario.

Ciclo de vida variable
^^^^^^^^^^^^^^^^^^^^^^

No existe el concepto de nombres de variables cuando el programa se está
ejecutando, y las variables existen en forma de entidades en este
momento. El “período de validez” de una variable durante la ejecución
del programa es el **ciclo de vida** de la variable. Las variables en
tiempo de ejecución solo son válidas dentro del alcance. Después de
salir del alcance, las variables se destruirán para recuperar recursos.

Las variables definidas en el ámbito global se denominan **Variable
global** y tienen **Ciclo de vida estático**. Se puede acceder a dichas
variables durante todo el programa en ejecución y no se destruirán. Las
variables definidas en el ámbito local se denominan **Variable local** y
tienen **Ciclo de vida dinámico**. No se puede acceder a dichas
variables después de abandonar el alcance y se destruirán.

Debido a los diferentes ciclos de vida, las variables locales y las
variables globales usan diferentes formas de asignar el espacio de
almacenamiento. Las variables locales se asignan en una estructura
llamada **Pila** (stack), y los objetos asignados en función de la pila
se pueden reclamar rápidamente al final del alcance. Las variables
globales se asignan en **Tabla global** (tabla global). Los objetos de
la tabla global no se reciclarán una vez creados y se puede acceder a la
tabla desde cualquier parte del programa.

2.2.3 Tipo de variable
~~~~~~~~~~~~~~~~~~~~~~

Berry determina el tipo de variable en tiempo de ejecución. En otras
palabras, la variable puede almacenar cualquier tipo de valor. Por lo
tanto, Berry es un lenguaje de **escritura dinámica**. El intérprete no
deduce el tipo de la variable en tiempo de compilación, lo que puede
provocar que se expongan algunos errores en tiempo de ejecución. Por
ejemplo, el error generado al ejecutar la expresión ``'1' + 1`` es un
error de tiempo de ejecución y no un error del compilador. La ventaja de
usar tipos dinámicos es que se pueden simplificar muchos diseños y el
programa será más flexible, sin mencionar la necesidad de diseñar un
sistema de inferencia de tipos complejo.

Debido a que el intérprete no verifica el tipo, es posible que el código
de usuario deba determinar el tipo de valor por sí mismo, y esta
característica también se puede usar para implementar algunas
operaciones especiales. Esta característica también hace que las
funciones sobrecargadas sean innecesarias. Por ejemplo, la función
nativa ``type`` acepta cualquier tipo de parámetro y devuelve una cadena
que describe el tipo de parámetro.
