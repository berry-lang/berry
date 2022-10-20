.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

5. Función
==========

Una **función** es una “subrutina” que puede ser llamada por un código
externo. Como parte del programa, la función en sí también es una pieza
de código. Una función puede tener 0 o más parámetros y devolverá un
resultado, que se denomina **valor de retorno** de la función.

En Berry, la función es un **valor de primera clase**. Por lo tanto,
además de llamar a funciones, también puede pasar funciones como
valores, por ejemplo, vincular funciones a variables, usar funciones
como valores devueltos, etc.

5.1 Información básica
----------------------

El uso de funciones incluye dos partes: definición de función y llamada.
La declaración de definición de función usa la palabra clave ``def``
como el comienzo. La definición de la función es el proceso de
empaquetar y nombrar el código del cuerpo de la función. Este proceso
solo genera la estructura de la función y no ejecuta la función. La
función de ejecución debe usar un **operador de llamada**, que es un par
de paréntesis. El operador de llamada actúa sobre una expresión cuyo
resultado es un tipo de función. Los parámetros que se pasan a la
función se escriben entre paréntesis y los parámetros múltiples se
separan con comas. El resultado de la expresión de llamada es el valor
de retorno de la función.

5.1.1 Definición de funciones
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Función con nombre
^^^^^^^^^^^^^^^^^^

Una **función con nombre** es una función a la que se le da un nombre
cuando se define. Su declaración de definición consta de las siguientes
partes: palabra clave ``def``, nombre de funcion, lista que constan de 0
a múltiples parámetros y cuerpo de función, múltiples parámetros en la
lista de parámetros están separados por comas, y todos los parámetros
están escritos en un par de paréntesis. Llamamos al parámetro cuando la
función se define como **Parámetros formales**, y al parámetro cuando
llamamos a la función como **Argumentos**. La forma general de la
definición de la función es:

.. code::

      ’def’ name ´(´ argumentos ´)´
        bloque
      ´end’

El nombre de función **nombre** es un identificador; **argumentos** es
la lista de parámetros formales; **bloque** es el cuerpo de la función.
Si el cuerpo de la función es una declaración vacía, la función se
denomina “función vacía”. La declaración del valor de retorno de la
función está contenida en el cuerpo de la función. Si no hay declaración
de devolución en **bloque**, la función devolverá ``nil`` por defecto.
El nombre de la función es en realidad el nombre de la variable del
objeto de la función vinculada. Si el nombre ya existe en el ámbito
actual, definir la función equivale a vincular el objeto de función a
esta variable.

El siguiente ejemplo define una función llamada ``add``. La función de
este ejemplo es sumar dos números y devolver el resultado.

.. code:: berry

   def add(a, b)
       return a + b
   end

La función ``add`` tiene dos parámetros ``a`` y ``b``, y los dos
sumandos se pasan a la función a través de estos parámetros para el
cálculo. La instrucción ``return`` devuelve el resultado del cálculo.

Una función como atributo de clase se llama método. Esta parte del
contenido se explicará en el capítulo orientado a objetos.

Función anónima
^^^^^^^^^^^^^^^

A diferencia de las funciones con nombre, la **función anónima** no
tiene nombre y su expresión de definición tiene la forma:

.. code::

      ´def’ ´(´ argumentos ´)´
        bloque
      ´end’

Se puede ver que, en comparación con las funciones con nombre, no hay un
**nombre** de función en su definición.. La definición de una función
anónima es esencialmente una expresión, que se denomina **Función
literal**. Para usar funciones anónimas podemos vincular el valor
literal de la función a una variable:

.. code:: berry

   add = def (a, b)
       return a + b
   end

La función de este código es exactamente la misma que la función ``add``
en la sección anterior. Se puede usar una función anónima para pasar
convenientemente el valor de la función como un valor literal. Al igual
que otros tipos de literales, los literales de función también son la
unidad de expresión más pequeña. Por lo tanto, lo que hay entre las
palabras clave ``def`` y ``end`` es un todo indivisible.

Función de llamada
~~~~~~~~~~~~~~~~~~

Tome la función ``add`` como ejemplo. Para llamar a esta función, debe
proporcionar dos valores y puede obtener la suma de los dos números
llamando a la función:

.. code:: berry

   res = add(5, 3)
   print(res) # 8

Llamamos a la función llamada (la función ``add`` en el ejemplo) como
**Función llamada**, y la función que llama a la función llamada (la
función ``principal`` en el ejemplo) como **Función clave**. El proceso
de llamada de función es el siguiente: Primero, el intérprete
(implícitamente) inicializará la lista de parámetros formales de la
función llamada con la lista de argumentos y, al mismo tiempo,
suspenderá la función de llamada y guardará su estado, luego creará un
entorno para la función llamada y ejecutará la función llamada.

La función finalizará su ejecución cuando encuentre la instrucción
``return`` y pase el valor de retorno a la función que llama. El
intérprete destruirá el entorno de la función llamada después de que
regrese la función llamada, luego restaurará el entorno de la función
que llama y continuará ejecutando la función que llama. El valor de
retorno de la función también es el resultado de la expresión de la
llamada a la función. El siguiente ejemplo define una función
``cuadrado`` y vincula esta función a una variable ``f``, y luego llama
a la función ``cuadrado`` a través de la variable ``f``. Este uso es
similar a los punteros de función en lenguaje C.

.. code:: berry

   def cuadrado(n)
       return n * n
   end
   f = cuadrado
   print(f(5)) # 25

Cabe señalar que el objeto de la función solo está vinculado a estas
variables (consulte la sección Capitulo-3: Operador de asignación

.. code:: berry

   f = cuadrado
   cuadrado = nil
   print(f(5)) # 25

Se puede ver que la función todavía se puede llamar normalmente después
de reasignar ``cuadrado``. Solo después de que el objeto de función ya
no esté vinculado a ninguna variable, se perderá y el sistema reciclará
los recursos ocupados por este tipo de objeto de función.

Desviar la llamada
^^^^^^^^^^^^^^^^^^

La llamada de la función debe estar en el ámbito de la variable de
función, por lo que normalmente no se puede llamar antes de que se
defina la función. Para resolver este problema, puede utilizar este
método para comprometer:

.. code:: berry

   var func1
   def func2(x)
       return func1(x)
   end
   def func1(x)
       return x * x
   end
   print(func2(4)) # 16

En este ejemplo, ``func2`` llama a ``func1``, pero la función ``func1``
se define después de ``func2``. Después de ejecutar este código, el
programa generará el resultado correcto ``16``. Esta rutina utiliza el
mecanismo de que no se llamará a la función cuando se defina. Defina la
variable ``func1`` antes de definir ``func2`` para asegurarse de que el
símbolo ``func1`` no se encontrará durante la compilación. Luego
definimos la función ``func1`` después de ``func2`` para que la función
se use para sobrescribir el valor de la variable ``func1``. Cuando se
llama a la función ``func2`` en la última línea ``print(func2(4))``, la
variable ``func1`` ya es la función que necesitamos, por lo que se
mostrará el resultado correcto.

Llamada recursiva
^^^^^^^^^^^^^^^^^

Con **función recursiva** se refiere a funciones que se llaman a sí
mismas directa o indirectamente. La recursividad se refiere a una
estrategia que divide el problema en subproblemas similares y luego los
resuelve. Tomando el factorial como ejemplo, la definición recursiva de
factorial es 0! = 1, *n*! = *n* ⋅ (*n*\ −1)!. Entonces podemos escribir
la función recursiva para calcular el factorial según la definición:

.. code:: berry

   def fact(n)
       if n == 0
           return 1
       end
       return n * fact(n-1)
   end

Tome el factorial de 5 como ejemplo, el proceso de calcular manualmente
el factorial de 5 es: ¡5! = 5 × 4 × 3 × 2 × 1 = 120. El resultado de
llamar a la función ``fact`` también es 120:

.. code:: berry

   print(fact(5)) # 120

Para garantizar que la profundidad de la llamada recursiva sea limitada
(un nivel de recursividad demasiado profundo agotará el espacio de la
pila), la función recursiva debe tener una condición de finalización. En
``fact`` la declaración ``if`` en la segunda línea de la definición de
la función se usa para detectar la condición final, y el proceso
recursivo finaliza cuando ``n`` se calcula como ``0``. La fórmula
factorial anterior no se aplica a parámetros no enteros. Ejecutar una
expresión como ``fact(5.1)`` provocará un error de desbordamiento de
pila debido a la imposibilidad de finalizar la recursividad.

Existe otra situación, la ``Recurrencia indirecta``, es decir, la
función no es llamada por sí misma sino por otra función (directa o
indirectamente) llamada por ella. La recursividad indirecta generalmente
requiere el uso de técnicas de llamada de función hacia adelante. Tome
las funciones ``es_impar`` y ``es_par`` para calcular números pares e
impares como ejemplos:

.. code:: berry

   var es_impar
   def es_par(n)
       if n == 0
           return true
       end
       return es_impar(n-1)
   end
   def es_impar(n)
       if n == 0
           return false
       end
       return es_par(n-1)
   end

Estas dos funciones se llaman entre sí. Para garantizar que este nombre
esté en el alcance cuando se llama a la función ``es_impar`` en la línea
6, la variable ``es_impar`` se define en la línea 1.

Llamada de función anónima
^^^^^^^^^^^^^^^^^^^^^^^^^^

Si una función anónima solo se llamará una vez, la forma más fácil es
llamarla cuando esté definida, por ejemplo:

.. code:: berry

   res = def (a, b) return a + b end (1, 2) # 3

En esta rutina, usamos la expresión de llamada directamente después del
literal de función para llamar a la función. Este uso es muy adecuado
para funciones que solo se llamarán en un lugar.

También puede vincular una función anónima a una variable y llamarla:

.. code:: berry

   add = def (a, b) return a + b end
   res = add(1, 2) # 3

Este uso es similar a la llamada de una función con nombre,
esencialmente llamando a la variable vinculada al valor de la función.
Cabe señalar que es más difícil realizar llamadas recursivas a funciones
anónimas, a menos que utilice técnicas de llamada de reenvío.

Parámetros formales y reales
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La función utiliza parámetros reales para inicializar los parámetros
formales cuando se llama. En circunstancias normales, el parámetro real
y el parámetro de forma son iguales y las posiciones se corresponden
entre sí, pero Berry también permite que el parámetro real sea diferente
del parámetro formal: si el parámetro real es mayor que el parámetro
formal, el parámetro real adicional al parámetro será descartado. De
otra forma los parámetros formales restantes se inicializarán a ``nil``.

El proceso de paso de parámetros es similar a la operación de
asignación. Para los tipos ``nil``, ``boolean`` y numéricos, el paso de
parámetros es por valor, mientras que otros tipos son por referencia.
Para el tipo de referencia de paso de escritura, como una instancia,
modificarlos en la función llamada también modificará el objeto en la
función de llamada. El siguiente ejemplo demuestra esta función:

.. code:: berry

   var l = [], i = 0
   def func(a, b)
       a.push(1)
       b ='cadena'
   end
   func(l, i)
   print(l, i) # [1] 0

Se puede ver que el valor de la variable ``l`` ha cambiado después de
llamar a la función ``func``, pero el valor de la variable ``i`` no ha
cambiado.

Función con número variable de argumentos (vararg)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Puede definir una función para tomar cualquier número arbitrario de
argumentos e iterarlos. Por ejemplo, ``print()`` toma cualquier cantidad
de argumentos e imprime cada uno de ellos separados por espacios. Debe
definir el último argumento como una captura de todos los argumentos
usando ``*`` antes de su nombre.

Todos los argumentos que siguen a los argumentos formales se agrupan en
tiempo de ejecución en una instancia de ``list``. Si no se capturan
argumentos, la lista está vacía.

Ejemplo:

.. code:: berry

   def f(a, b, *c) return size(c) end
   f(1,2) # devuelve 0, c is []
   f(1,2,3) # devuelve 1, c is [3]
   f(1,2,3,4) # devuelve 2, c is [3,4]

Llamar a una función con un número dinámico de argumentos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La sintaxis de Berry solo permite llamar con un número fijo de
argumentos. Utilice la función ``call(f, [args])`` para pasar cualquier
número de argumentos arbitrario.

Puede agregar estáticamente cualquier número de argumentos a ``call()``.
Si el último argumento es una ``lista``, se expande automáticamente a
argumentos discretos.

Ejemplo:

.. code:: berry

   def f(a,b) return nil end

   call(f,1)        # llama a f(1)
   call(f,1,2)      # llama a f(1,2)
   call(f,1,2,3)    # llama a f(1,2,3), el último argumento es ignorado por f
   call(f,1,[2,3])  # llama a f(1,2,3), el último argumento es ignorado por f
   call(f,[1,2])    # llama a f(1,2)
   call(f,[])       # llama a f()

Puede combinar ``call`` y vararg. Por ejemplo, creemos una función que
actúe como ``print()`` pero convierta todos los argumentos a mayúsculas.

Ejemplo completo:

.. code:: berry

   def print_upper(*a) # toma un número arbitrario de argumentos, args es una lista
       import string
       for i:0..size(a)-1
           if type(a[i]) == 'string'
               a[i] = string.toupper(a[i])
           end
       end
       call(print, a) #  llama a print con todos los argumentos
   end

   print_upper("a",1,"Foo","Bar")  # imprime: A 1 FOO BAR

Funciones y variables locales
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

El cuerpo de la función en sí es un ámbito, por lo que las variables
definidas en la función son todas variables locales. A diferencia de los
bloques directamente anidados, cada vez que se llama a una función, se
asigna espacio para las variables locales. El espacio para las variables
locales se asigna en la pila y la información de asignación se determina
en el momento de la compilación, por lo que este proceso es muy rápido.
Cuando se anidan varios niveles de alcance en una función, el intérprete
asigna espacio de pila para la cadena de anidamiento de alcance con la
mayoría de las variables locales, en lugar del número total de variables
locales en la función.

Declaración ``return``
~~~~~~~~~~~~~~~~~~~~~~

La declaración ``return`` se utiliza para devolver el resultado de una
función, es decir, el valor de retorno de la función. Todas las
funciones en Berry tienen un valor de retorno, pero no puede usar
ninguna declaración ``return`` en el cuerpo de la función. En este
momento, el intérprete generará una declaración ``return``
predeterminada para garantizar que la función regrese ``return``. Hay
dos formas de escribir oraciones:

.. container:: algorithm

   ::

      ´return’
      ´return’ expresión

La primera forma de escribir es escribir solo la palabra clave
``return`` y no la expresión que se devolverá. En este caso, se devuelve
el valor ``nil`` predeterminado. La segunda forma de escribir es seguir
la expresión **expresión** después de la palabra clave ``return``, y el
valor de la expresión se usará como valor de retorno de la función.
Cuando el programa ejecuta la declaración ``return``, la función que se
está ejecutando actualmente finalizará la ejecución y volverá al código
que llamó a la función para continuar ejecutándose.

Cuando se usa una palabra clave separada ``return`` como declaración de
retorno de una función, es fácil causar ambigüedad. En ese caso se
recomienda agregar un punto y coma después de ``return`` para evitar
errores:

.. code:: berry

   def func()
       return;
       x = 1
   end

En este ejemplo, la declaración ``x = 1`` después de la declaración
``return`` no se ejecutará, por lo que es redundante. Si se evita este
tipo de código redundante, la instrucción ``return`` suele ir seguida de
palabras clave como ``end``, ``else`` o ``elif``. En este caso, incluso
si se usa una declaración ``return`` por separado, no hay necesidad de
preocuparse por la ambigüedad.

Cierre (closure)
----------------

Conceptos básicos
~~~~~~~~~~~~~~~~~

Como se mencionó anteriormente, las funciones son el primer tipo de
valor en Berry. Puede definir funciones en cualquier lugar y también
puede pasar funciones como parámetros o devolver valores. Cuando se
define otra función en una función, la función anidada puede acceder a
las variables locales de cualquier función externa. Llamamos a las
“variables locales de la función externa” utilizadas en la función la
función como **Variables libres**. Las variables libres generalizadas
también incluyen variables globales, pero no existe tal regla en Berry.
El **Cierre** es una técnica que vincula funciones a **entornos**. El
entorno es un mapeo que asocia cada variable libre de una función con un
valor. En términos de implementación, los cierres asocian el prototipo
de función con sus propias variables. Los prototipos de funciones se
generan en tiempo de compilación y el entorno es un concepto de tiempo
de ejecución, por lo que los cierres también se generan dinámicamente en
tiempo de ejecución. Cada cierre vincula el prototipo de función al
entorno cuando se genera, como se ve en el siguiente ejemplo:

.. code:: berry

   def func(i) # La función externa
       def foo() # La función interna (closure)
           print(i)
       end
       foo()
   end

La función interna ``foo`` es un cierre y tiene una variable libre
``i``, que es un parámetro de la función externa ``func``. Cuando se
genera el cierre ``foo``, su prototipo de función se vincula al entorno
que contiene la variable libre ``i``. Cuando la variable ``foo`` sale
del alcance, el cierre se destruirá. Por lo general, la función interna
será el valor de retorno de la función externa, por ejemplo:

.. code:: berry

   def func(i) # La función externa
       return def () # Devuelve un cierre (función anónima)
           print(i)
           i = i + 1
       end
   end

El cierre devuelto aquí es una función anónima. Cuando la función
externa devuelve el cierre, las variables locales de la función externa
se destruirán y el cierre no podrá acceder directamente a las variables
en la función externa original. El sistema copiará el valor de la
variable libre al entorno cuando se destruya la variable libre. El ciclo
de vida de estas variables libres es el mismo que el del cierre, y solo
el cierre puede acceder a ellas. La función o el cierre devuelto no se
ejecutará automáticamente, por lo que debemos llamar al cierre devuelto
por la función ``func``:

.. code:: berry

   f = func(0)
   f()

Este código generará ``0``. Si continuamos llamando al cierre ``f``,
obtendremos la salida ``1``, ``2``, ``3``\ … Esto puede no entenderse
bien: la variable [2.198 ] se destruye después de que la función
``func`` regresa , y como la variable libre del cierre ``f``, ``i`` se
almacenará en el entorno de cierre, por lo que cada vez que se llame a
``f``, el valor de ``i`` se sumará a 1 (definición de la función
``func`` línea 4).

Uso de cierres
^^^^^^^^^^^^^^

Los cierres tienen muchos usos. Aquí hay algunos usos comunes:

Evaluación perezosa
'''''''''''''''''''

El cierre no hace nada hasta que se llama.

Función de comunicación privada
'''''''''''''''''''''''''''''''

Puede permitir que algunos cierres compartan variables libres, que solo
son visibles para estos cierres, y se comuniquen entre funciones
cambiando los valores de estas variables libres. Esto puede evitar el
uso de variables externas.

Generar múltiples funciones
'''''''''''''''''''''''''''

A veces es posible que necesitemos usar múltiples funciones, estas
funciones pueden tener solo diferentes valores de algunas variables.
Podemos implementar una función y luego usar estas diferentes variables
como parámetros de función. Una mejor manera es devolver el cierre a
través de una función de fábrica y usar estas variables posiblemente
diferentes como variables libres del cierre, de modo que no siempre
tenga que escribir esos parámetros al llamar a la función, y cualquier
número de funciones similares puede ser generado.

Simular miembros privados
'''''''''''''''''''''''''

Algunos lenguajes admiten el uso de miembros privados en objetos, pero
la clase de Berry no lo admite. Podemos usar las variables libres de los
cierres para simular miembros privados. Este uso no es la intención
original de diseñar cierres, pero hoy en día, este “mal uso” de los
cierres es muy común.

Resultado de caché
''''''''''''''''''

Si hay una función que requiere mucho tiempo para ejecutarse, llevará
mucho tiempo llamarla cada vez. Podemos almacenar en caché el resultado
de esta función, buscarlo en el caché antes de llamar a la función y
devolver el valor almacenado en caché si lo encuentra; de lo contrario,
se llama a la función y se actualiza el valor almacenado en caché.
Podemos usar los cierres para guardar el valor almacenado en caché para
que no quede expuesto al alcance externo, y el resultado almacenado en
caché se conservará (hasta que se destruya el cierre).

Vinculación de variables libres
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Si varios cierres vinculan la misma variable libre, todos los cierres
siempre compartirán esta variable libre. Por ejemplo:

.. code:: berry

   def func(i) # La función externa
       return [# Devuelve la lista de cierre
           def () # El cierre #1
               print("cierre 1 log:", i)
               i = i + 1
           end,
           def () # El cierre #2
               print("cierre 2 log:", i)
               i = i + 1
           end
       ]
   end

La función ``func``, en este ejemplo, devuelve dos cierres a través de
una lista, y estos dos cierres comparten la variable libres ``i``. Si
llamamos a estos cierres:

.. code:: berry

   f = func(0)
   f[0]() # cierre 1 log: 0
   f[1]() # cierre 2 log: 1

Como puede ver, actualizamos la variable libre ``i`` cuando llamamos al
cierre ``f[0]``, y este cambio afectó el resultado de llamar al cierre
``f[1]``. Esto se debe a que si varios cierres utilizan una variable
libre, solo hay una copia de la variable libre y todos los cierres
tienen una referencia a la entidad de variable libre. Por lo tanto,
cualquier modificación a la variable libre es visible para todos los
cierres que usan dicha variable.

De manera similar, antes de que se destruyan las variables locales de la
función externa, modificar el valor de la variable libre también
afectará el cierre:

.. code:: berry

   def func()
       i = 0
       def foo()
           print(i)
       end
       i = 1
       return foo
   end

En este ejemplo cambiamos el valor de la variable ``i`` (que es la
variable libre del cierre ``foo``) de ``0`` a ``1`` antes de que regrese
la función externa ``func``, luego llamamos al cierre, y después el
valor de la variable libre ``i`` cuando el paquete ``foo`` también es
``1``:

.. code:: berry

   func()() # 1

Crear cierre en bucle
~~~~~~~~~~~~~~~~~~~~~

Al construir un cierre en el cuerpo del ciclo, es posible que no desee
que las variables libres del cierre cambien con las variables del ciclo.
Primero veamos un ejemplo de cómo crear un cierre en un bucle ``while``:

.. code:: berry

   def func()
       l = [] i = 0
       while i <= 2
           l.push(def () print(i) end)
           i = i + 1
       end
       return l
   end

En este ejemplo, construimos un cierre en un ciclo y colocamos este
cierre en una ``lista``. Obviamente, cuando finalice el ciclo, el valor
de la variable ``i`` será ``3``, y todos los cierres de la lista ``l``
también son referencias usando esta variable. Si ejecutamos el cierre
devuelto por ``func`` obtendremos el mismo resultado:

.. code:: berry

   res = func()
   res[0]() # 3
   res[1]() # 3
   res[2]() # 3

Si queremos que cada cierre se refiera a diferentes variables libres,
podemos definir otra capa de funciones y luego vincular las variables
del ciclo actual con los parámetros de la función:

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

Para ayudar a entender este código aparentemente incomprensible, nos
enfocaremos en el código de las líneas 4 a 6:

.. code:: berry

   def (n)
       return def ()
           print(n)
       end
   end (i)

Aquí realmente se define una función anónima y se llama inmediatamente.
La función de esta función anónima temporal es vincular el valor de la
variable de bucle ``i`` a su parámetro ``n``, y la variable ``n``
también es lo que necesitamos para cerrar las variables libres del
paquete, de modo que las las variables vinculadas al cierre construido
durante cada ciclo son diferentes. Ahora obtendremos la salida deseada:

.. code:: berry

   res = func()
   res[0]() # 0
   res[1]() # 1
   res[2]() # 2

Hay algunas formas de resolver el problema de las variables de bucle
como variables libres. Una forma un poco más simple es definir una
variable temporal en el cuerpo del bucle:

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

Aquí ``temp`` es una variable temporal. El alcance de esta variable está
en el cuerpo del ciclo, por lo que se redefinirá cada vez que se realice
un ciclo. También podemos usar la instrucción ``for`` para resolver el
problema:

.. code:: berry

   def func()
       l = []
       for i: 0 .. 2
           l.push(def () print(i) end)
       end
       return l
   end

Esta puede ser la forma más sencilla de\ ``for``. La variable de
iteración de la instrucción se creará en cada ciclo. El principio es
similar al método anterior.

Expresión lambda
----------------

La **Expresión lambda** es una función anónima especial. La expresión
lambda se compone de una lista de parámetros y un cuerpo de función,
pero la forma es diferente de la función general:

.. code::

   ´/´ args ´->´ expr ´end’

**args** es la lista de parámetros, la cantidad de parámetros puede ser
cero o más, y los parámetros múltiples están separados por comas o
espacios (no se pueden mezclar al mismo tiempo); **expr** es la
expresión de retorno, la expresión lambda devolverá el valor de la
expresión. Las expresiones lambda son adecuadas para implementar
funciones muy simples. Por ejemplo, la expresión lambda para juzgar el
tamaño de dos números es:

.. code:: berry

   / a b -> a < b

Esto es más fácil que escribir una función con la misma funcionalidad.
En algunos algoritmos generales de clasificación, este tipo de función
de comparación de tamaño puede necesitar un uso extensivo. El uso de
expresiones lambda puede simplificar el código y mejorar la legibilidad.

Al igual que las funciones generales, las expresiones lambda pueden
formar cierres. Las expresiones lambda se llaman de la misma manera que
las funciones ordinarias. Si usa el método de llamada inmediata similar
a las funciones anónimas:

.. code:: berry

   lambda = / a b -> a < b
   result = lambda(1, 2) #  llamada normal
   result = (/ a b -> a < b)(1, 2) #  llamada directa

Dado que el operador de llamada de función tiene una prioridad más alta,
se debe agregar un par de paréntesis a la expresión lambda cuando se
realiza una llamada directa, para que se llame como un todo.
