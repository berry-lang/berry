4. Declaración
==============

Berry es un lenguaje de programación imperativo. Este paradigma asume
que los programas se ejecutan paso a paso. Normalmente, las
declaraciones de Berry se ejecutan secuencialmente, y esta estructura de
programa se denomina estructura secuencial. Aunque la estructura de la
secuencia es muy básica, las estructuras de rama y las estructuras de
bucle se utilizan normalmente en los programas reales. Berry proporciona
varias declaraciones de control para realizar esta compleja estructura
de flujo, como declaraciones condicionales y declaraciones de iteración.

A excepción de los comentarios de línea, los retornos de carro o los
saltos de línea (“``\r``” y “``\n``”) solo se usan como caracteres en
blanco, por lo que las declaraciones se pueden escribir en líneas.
Además, puede escribir varias declaraciones en la misma línea.

Puede agregar un punto y coma al final de la declaración para indicar el
final de la declaración, pero el intérprete generalmente puede dividir
la declaración automáticamente sin usar un punto y coma. Puede usar
punto y coma para decirle al intérprete cómo analizar el código para el
código que será ambiguo. Sin embargo, es mejor no escribir código
ambiguo.

4.1 Oración simple
------------------

4.1.1 Declaración de expresión
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Las declaraciones de expresión son principalmente declaraciones
compuestas de expresiones de asignación o expresiones de llamada de
función. Otras expresiones también pueden formar oraciones, pero no
tienen significado. Por ejemplo, la expresión ``1+2`` es una oración
escrita sola, pero no tiene ningún efecto. Las siguientes rutinas dan
ejemplos de sentencias de expresión y sentencias de función:

.. code:: berry

   a = 1 #  Declaración de asignación
   print(a) #  Declaración de llamada

La línea 1 es una declaración de asignación simple que asigna el valor
literal ``i`` a la variable ``a``. La declaración en la línea 2 es una
declaración de llamada de función, que imprime el valor de la variable
‘a’ llamando a la función ‘imprimir’.

Las expresiones de líneas cruzadas se escriben de la misma manera que
las expresiones de una sola línea y no se requieren símbolos especiales
de continuación de línea. P.ej:

.. code:: berry

   a = 1 +
       func() # Ajustar línea

También puede escribir varias declaraciones de expresión en una línea y
varios tipos de declaraciones se pueden escribir en una línea. Este
ejemplo pone dos declaraciones de expresión en la misma línea:

.. code:: berry

   b = 1 c = 2 #  sentencias múltiples

A veces el programador quiere escribir dos declaraciones, pero el
intérprete puede pensar erróneamente que es una declaración. Este
problema es causado por la ambigüedad en el proceso de análisis
gramatical. Tome este código como ejemplo:

.. code:: berry

   a = c
   (b) = 1 #  Considérese como una llamada de función

Supongamos que las líneas 1 y 2 están destinadas a ser dos oraciones de
expresión: ``a = c`` y ``(b) = 1``, pero el intérprete las interpretará
como una oración: ``a = c(b) = 1``. La causa de este problema es que el
intérprete analiza incorrectamente ``c`` y ``(b)`` en llamadas de
función. Para evitar ambigüedades, podemos agregar un punto y coma al
final de la declaración para separar claramente la declaración:

.. code:: berry

   a = c; (b) = 1;

Una mejor manera es no usar paréntesis en el lado izquierdo del número
de tarea. Obviamente, no hay razón para usar paréntesis aquí. En
circunstancias normales, las expresiones complejas no deberían aparecer
en el lado izquierdo del operador de asignación, sino solo expresiones
simples compuestas de nombres de variables, expresiones de operación de
dominio y expresiones de operación de subíndice:

.. code:: berry

   a = c b = 1

Usar expresiones simples solo en el lado izquierdo del signo de
asignación no causará ambigüedad en la segmentación de oraciones. Por lo
tanto, en la mayoría de los casos, no es necesario usar punto y coma
para separar expresiones y no recomendamos esta forma de escritura.

Bloque
~~~~~~

Un **Bloque** es una colección de varias oraciones. Un bloque es un
alcance, por lo que solo se puede acceder a las variables definidas en
el bloque dentro del bloque y sus sub-bloques. Hay muchos lugares donde
se utilizan bloques, como declaraciones ``if``, declaraciones ``while``,
declaraciones de funciones, etc. Estas declaraciones contendrán un
bloque a través de un par de palabras clave. Por ejemplo, el bloque
utilizado en la sentencia ``if``:

.. code:: berry

   if isOpen
       close()
       print('el dispositivo fue cerrado')
   end

Las sentencias en las líneas 2 a 3 constituyen un bloque, que se
intercala entre el par de palabras clave ``if`` y ``end`` (la expresión
condicional de la sentencia en ``if`` no está en el bloque). No es
necesario que el bloque contenga declaraciones, lo que constituye un
bloque vacío, o se puede decir que es un bloque que contiene una
declaración vacía. En términos generales, cualquier cantidad de
oraciones consecutivas puede llamarse bloque, pero preferimos expandir
el alcance del bloque tanto como sea posible, lo que puede garantizar
que el área del bloque sea consistente con el alcance del alcance. En el
ejemplo anterior, tendemos a pensar que las filas 2 a 3 son un bloque
completo, que es el rango más grande entre las palabras clave ``if`` y
``end``.

Declaración ``do``
^^^^^^^^^^^^^^^^^^

A veces solo queremos abrir un nuevo ámbito, pero no queremos usar
ninguna declaración de control. En este caso, podemos usar la
instrucción ``do`` para encapsular el bloque, entonces la sentencia no
tiene función de control. La oración tiene la forma:

``do`` **bloque** ``end``

Entre ellos **bloque** está el bloque que necesitamos. Esta instrucción
utiliza un par de palabras clave ``do`` y ``end`` para contener bloques.
La declaración no tiene función de control, ni genera ninguna
instrucción de tiempo de ejecución.

Sentencia condicional
---------------------

Berry proporciona sentencias ``if`` para realizar la función de
ejecución de control condicional. Este tipo de estructura de programa
generalmente se denomina estructura de rama ``if``. La declaración
determinará la rama de ejecución basada en la expresión condicional
verdadera (``true``) o falsa (``false``). En algunos lenguajes, existen
otras opciones para implementar el control condicional. Por ejemplo, los
lenguajes como C y C++ proporcionan sentencias ``switch``, pero para
simplificar el diseño, Berry no admite sentencias ``switch``.

Declaración ``if``
~~~~~~~~~~~~~~~~~~

La instrucción ``if`` se utiliza para implementar la estructura de
rama, que selecciona la rama del programa de acuerdo con el verdadero o
falso de una determinada condición de juicio. La sentencia también puede
incluir la rama ``else`` o la rama ``elif``. La forma simple de
declaración ``if`` sin ramas es

``if`` **condición**    **bloque** ``end``

**condición** es una expresión condicional. Cuando el valor de
**condición\* es ``verdadero``, se ejecutará**\ bloque*\* en la segunda
línea; de lo contrario, se omitirá el **bloque** y se ejecutará la
instrucción que sigue a ``end``. En el caso de que se ejecute
**bloque**, después de que se ejecute la última declaración en el
bloque, dejará la declaración ``if`` y comenzará a ejecutar la
declaración que sigue a ``end``.

Aquí hay un ejemplo para ilustrar el uso de la sentencia ``if``:

.. code:: berry

   if 8 % 2 == 0
       print('este número es par')
   end

Este código se usa para juzgar si el número ‘8’ es par y, si lo es,
generará ‘este número es par’. Aunque este ejemplo es muy simple, es
suficiente para ilustrar el uso básico de las oraciones ``if``.

Si desea tener una rama correspondiente para la ejecución cuando la
condición se cumple y no se cumple, use la instrucción ``if`` con la
rama ``else``. La forma de la oración es:

``if`` **condición**    **bloque** ``else`` **bloque** ``end``

A diferencia de la simple instrucción ``if``, la declaración ``if else``
ejecutará **bloque** en la rama ``else`` cuando el valor de
**condición** sea ``falso``. No importa qué rama se ejecute bajo
**bloque**, después de que se ejecute la última declaración en el
bloque, aparecerá la declaración ``if else``, es decir, se ejecutará la
declaración después de ``end``. En otras palabras, no importa si el
valor de **condición** es ``verdadero`` o ``falso``, se ejecutará un
**bloque**.

Continúe usando el juicio de paridad como ejemplo, esta vez cambie la
demanda para generar la información correspondiente de acuerdo con la
paridad del número de entrada. El código para lograr este requisito es:

.. code:: berry

   if x % 2 == 0
       print('este número es par')
   else
       print('este número es impar')
   end

Antes de ejecutar este código, primero debemos asignar un valor entero a
la variable ``x``, que es el número cuya paridad queremos comprobar. Si
‘x’ es un número par, el programa generará ‘este número es par’; de lo
contrario, generará ‘este número es impar’. A veces necesitamos anidar
declaraciones ``if``. Una forma es anidar una instrucción ``if`` debajo
de la rama ``else``. Este es un requisito muy común porque muchas
condiciones deben juzgarse consecutivamente. Para este tipo de demanda,
use la instrucción ``if else`` para escribir:

.. code:: berry

   if expr
       bloque
   else
       if expr
           bloque
       end
   end

Obviamente, esta forma de escribir aumentará el nivel de sangría del
código, y es más engorroso usar múltiples ``end`` al final. Como mejora,
Berry proporciona la rama ``elif`` para optimizar la escritura anterior.
Usar la rama ``elif`` es equivalente al código anterior, en la forma

``if`` **condición** **bloque** ``elif`` **condición** **bloque**
``else`` **bloque** ``end``

La rama debe usarse después de la rama ``if`` y antes de la rama, y la
rama ``elif`` se puede usar varias veces seguidas. Si se cumple la
**condición** correspondiente a la rama ``elif``, se ejecutará el
**bloque** debajo de la rama. La ramificación ``elif`` es adecuada para
situaciones que requieren que se juzguen múltiples condiciones en
secuencia.

Usamos un fragmento de código que juzga positivo, negativo y 0 para
demostrar la rama ``elif``:

.. code:: berry

   if x> 0
       print('positivo')
   elif x == 0
       print('cero')
   else
       print('negativo')
   end

Aquí también, la variable ``x`` debe asignarse primero. Este código es
muy simple y no será explicado.

Algunos lenguajes tienen un problema llamado “``else``” colgante, que se
refiere a cuando una oración ``if`` está anidada dentro de otra oración
``if``, ¿a dónde pertenece la rama ``else``? Es un problema con la
sentencia ``if``. Cuando usamos C/C++, debemos considerar el problema de
colgar ``else``. Para evitar la ambigüedad en el problema de
``if else``, los programadores de C/C++ a menudo usan llaves para
convertir una rama en un bloque. En Berry, la rama de la instrucción
``if`` debe ser un bloque, lo que también determina que Berry no tiene
el problema de sobresalir por ``else``.

Declaración de iteración
------------------------

Las declaraciones iterativas también se denominan declaraciones de
bucle, que se utilizan para repetir ciertas operaciones hasta que se
cumple la condición de terminación. Berry proporciona las declaraciones
``while`` y ``for``, dos declaraciones de iteración. Muchos lenguajes
también proporcionan estas dos declaraciones para la iteración. La
declaración ``while`` de Berry es similar a la declaración ``while`` en
C/C++, pero la declaración ``for`` de Berry solo se usa para recorrer
los elementos en el contenedor, similar a la declaración ``foreach``
proporcionada por algunos lenguajes y la que se introdujo por el nuevo
estilo en C++11 de ``for``. No se admite la instrucción ``for`` de
estilo C.

Sentencia ``while``
~~~~~~~~~~~~~~~~~~~

La declaración ``while`` es una declaración iterativa básica. La
instrucción ``while`` utiliza una condición de juicio. Cuando la
condición es verdadera, el cuerpo del ciclo se ejecuta repetidamente; de
lo contrario, el ciclo finaliza. El patrón de la declaración es

``while`` **condición**    **bloque** ``end``

Cuando el programa ejecuta la sentencia ``while``, comprobará si la
expresión **condición** es verdadera o falsa. Si es cierto, ejecuta el
**bloque** del cuerpo del ciclo; de lo contrario, finaliza el ciclo.
Después de ejecutar la última declaración en **bloque**, el programa
saltará al comienzo de la declaración ``while`` y comenzará la siguiente
ronda de detección. Si la expresión de **condición** es falsa cuando se
evalúa por primera vez, el **bloque** del cuerpo del bucle no se
ejecutará en absoluto (al igual que la expresión de **condición** de la
declaración ``if`` es falsa). En términos generales, el valor de la
expresión **condición** debería poder cambiar durante el ciclo, en lugar
de ser una constante o una variable modificada fuera del ciclo, lo que
hará que el ciclo no se ejecute o no termine. Un bucle que nunca termina
se llama bucle sin fin. Por lo general, esperamos que el ciclo se
ejecute un número específico de veces y luego termine. Por ejemplo,
cuando usamos el bucle ``while`` para acceder a todos los elementos de
la matriz, esperamos que el número de ejecuciones del bucle sea igual a
la longitud de la matriz, por ejemplo:

.. code:: berry

   i = 0
   l = ['a','b','c']
   while i < l.size()
       print(l[i])
       i = i + 1
   end

Este bucle obtiene los elementos del arreglo ``l`` y los imprime. Usamos
una variable ``i`` como contador de bucles e índice de matriz. Dejamos
que el valor de ``i`` alcance la longitud de la matriz ``l`` para
finalizar el bucle. En la última línea del cuerpo del bucle, añadimos
``1`` al valor de ``i`` para asegurar que se acceda al siguiente
elemento de la matriz en el siguiente bucle, y el bucle ``while``
finaliza cuando el número de bucles alcanza la longitud de la matriz.

Sentencia ``for``
~~~~~~~~~~~~~~~~~

La instrucción ``for`` de Berry se usa para recorrer los elementos
en el contenedor, y su forma es

``for`` **variable** ``:`` **expresión** **bloque** ``end``

**expresión** El valor de la expresión debe ser un contenedor iterable o
una función, como la clase ``range``. La declaración obtiene un iterador
del contenedor y obtiene un elemento en el contenedor cada vez que se
llama al iterador.

**variable** se denomina variable de iteración, que siempre se define en
la instrucción ``for``. Por lo tanto, **variable** debe ser un nombre de
variable y no una expresión. El elemento contenedor obtenido del
iterador en cada bucle se asignará a la variable de iteración. Este
proceso ocurre antes de la primera declaración en **bloque**.

La declaración ``for`` verificará si hay elementos no visitados en el
iterador para la iteración. Si los hay, comenzará la siguiente
iteración; de lo contrario, finalizará la declaración ``for`` y
ejecutará la declaración que sigue a ``end``. Actualmente, Berry solo
proporciona iteradores de solo lectura, lo que significa que los
elementos del contenedor no se pueden modificar a través de las
variables de iteración en la instrucción ``for``.

El alcance de la variable de iteración **variable** se limita al
**bloque** del cuerpo del ciclo, y la variable no tendrá ninguna
relación con la variable con el mismo nombre fuera del alcance. Para
ilustrar este punto, usemos un ejemplo para ilustrar. En este ejemplo,
usamos la instrucción ``for`` para acceder a todos los elementos en la
instancia ``range`` e imprimirlos. Por supuesto, también usamos este
ejemplo para demostrar el alcance de las variables de bucle.

.. code:: berry

   i = "Hola, estoy bien". # Variable exterior
   for i: 0 .. 2
       print(i) #  variable de iteración
   end
   print(i)

En este ejemplo, en relación con la variable de iteración ``i`` definida
en la línea 2, la variable ``i`` definida en la línea 1 es una variable
externa. Al ejecutar este ejemplo obtendrá el siguiente resultado

``0 1 2 Hola, estoy bien``

Se puede ver que la variable de iteración ``i`` y la variable externa
``i`` son dos variables diferentes. Solo tienen el mismo nombre pero
diferentes alcances.

Principio de enunciado ``for``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A diferencia de la sentencia iterativa tradicional ``while``, la
sentencia ``for`` utiliza iteradores para atravesar el contenedor. Si
necesita usar la declaración ``for`` para atravesar una clase
personalizada, debe comprender su mecanismo de implementación. Cuando se
usa la instrucción ``for``, el intérprete oculta muchos detalles de
implementación. De hecho, para dicho código:

.. code:: berry

   for i: 0 .. 2
       print(i)
   end

Será traducido al siguiente código equivalente por el intérprete:

.. code:: berry

   var it = __iterator__(0 .. 2)
   try
       while true
           var i = it()
           print(i)
       end
   except 'stop_iteration'
       #  no hacer nada
   end

Hasta cierto punto, la declaración ``for`` es solo un azúcar sintáctico,
y es esencialmente solo una forma simple de escribir una pieza de código
complejo. En este código equivalente se usa una variable intermedia
``it``. El valor de la variable es un iterador y, en este ejemplo, es un
iterador del contenedor ``range`` ``0..2``. Al procesar la instrucción
``for``, el intérprete oculta la variable intermedia del iterador, por
lo que no se puede acceder a ella en el código.

El parámetro de la función ``__iterator__`` es un contenedor y la
función devuelve un iterador de parámetros. Esta función obtiene el
iterador llamando al método de parámetro. Por lo tanto, si el valor de
retorno del método ``iter`` es un tipo de instancia (``instance``), esta
instancia debe tener un método ``next`` y un método ``hasnext``.

El parámetro de la función ``__hasnext__`` es un iterador, que comprueba
si el iterador tiene el siguiente elemento llamando al método
``hasnext`` del iterador ``hasnext`` El valor de retorno del método es
de tipo ``booleano``. El parámetro de la función ``__next__`` también es
un iterador, que obtiene el siguiente elemento en el iterador llamando
al método ``next`` del iterador.

Hasta ahora, las funciones ``__iterator__``, ``__hasnext__`` y
``__next__`` simplemente llaman a algunos métodos del contenedor o
iterador y luego devuelven el valor de retorno de estos métodos. Por lo
tanto, la escritura equivalente de la instrucción ``for`` también se
puede simplificar de esta forma:

.. code:: berry

   do
       var it = (0 .. 2).iter()
       while (it.hasnext())
           var i = it.next()
           print(i)
       end
   end

Este código es más fácil de leer. Se puede ver en el código que el
alcance de la variable iteradora ``it`` es la declaración ``for``
completa, pero no es visible fuera de la declaración ``for``, mientras
que el alcance de la variable de iteración ``i`` está en el cuerpo del
bucle, por lo que cada iteración definirá nuevas variables de iteración.

Declaración de salto
--------------------

La declaración de salto proporcionada por Berry se usa para realizar el
salto del flujo del programa en el proceso de bucle. Las sentencias de
salto se dividen en sentencias de “ruptura” y sentencias de
“continuación”. Estas dos declaraciones deben usarse dentro de
declaraciones iterativas y solo pueden usarse dentro de funciones para
saltar. Algunos lenguajes proporcionan sentencias ``goto`` para realizar
saltos arbitrarios dentro de las funciones, que Berry no proporciona,
pero los efectos de las sentencias ``goto`` se pueden reemplazar por
sentencias condicionales y sentencias de iteración.

Declaración ``break``
---------------------

``break`` se usa para terminar la declaración de iteración y saltar.
Después de la ejecución de la sentencia ``break``, el nivel más cercano
de la sentencia de iteración terminará inmediatamente y la ejecución
continuará desde la posición de la primera sentencia después de la
sentencia de iteración. Para ilustrar el flujo de ejecución de la
declaración ``break``, usamos un ejemplo para demostrarlo:

.. code:: berry

   while true
       print('antes del break')
       break
       print('después del break')
   end
   print('fuera del bucle')

En este código, la sentencia ``break`` está en un bucle ``while``. Antes
y después de la declaración ``break`` y después de la declaración
``while``, hemos colocado una declaración de impresión para probar el
flujo de ejecución del programa. El resultado de este código es:

::

   antes del break
   fuera del bucle

Esto muestra que la sentencia ``while`` finaliza el bucle en la posición
de la sentencia ``break`` en la tercera línea y el programa continúa
ejecutándose desde la sexta línea.

Declaración ``continue``
~~~~~~~~~~~~~~~~~~~~~~~~

Esta declaración también se usa dentro de una declaración de iteración.
Su función es finalizar una iteración e iniciar inmediatamente la
siguiente ronda. Por lo tanto, después de la ejecución de la sentencia
``continue``, el código restante en la sentencia de iteración de la capa
más cercana ya no se ejecutará, pero comenzará una nueva ronda de
iteración. Aquí usamos una sentencia ``for`` para demostrar la función
de la sentencia ``continue``:

.. code:: berry

   for i: 0 .. 5
       if i >= 2
           continue
       end
       print('i =', i)
   end
   print('fuera del bucle')

Aquí, la instrucción ``for`` iterará 6 veces. Cuando la variable de
iteración ``i`` es mayor o igual que ``2``, se ejecutará la declaración
``continue`` en la línea 3, y la declaración de impresión en la línea 5
no se ejecutará a partir de entonces. En otras palabras, la línea 5 solo
se ejecutará en las dos primeras iteraciones (en este momento
``i < 2``). El resultado de ejecución de esta rutina es:

::

   i = 0
   i = 1
   fuera del bucle

Se puede ver que el valor de la variable ``i`` solo se imprime dos
veces, lo cual está en línea con las expectativas. Los lectores pueden
intentar imprimir el valor de la variable ``i`` antes de la instrucción
``continue``. Encontrará que la declaración ``for`` itera 6 veces, lo
que indica que la declaración ``continue`` no finaliza la iteración.

Declaración ``import``
----------------------

Berry tiene algunos módulos predefinidos, como el módulo ``math`` para
cálculos matemáticos. Estos módulos no se pueden usar directamente, sino
que se deben importar con la instrucción ``import``. Hay dos formas de
importar un módulo:

``import`` **nombre**

``import`` **nombre** ``as`` **variable**

**nombre** Para importar el nombre del módulo, al usar el primer método
de escritura para importar el módulo, el módulo importado se puede
llamar directamente usando el nombre del módulo. La segunda forma de
escribir es importar un módulo llamado **nombre** y modificar el nombre
del módulo al llamarlo a **variable**. Por ejemplo, un módulo llamado
``math``, usamos el primer método para importar y usar:

.. code:: berry

   import math
   math.sin(0)

Aquí usa directamente ``math`` para llamar al módulo. Si el nombre de un
módulo es relativamente largo y no es conveniente escribirlo, puede usar
la instrucción ``import as``. Aquí, asuma un módulo llamado
``hardware``. Queremos llamar a la función ``setled`` del módulo,
podemos importar el ``hardware`` del módulo a la variable llamada ``hw``
y usar:

.. code:: berry

   import hardware as hw
   hw.setled(true)

Para encontrar módulos, todas las rutas en ``sys.path()`` se exploran
secuencialmente. Si desea agregar una ruta específica antes de la
importación (como leer desde la tarjeta SD), puede usar la siguiente
función de ayuda:

.. code:: berry

   def push_path(p)
     import sys
     var path = sys.path()
     if path.find(p) == nil  # agregar solo si aún no está allí
       path.push(p)
     end
   end

Manejo de excepciones
---------------------

El mecanismo permite que el programa capture y maneje las excepciones
que ocurren durante el tiempo de ejecución. Berry admite un mecanismo de
captura de excepciones que permite separar el proceso de captura y
manejo de excepciones. Es decir, parte del programa se usa para detectar
y recopilar excepciones, y la otra parte del programa se usa para
manejar excepciones.

En primer lugar, el programa problemático necesita lanzar primero una
excepción. Cuando estos programas están en un bloque de manejo de
excepciones, un programa específico atrapará y manejará la excepción.

Generar una excepción
~~~~~~~~~~~~~~~~~~~~~

El uso de la instrucción ``raise`` genera una excepción ``raise``. La
declaración pasará un valor para indicar el tipo de excepción para que
pueda ser identificada por un manejador de excepciones específico. A
continuación se explica cómo utilizar la sentencia ``raise``:

``raise`` **excepción**

``raise`` **excepción**\ ``,``\ **mensaje**

El valor de la expresión **excepción** son los **valores atípicos**
arrojados; la expresión de **mensaje** opcional suele ser una cadena que
describe la información de la excepción, y esta expresión se denomina
**parámetro anómalo**. Berry permite que cualquier valor se use como un
valor anormal, por ejemplo, una cadena se puede usar como un valor
anormal:

.. code:: berry

   raise 'mi_error','un ejemplo de subida'

Después de que el programa se ejecute en la declaración ``raise``, no
continuará ejecutando las declaraciones que le siguen, sino que saltará
al bloque de manejo de excepciones más cercano. Si el bloque de manejo
de excepciones más reciente está en otras funciones, las funciones a lo
largo de la cadena de llamadas se cerrarán antes. Si no hay un bloque de
manejo de excepciones, se producirá una **salida anormal** y el
intérprete imprimirá el mensaje de error de la excepción y la pila de
llamadas de la ubicación del error. Cuando la instrucción ``raise`` está
en el bloque de instrucciones ``try``, la excepción será capturada por
este último. La excepción capturada será manejada por el bloque
``except`` asociado con el bloque ``try``. Si la excepción lanzada puede
ser manejada por el bloque ‘except’, la ejecución de este bloque
continuará desde la declaración después del último bloque ``except``. Si
ninguna de las sentencias ``except`` pueden manejar la excepción, la
excepción se volverá a generar hasta que se pueda manejar o la excepción
finalice.

Valores atípicos
^^^^^^^^^^^^^^^^

En Berry, puede usar cualquier valor como valor atípico, pero
generalmente usamos cadenas cortas. Berry también puede lanzar algunas
excepciones internamente. Llamamos a estas excepciones **Excepción
estándar**. Todos los valores de excepción estándar son de tipo cadena.

+------------+-------------------------------+-------------------------+
| **Valores  | **Descripción**               | **Descripción del       |
| atípicos** |                               | parámetro**             |
+============+===============================+=========================+
| ``asser    | Afirmación fallida            | Información sobre       |
| t_failed`` |                               | excepciones específicas |
+------------+-------------------------------+-------------------------+
| ``ind      | (generalmente fuera de los    | Información sobre       |
| ex_error`` | límites)                      | excepciones específicas |
+------------+-------------------------------+-------------------------+
| ``         | Mal funcionamiento de E/S     | Información sobre       |
| io_error`` |                               | excepciones específicas |
+------------+-------------------------------+-------------------------+
| ``k        | Error clave                   | Información sobre       |
| ey_error`` |                               | excepciones específicas |
+------------+-------------------------------+-------------------------+
| ``runti    | Excepción de tiempo de        | Información sobre       |
| me_error`` | ejecución de máquina virtual  | excepciones específicas |
+------------+-------------------------------+-------------------------+
| ``stop_i   | Fin del iterador              | no                      |
| teration`` |                               |                         |
+------------+-------------------------------+-------------------------+
| ``synt     | Error de sintaxis             |                         |
| ax_error`` |                               |                         |
+------------+-------------------------------+-------------------------+
| por el     |                               |                         |
| compilador |                               |                         |
+------------+-------------------------------+-------------------------+
| ``unrealiz | Función no realizada          | Información sobre       |
| ed_error`` |                               | excepciones específicas |
+------------+-------------------------------+-------------------------+
| ``ti       | Error de tipo                 | Información sobre       |
| pe_error`` |                               | excepciones específicas |
+------------+-------------------------------+-------------------------+

*Tabla 8: Lista de excepciones estándar*

Capturar excepciones
~~~~~~~~~~~~~~~~~~~~

Utilice la instrucción ``except`` para detectar excepciones. Debe estar
emparejado con la sentencia ``try``, es decir, un bloque de sentencia
``try`` debe ir seguido de uno o más bloques de sentencia ``except``. La
forma básica de la oración ``try-except`` es

| ``try`` **bloque**
| ``except`` **…**   **bloque**   ``end``


La rama ``except`` puede tener las siguientes formas

| ``except ..`` ``except`` **excepciones**
| ``except`` **excepciones**\ ``as`` **variable**
| ``except`` **excepciones**\ ``as`` **variable** ``,`` **mensaje**
| ``except .. as`` **variable**
| ``except .. as`` **variable** ``,`` **mensaje**


La instrucción ``except`` más básica no usa parámetros, esta rama
``except`` capturará todas las excepciones; **Lista de excepciones de
captura**: **excepciones** es una lista de valores atípicos que pueden
coincidir con la correspondiente rama ``except``, que se utiliza entre
varios valores de la lista Separados por comas; **variable** es
**variable anormal**, si la rama detecta una excepción, el valor atípico
se vinculará a la variable; **mensaje** es **Variable de parámetro
anómalo**, si la rama detecta una excepción, el valor del parámetro
anómalo se vinculará a la variable.

Cuando se detecta una excepción en el bloque de instrucciones ``try``,
el intérprete verificará la rama ``except`` una por una. Si el valor de
la excepción existe en la lista de captura de una rama, se llamará al
bloque de código debajo de la rama para manejar la excepción, y la
declaración ``try-except`` completa se cerrará después de que se ejecute
el bloque de código. Si ninguna de las ramas ``except`` coinciden, el
controlador de excepciones externo volverá a lanzar y capturar y manejar
la excepción.
