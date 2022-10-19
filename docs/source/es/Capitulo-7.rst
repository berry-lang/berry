.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

7. Bibliotecas y Módulos
========================

7.1 Biblioteca básica
---------------------

Hay algunas funciones y clases que se pueden usar directamente en la
biblioteca estándar. Proporcionan servicios básicos para los programas
de Berry, por lo que también se denominan bibliotecas básicas. Las
funciones y clases de la biblioteca básica están visibles en el ámbito
global (perteneciente al ámbito integrado), por lo que se pueden
utilizar en cualquier lugar. No defina variables con el mismo nombre que
las funciones o clases en la biblioteca base. Si lo hace, será imposible
hacer referencia a las funciones y clases en la biblioteca base.

7.1.1 Función integrada
~~~~~~~~~~~~~~~~~~~~~~~

Función ``print``
^^^^^^^^^^^^^^^^^

**Ejemplo**

.. code:: python

   print(...)

**Descripción**

Esta función imprime los parámetros de entrada en el dispositivo de
salida estándar. La función puede aceptar cualquier tipo y cualquier
número de parámetros. Todos los tipos imprimirán su valor directamente,
y para una instancia, esta función verificará si la instancia tiene un
método ``tostring()``, y si lo hay, imprimirá el valor de retorno de la
instancia llamando al método ``tostring()``, de lo contrario, imprimirá
la dirección de la instancia.

.. code:: python

   print('Hola mundo!') # Hola mundo!
   print([1, 2, '3']) # [1, 2, '3']
   print(print) # <function: 0x561092293780>

Función ``input``
^^^^^^^^^^^^^^^^^

**Ejemplo**

.. code:: python

   input()
   input(prompt)

**Descripción**

La función ``input`` se usa para ingresar una línea de cadena de
caracteres desde el dispositivo de entrada estándar. Esta función puede
usar el parámetro ``prompt`` como un indicador de entrada, y el
parámetro ``prompt`` debe ser de tipo cadena. Después de llamar a la
función ``input``, los caracteres se leerán desde el búfer del teclado
hasta que se encuentre un carácter de nueva línea.

.. code:: python

   input('por favor ingrese una cadena:') # por favor ingrese una cadena:

La función ``input`` no regresa hasta que se presiona la tecla “Enter”,
por lo que el programa queda “atascado” y no es un error.

Función ``type``
^^^^^^^^^^^^^^^^

**Ejemplo**

.. code:: python

   type(valor)

-  *valor*: parámetro de entrada (se espera obtener su tipo).

-  *valor devuelto*: una cadena que describe el tipo de parámetro.

**Descripción**

Esta función recibe un parámetro de cualquier tipo y devuelve el tipo
del parámetro. El valor devuelto es una cadena que describe el tipo del
parámetro. La siguiente tabla muestra los valores de retorno
correspondientes a los principales tipos de parámetros:

================= ============== ================= ==============
Tipo de parámetro Valor devuelto Tipo de parámetro Valor devuelto
================= ============== ================= ==============
Nil               ``’nil’``      Integer           ``’int’``
Real              ``’real’``     Boolean           ``’bool’``
Function          ``’function’`` Class             ``’class’``
String            ``’string’``   Instance          ``’instance’``
puntero nativo    ``’ptr’``                        
================= ============== ================= ==============

.. code:: python

   type(0) # 'int'
   type(0.5) # 'real'
   type('hello') # 'string'
   type(print) # 'función'

Función ``classname``
^^^^^^^^^^^^^^^^^^^^^

**Ejemplo**

.. code:: python

   classname(objeto)

**Descripción**

Esta función devuelve el nombre de clase (cadena) del parámetro. Por lo
tanto, el parámetro debe ser una clase o instancia, y otros tipos de
parámetros devolverán ``nil``.

.. code:: python

   classname(list) # 'list'
   classname(list()) # 'list'
   classname({}) # 'map'
   classname(0) # nil

Función ``classof``
^^^^^^^^^^^^^^^^^^^

**Ejemplo**

.. code:: python

   classof(objeto)

**Descripción**

Devuelve la clase de un objeto de instancia. El parámetro ``objeto``
debe ser una instancia. Si la función se llama con éxito, devolverá la
clase a la que pertenece la instancia; de lo contrario, devolverá
``nil``.

.. code:: python

   classof(list) # nil
   classof(list()) # <class: list>
   classof({}) # <class: map>
   classof(0) # nil

Función ``str``
^^^^^^^^^^^^^^^

**Ejemplo**

.. code:: python

   str(valor)

**Descripción**

Esta función convierte los parámetros en cadenas y los devuelve. Las
funciones ``str`` pueden aceptar cualquier tipo de parámetros y
convertirlos. Cuando el tipo de parámetro es una instancia, verificará
si la instancia tiene un método ``tostring()``, si lo hay, se usará el
valor de retorno del método; de lo contrario, la dirección de la
instancia se convertirá en una cadena.

.. code:: python

   str(0) # '0'
   str(nil) # 'nil'
   str(list) # 'list'
   str([0, 1, 2]) # '[0, 1, 2]'

Función ``number``
''''''''''''''''''

.. code:: python

   number(valor)

**Descripción**

Esta función convierte la cadena o el número de entrada en un tipo
numérico y lo devuelve. Si el parámetro de entrada es un número entero o
real, devuelve directamente. Si es una cadena de caracteres, intenta
convertir la cadena de caracteres en un valor numérico en formato
decimal. El número entero o real se juzgará automáticamente durante la
conversión. Otros tipos devuelven ``nil``.

**Ejemplo**

.. code:: python

   number(5) # 5
   number('45.6') # 45.6
   number('50') # 50
   number(list) # nil

Función ``int``
'''''''''''''''

.. code:: python

   int(valor)

**Descripción**

Esta función convierte la cadena o el número de entrada en un número
entero y lo devuelve. Si el parámetro de entrada es un número entero,
regresa directamente, si es un número real, descarta la parte decimal.
Si es una cadena, intenta convertir la cadena en un número entero en
decimal. Otros tipos devuelven ``nil``. Cuando el tipo de parámetro es
una instancia, verificará si la instancia tiene un método ``toint()``,
si lo hay, se utilizará el valor de retorno del método.

**Ejemplo**

.. code:: python

   int(5) # 5
   int(45.6) # 45
   int('50') # 50
   int('0x10) # 16 - literal hexadecimal es válido
   int(list) # nil

Función ``real``
''''''''''''''''

.. code:: python

   real(valor)

**Descripción**

Esta función convierte la cadena o el número de entrada en un número
real y lo devuelve. Si el parámetro de entrada es un número real,
devolverá directamente, si es un número entero, se convertirá en un
número real. Si es una cadena, intenta convertir la cadena en un número
real en decimal. Otros tipos devuelven ``nil``.

**Ejemplo**

.. code:: python

   real(5) # 5, type(real(5)) →'real'
   real(45.6) # 45.6
   real('50.5') # 50.5
   real(list) # nil

Función ``bool``
''''''''''''''''

.. code:: python

   bool(valor)

**Descripción**

Esta función convierte la cadena o el número de entrada en un valor
booleano y lo devuelve.

La conversión sigue las siguientes reglas:

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
-  Todos los demás tipos: convierte a ``verdadero``.

**Ejemplo**

.. code:: python

   bool() # false
   bool(nil) # false
   bool(false) # false
   bool(true) # true
   bool(0) # false
   bool(1) # true
   bool("") # false
   bool("a") # true
   bool(3.5) # true
   bool(list) # true
   bool([]) # true
   bool({}) # true
   # avanzado
   import introspect
   bool(introspect.toptr(0)) # false
   bool(introspect.toptr(0x1000)) # true

Función ``size``
''''''''''''''''

.. code:: python

   size(valor)

**Descripción**

Esta función devuelve el tamaño de la cadena de entrada. Si el parámetro
de entrada no es una cadena, se devuelve 0. La longitud de la cadena se
calcula en bytes. Esta función también funciona para instancias de
``list`` y ``map`` y devuelve el número de elementos.

**Ejemplo**

.. code:: python

   size(10) # 0
   size('s') # 1
   size('string') # 6
   size([1,2]) # 2
   size({"a":1}) # 1

Función ``super``
'''''''''''''''''

.. code:: python

   super(objeto)

**Descripción**

Esta función devuelve el objeto principal de la instancia. Cuando crea
una instancia de una clase derivada, también creará una instancia de su
clase base. Se requiere la función ``super`` para acceder a la instancia
de la clase base (es decir, el objeto principal).

Consulte el capítulo 6 sobre el comportamiento mágico de
``super(objeto)`` al llamar a un supermétodo.

**Ejemplo**

.. code:: python

   class mi_lista: lista end
   l = mi_lista() # classname(l) -->'mi_lista'
   sl = super(l) # classname(sl) -->'lista'

Función ``assert``
''''''''''''''''''

.. code:: python

   assert(expresión)
   assert(expresión, mensaje)

**Descripción**

Esta función se utiliza para implementar la función de aserción. La
función ``assert`` acepta un parámetro. Cuando el valor del parámetro es
``false`` o ``nil``, la función activará un error de aserción; de lo
contrario, la función no tendrá ningún efecto. Cabe señalar que incluso
si el parámetro es un valor equivalente a ``false`` en operaciones
lógicas (por ejemplo, ``0``), no generará un error de aserción. El
parámetro ``mensaje`` es opcional y debe ser una cadena. Si se utiliza
este parámetro, la información de texto proporcionada en ``message`` se
mostrará cuando se produzca un error de aserción; de lo contrario, se
mostrará el mensaje predeterminado “``Assert Fail``”.

**Ejemplo**

.. code:: python

   assert(false) # aserción fallida!
   assert(nil) # aserción fallida!
   assert() # aserción fallida!
   assert(0) # aserción fallida!
   assert(false,'mensaje de aserción del usuario.') # mensaje de aserción.
   assert(true) # pasa

Función ``compile``
'''''''''''''''''''

.. code:: python

   compile(cadena)
   compile(cadena, 'string')
   compile(nombre_archivo, 'file')

**Descripción**

Esta función compila el código fuente de Berry en una función. El código
fuente puede ser una cadena o un archivo de texto. El primer parámetro
de la función ``compile`` es una cadena, y el segundo parámetro es una
``'cadena'`` o ``'archivo'``. Cuando el segundo parámetro es
``'cadena'`` o no hay un segundo parámetro, la función ``compile``
compilará el primer parámetro como código fuente. Cuando el segundo
parámetro es ``'file'``, la función ``compile`` compilará el archivo
correspondiente al primer parámetro. Si la compilación es exitosa,
``compile`` devolverá la función compilada; de lo contrario, devolverá
``nil``.

**Ejemplo**

.. code:: python

   compile('print(\'Hola mundo!\')')() # Hola mundo!
   compile('test.be','file')

Clase ``list``
~~~~~~~~~~~~~~

``list`` es un tipo incorporado, y define un contenedor de
almacenamiento secuencial que admite la lectura y escritura de
subíndices. Es similar a las matrices en otros lenguajes de
programación. La obtención de una instancia de la clase ``list`` se
puede construir usando un par de corchetes: ``[]`` generará una
instancia vacía de ``list``, y ``[expr, expr, ...]`` generará una
``list`` ejemplo con varios elementos. También se puede instanciar
llamando a la clase ``list``: ejecutar ``list()`` obtendrá una instancia
vacía de ``list``, y ``list(expr, expr, ...)`` devolverá una instancia
con varios elementos.

Método ``list`` (Constructor)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Inicializa el contenedor ``list``. Este método puede aceptar de 0 a
múltiples parámetros. La instancia ``list`` generada cuando se pasan
múltiples parámetros tendrá estos parámetros como elementos, y el orden
de disposición de los elementos es coherente con el orden de disposición
de los parámetros.

Método ``tostring``
^^^^^^^^^^^^^^^^^^^

Serializa la instancia de ``list`` en una cadena y la devuelve. Por
ejemplo, el resultado de ejecutar ``[1, [], 1.5].tostring()`` es
``'[1, [], 1.5]''. Si el contenedor``\ list\` se refiere a sí mismo, la
posición correspondiente utilizará puntos suspensivos en lugar del valor
específico:

.. code:: python

   l = [1, 2]
   l[0] = l
   print(l) # [[...], 2]

Método ``concat``
^^^^^^^^^^^^^^^^^

Convierte cada elemento de la lista en cadenas y la concatena usando la
cadena proporcionada.

.. code:: python

   l = [1, 2, 3]
   l.concat()  # '123'
   l.concat(", ")  # '1, 2, 3'

Método ``push``
^^^^^^^^^^^^^^^

Agrega un elemento al final del contenedor ``list``. El prototipo de
este método es ``push(valor)``, el parámetro ``valor`` es el valor que
se agregará, y el valor agregado se almacena al final del contenedor
``list``. La operación de agregar aumenta el número de elementos en el
contenedor ``list`` en 1. Puede agregar cualquier tipo de valor a la
instancia de ``lista``.

Método ``insert``
^^^^^^^^^^^^^^^^^

Inserta un elemento en la posición especificada del contenedor ``list``.
El prototipo de este método es ``insert(indice, valor)``, el parámetro
``indice`` es la posición a insertar, y ``valor`` es el valor a
insertar. Después de insertar un elemento en la posición ``indice``,
todos los elementos que originalmente comenzaron desde esta posición se
moverán hacia atrás un elemento. La operación de inserción aumenta el
número de elementos en el contenedor ``list`` en 1. Puede insertar
cualquier tipo de valor en el contenedor ``list``.

Supongamos que el valor de una instancia de ``lista`` ``l`` es
``[0, 1, 2]``, e insertamos una cadena ``'cadena'`` en la posición 1, y
necesitamos llamar a ``l.insert(1, 'cadena')``. Finalmente, el nuevo
valor de ``lista`` es ``[0, 'cadena', 1, 2]``.

Si el número de elementos en un contenedor ``list`` es *S*, el rango de
valores de la posición de inserción es {*i* ∈ ℤ : − *S* ≤ *i* < *S*}.
Cuando la posición de inserción es positiva, indexa hacia atrás desde el
principio del contenedor ``list``; de lo contrario, indexa hacia
adelante desde el final del contenedor ``list``.

Método ``remove``
^^^^^^^^^^^^^^^^^

Quita un elemento del contenedor. El prototipo de este método es
``remove(indice)``, y el parámetro ``indice`` es la posición del
elemento a eliminar. Después de eliminar el elemento, el elemento detrás
del elemento eliminado avanzará un elemento y la cantidad de elementos
en el contenedor se reducirá en 1. Al igual que el método ``insert``, el
método ``remove`` también puede usar índices positivos o negativos.

Método ``item``
^^^^^^^^^^^^^^^

Obtiene un elemento en el contenedor ``list``. El prototipo de este
método es ``item(indice)``, el parámetro ``indice`` es el índice del
elemento a obtener, y el valor de retorno del método es el elemento en
la posición del índice. ``list`` El contenedor admite múltiples métodos
de indexación:

-  Índice entero: El valor del índice puede ser un número entero
   positivo o un entero negativo. Si el índice es negativo, es relativo
   al final de la lista; es decir, ``-1`` indica el último elemento de
   la lista. El valor de retorno de ``item`` es el elemento en la
   posición del índice. Si la posición del índice excede el número de
   elementos en el contenedor o está antes del elemento 0, el método
   ``item`` devuelve ``nil``.

-  Índice ``list``: Utilizando una lista de enteros como índice,
   ``item`` devuelve una ``lista``, y cada elemento en el valor devuelto
   ``lista`` es un elemento correspondiente a cada índice entero en el
   parámetro ``lista``. los el valor de la expresión
   ``[3, 2, 1].item([0, 2])`` es ``[3, 1]``. Si una tipo de elemento en
   el parámetro ``lista`` no es un número entero, entonces el el valor
   en esa posición en el valor de retorno ``lista`` es ``nil``.

-  Índice ``range``: Usando un rango de enteros como índice, ``item``
   devuelve una ``lista``. El valor devuelto almacena los elementos
   indexados de la ``lista`` desde el límite inferior hasta el límite
   superior del parámetro ``range``. Si el índice excede el rango de
   índice de la ‘lista’ indexada, el retorno value ``list`` usará
   ``nil`` para llenar la posición más allá del índice.

Método ``setitem``
^^^^^^^^^^^^^^^^^^

Establece el valor de la posición especificada en el contenedor. El
prototipo de este método es ``setitem(indice, valor)``, ``indice`` es la
posición del elemento a escribir y ``valor`` es el valor a escribir.
``indice`` es el valor de índice entero de la posición de escritura. Las
posiciones de índice fuera del rango de índice del contenedor harán que
``setitem`` no se ejecute.

Método ``size``
^^^^^^^^^^^^^^^

Devuelve el número de elementos en el contenedor, que es la longitud del
contenedor. El prototipo de este método es ``size()``.

Método ``resize``
^^^^^^^^^^^^^^^^^

Restablece la ``lista`` a la longitud del contenedor. El prototipo de
este método es ``resize(count)``, y el parámetro ``count`` es la nueva
longitud del contenedor. Al usar ``resize`` para aumentar la longitud
del contenedor, el nuevo elemento se inicializará en ``nil``. El uso de
``resize`` para reducir la longitud del contenedor descartará algunos
elementos al final del contenedor. P.ej:

.. code:: python

   l = [1, 2, 3]
   l.resize(5) # Expansion, l == [1, 2, 3, nil, nil]
   l.resize(2) # Reduce, l == [1, 2]

Método ``iter``
^^^^^^^^^^^^^^^

Devuelve un iterador para recorrer el contenedor ``list`` actual.

Método ``find``
^^^^^^^^^^^^^^^

Similar a ``item`` o ``list[idx]``. La única diferencia es que si el
índice está fuera de rango, ``find`` devuelve ``nil`` en su lugar o
genera una excepción.

Método ``reverse``
^^^^^^^^^^^^^^^^^^

Cambia la lista en el lugar e invierte el orden de los elementos.
También devuelve la lista resultante.

Clase ``map``
~~~~~~~~~~~~~

La clase ``map`` es un tipo de clase incorporado que se utiliza para
proporcionar un contenedor desordenado de pares clave-valor. Dentro del
intérprete de Berry, ``map`` usa la tabla Hash para su implementación.
Puede utilizar pares de llaves para construir un contenedor ``map``. El
uso de un par de llaves vacías ``{}`` generará una instancia de ``map``
vacía. Si necesita construir una instancia de ``map`` que no esté vacía,
use dos puntos para separar la clave y el valor, y use un punto y coma
para separar varios pares clave-valor. Por ejemplo, ``{0: 1, 2: 3}``
tiene dos pares clave-valor (0,1) y (2,3). También puede obtener una
instancia de ``map`` vacía llamando a la clase ``map``.

Método ``map`` (Constructor)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Inicializa el contenedor ``map``, este método no acepta parámetros.
Ejecutar ``map()`` obtendrá una instancia de ``map`` vacía.

.. _método-tostring-1:

Método ``tostring``
^^^^^^^^^^^^^^^^^^^

Serializa ``map`` como una cadena y regresa. La cadena serializada es
similar a la escritura literal. Por ejemplo, el resultado de ejecutar
``'str': 1, 0: 2`` es ``"'str': 1, 0: 2"``. Si el contenedor ``map`` se
refiere a sí mismo, la posición correspondiente utilizará puntos
suspensivos en lugar del valor específico:

.. code:: python

   m = {'map': nil,'texto':'hola'}
   m['map'] = m
   print(m) # {'texto':'hola','map': {...}}

.. _método-insert-1:

Método ``insert``
^^^^^^^^^^^^^^^^^

Inserta un par clave-valor en el contenedor ``map``. El prototipo de
este método es ``insert(llave, valor)``, el parámetro ``llave`` es la
clave a insertar, y ``valor`` es el valor a insertar. Si el ``map``
clave que se va a insertar existe en el contenedor, se actualizará el
par clave-valor original.

.. _método-remove-1:

Método ``remove``
^^^^^^^^^^^^^^^^^

Elimina un par clave-valor del contenedor ``map``. El prototipo de este
método es ``remove(llave)``, y el parámetro ``llave`` es la clave del
par clave-valor que se eliminará.

.. _método-item-1:

Método ``item``
^^^^^^^^^^^^^^^

Obtiene un valor en el contenedor ``map``. El prototipo de este método
es ``item(llave)``, el parámetro ``llave`` es la clave del valor a
obtener, y el valor de retorno del método es el valor correspondiente a
la clave.

.. _método-setitem-1:

Método ``setitem``
^^^^^^^^^^^^^^^^^^

Establece el valor correspondiente a la clave especificada en el
contenedor. El prototipo de este método es ``setitem(clave, valor)``,
``clave`` es la clave del par clave-valor a escribir, y ``valor`` es el
valor a escribir. Si no hay un par clave-valor con la clave ``clave`` en
el contenedor, el método ``setitem`` fallará.

.. _método-size-1:

Método ``size``
^^^^^^^^^^^^^^^

Devuelve el número de pares clave-valor del contenedor ``map``, que es
la longitud del contenedor. El prototipo de este método es ``size()``.

Método ``contains``
^^^^^^^^^^^^^^^^^^^

Devuelve ``true`` booleano si se encuentra un par clave-valor
coincidente en el contenedor ``map``; de lo contrario, ``false``. El
prototipo de este método es ``contains(llave)``.

.. _método-find-1:

Método ``find``
^^^^^^^^^^^^^^^

Devuelve el valor correspondiente a la clave especificada en el
contenedor. El prototipo de este método es ``find(llave)`` o
``find(llave, valor_defecto)``, ``llave`` es la clave del par
clave-valor al que se accederá, y ``valor_defecto`` es el valor
predeterminado devuelto si la clave no se encuentra. Si no se especifica
ningún valor predeterminado, se devuelve ``nil`` en su lugar.

Clase ``range``
~~~~~~~~~~~~~~~

La clase se usa para representar un intervalo cerrado entero. Utilice el
operador binario ``..`` para construir una instancia de ``range``. Los
operandos izquierdo y derecho del operador deben ser números enteros.
Por ejemplo, ``0..10`` significa el intervalo entero [0,10] ∩ ℤ.

Si no especifica el rango alto, se establece en ``MAXINT``. Ejemplo:
``imprimir(0..) # (0..9223372036854775807)``

Por lo general, hay dos formas de recorrer una lista:

.. code:: python

   l = [1,2,3,4]
   for e:l print(e) end # 1/2/3/4
   for i:0..size(l)-1 print(l[i]) end # 1/2/3/4

Clase ``bytes``
~~~~~~~~~~~~~~~

Los objetos ``bytes`` se representan como matrices de bytes
hexadecimales. El constructor ``bytes`` toma una cadena de Hex y
construye el búfer en memoria.

Ejemplo:

.. code:: python

   b = bytes()
   print(b)   # bytes('')
   b = bytes("1155AA")  # secuencia de bytes 0x11 0x55 0xAA
   size(b)  # 3 = 3 bytes
   b[0]     # 17 (0x11)
   b[0] = 16  #  asigna el primer byte
   print(b)   # bytes('1055AA')

Método ``bytes`` (Constructor)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Inicializar una matriz de bytes. Hay varias opciones.

**Opción 1: valor vacío**

``bytes()`` crea una nueva matriz de bytes vacía.
``tamaño(bytes()) == 0``.

No hay límite en el tamaño de una matriz de bytes, excepto la memoria
disponible. Se asigna un búfer interno y se reasigna en caso de que el
anterior fuera demasiado pequeño. El búfer inicial es de 36 bytes, pero
puede preasignar un búfer más grande (o más pequeño) si sabe de antemano
el tamaño necesario.

De manera similar, el búfer se reduce automáticamente si se usa menos
del tamaño necesario.

.. code:: python

   b = bytes(4096)   # 4096 bytes preasignados

**Opción 2: valor inicial**

Si el primer argumento es una “cadena”, se analiza como una lista de
valores hexadecimales. Puede agregar un segundo argumento opcional para
preasignar un búfer más grande.

.. code:: python

   b = bytes("BEEF0000")
   print(b)   # bytes('beef0000')
   b = bytes("112233", 128)   #  preasignar 128 bytes internamente
   print(b)   # bytes('112233')

**Opción 3: tamaño fijo**

Si el tamaño proporcionado es negativo, el tamaño de la matriz es fijo y
no se puede reducir ni aumentar.

.. code:: python

   b = bytes(-8)
   print(b)   # bytes('0000000000000000')

   b = bytes("AA", -4)
   print(b)   # bytes('AA000000')

   b = bytes("1122334455", -4)
   atributo_error: tamaño del objeto en bytes es fijo y no se puede cambiar el tamaño

**Opción 4: asignación de memoria**

**Precaución, use con mucho cuidado**

En este modo, la matriz de bytes se asigna a una región específica de la
memoria. Debe proporcionar la dirección base como ``comptr`` y el
tamaño. El tamaño siempre se fija, ya sea positivo o negativo. Esta
función es **peligrosa** ya que puede acceder a cualquier ubicación de
la memoria, lo que provoca un bloqueo si la ubicación está protegida o
no es válida. Usar con cuidado.

En este caso, ``b.ismapped()`` devuelve ``true`` indicando un búfer de
memoria mapeado. En todos los demás casos, ``b.ismapped()`` devuelve
``falso``. Esto se usa típicamente para saber si Berry asignó el búfer o
no, y si los subelementos deben desasignarse explícitamente.

Ejemplo:

.. code:: python

   import introspect
   def f() return 0 end

   addr = introspect.toptr(f)
   print(addr)   # <ptr: 0x3ffeaf88>

   b = bytes(addr, 8)
   print(b)   # bytes('F8EAFE3F24000000')
   #  este ejemplo muestra los primeros 8 bytes del objeto de función en la memoria

.. _método-size-2:

Método ``size``
^^^^^^^^^^^^^^^

Devuelve el número de bytes en la matriz de bytes

.. code:: python

   b = bytes("1122334455")
   b.size()   # 5
   size(b)    # 5

.. _método-tostring-2:

Método ``tostring``
^^^^^^^^^^^^^^^^^^^

Muestra una forma legible por humanos la matriz de bytes en hexadecimal.
Por defecto, muestra solo los primeros 32 caracteres. Puede solicitar
más caracteres agregando un argumento int con la cantidad máxima de
bytes que desea convertir. ``tostring`` se usa internamente cuando se
imprime un objeto. ``print(b)`` es equivalente a
``print(b.tostring())``. Es diferente de ``asstring``, que convierte una
matriz de bytes en el objeto de cadena de bajo nivel equivalente sin
ninguna codificación.

.. code:: python

   b = bytes("1122334455")
   b.tostring()   # 'bytes(\'1122334455\')'

   b = bytes()
   b.resize(64)   # redimenciona a 64 bytes
   b.tostring()   # 'bytes(\'0000000000000000000000000000000000000000000000000000000000000000...\')'
   b.tostring(500) # 'bytes(\'00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000\')'

Método ``tohex``
^^^^^^^^^^^^^^^^

Convierte la matriz de bytes en una cadena hexadecimal, similar a la
devuelta por ``tostring()`` pero sin decoradores.

.. code:: python

   b = bytes("1122334455")
   b.tohex()   # '1122334455'

Método ``fromhex``
^^^^^^^^^^^^^^^^^^

Actualiza el contenido de la matriz de bytes a partir de una nueva
cadena hexadecimal. Esto permite cargar una nueva cadena hexadecimal sin
asignar un nuevo objeto de bytes.

.. code:: python

   b = bytes("1122334455")
   b.fromhex("AABBCC")  # bytes('AABBCC')

Método ``clear``
^^^^^^^^^^^^^^^^

Vuelve a poner la matriz de bytes en vacío

.. code:: python

   b = bytes("1122")
   b.clear()
   print(b)   # bytes()

.. _método-resize-1:

Método ``resize``
^^^^^^^^^^^^^^^^^

Reduce o expande la matriz de bytes para que coincida con el tamaño
especificado. Si se expande, se agregan bytes NULL (0x00) al final del
búfer.

.. code:: python

   b = bytes("11223344")
   b.resize(6)
   print(b)   # bytes('112233440000')
   b.resize(2)
   print(b)   # bytes('1122')

Métodos de concatenación ``+`` y ``..``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Puede usar ``+`` para concatenar dos listas de ``bytes``, creando un
nuevo objeto ``bytes``. ``..`` cambia la lista en su lugar y se puede
usar para agregar un objeto ``int`` (1 byte) o ``bytes``

::

   b = bytes("1122")
   c = bytes("3344")
   d = b + c           # b y c no cambian
   print(d)            # bytes('11223344')
   print(b)            # bytes('1122')
   print(c)            # bytes('3344')

   e = b..c            # ahora b ha cambiado
   print(e)            # bytes('11223344')
   print(b)            # bytes('11223344')
   print(c)            # bytes('3344')

Método de acceso a bytes ``[]``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Puede acceder a bytes individuales como enteros, para leer y escribir.
Los valores que no están en el rango de 0 a 255 se cortan
silenciosamente.

.. code:: python

   b = bytes("010203")
   print(b[0])         # 1

   #  índices negativos cuentan desde el final
   print(b[-1])        # 3

   #  fuera de los límites genera una excepción
   print(b[5])         # index_error: índice de bytes fuera de rango

   b[0] = -1
   print(b)            # bytes('FF0203')

   b[1] = 256
   print(b)            # bytes('FF0003')

Método de acceso de rango ``[]``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Puede usar el descriptor de acceso ``[]`` con un rango para obtener una
sublista de ``bytes``. Si un índice es negativo, se toma del final de la
matriz.

Esta construcción no se puede usar como un *lvalue*, es decir, no se
puede empalmar como ``b[1..2] = bytes("0011") # no permitido``.

.. code:: python

   b = bytes("001122334455")
   print(b[1..2])      # bytes('1122')

   # elimina los primeros 2 bytes
   print(b[2..-1])     # bytes('22334455')

   # eliminar los últimos 2 bytes
   print(b[0..-3])     # bytes('00112233')

   #  se permite el sobreimpulso
   print(b[4..10])     # bytes('4455')

   #  índices invertidos devuelven una matriz vacía
   print(b[5..4])      # bytes('')

Los métodos estándar ``item`` y ``setitem`` se implementan y se asignan
de forma transparente al operador ``[]``.

Método ``copy``
^^^^^^^^^^^^^^^

Crea una nueva copia nueva del objeto ``bytes``. Se asigna un nuevo
búfer de memoria y se duplican los datos.

.. code:: python

   b = bytes("1122")
   print(b)          # bytes('1122')

   c = b.copy()
   print(c)          # bytes('1122')

   b.clear()
   print(b)          # bytes('')
   print(c)          # bytes('1122')bytes('1122')

Métodos ``get, geti``
^^^^^^^^^^^^^^^^^^^^^

Lea un valor de 1/2/4 bytes de cualquier desplazamiento en la matriz de
bytes. El modo estándar es little endian, si se especifica un tamaño
negativo habilita big endian. ``get`` devuelve valores sin signo,
mientras que ``geti`` devuelve valores con signo.

::

   b.get(<offset>, <size>) ->  objeto de bytes

Si el desplazamiento está fuera de rango, se devuelve ``0`` (no se
genera ninguna excepción).

Ejemplo:

.. code:: python

   b = bytes("010203040506")
   print(b.get(2,2))         # 1027 - 0x0403 read 2 bytes little endian
   print(b.get(2,-2))        #  772 - 0x0304 read 2 bytes big endian

   print(b.get(2,4))         # 100992003 - 0x06050403 - little endian
   print(b.get(2,-4))        #  50595078 - 0x03040506 - big endian

   b = bytes("FEFF")
   print(b.get(0, 2))        # 65534 - 0xFFFE
   print(b.geti(0, 2))       # -2 - 0xFFFE

Métodos ``set, seti``
^^^^^^^^^^^^^^^^^^^^^

Similar a ``get`` y ``geti``, permite establecer un valor de 1/2/4 bytes
en cualquier desplazamiento. ``seti`` usa números enteros con signo,
``set`` no tiene signo (en realidad, no hace la diferencia).

Si el desplazamiento está fuera de rango, no se realiza ningún cambio
(no se genera ninguna excepción).

::

   bytes.set(<offset>, <valor>, <tamaño>)

Método ``add``
^^^^^^^^^^^^^^

Este método agrega valor de 1/2/4 bytes (little endian o big endian) al
final del búfer. Si el tamaño es negativo, el valor se trata como big
endian.

::

   b.add(<valor>, <tamaño>)

Ejemplo:

.. code:: python

   b = bytes("0011")
   b.add(0x22, 1)
   print(b)             # bytes('001122')
   b.add(0x2233, 2)
   print(b)             # bytes('0011223322')
   b.add(0x22334455, 4)
   print(b)             # bytes('001122332255443322')
   b.add(0x00)
   print(b)             # bytes('00112233225544332200')
   b.clear()
   b.add(0x0102, -2)
   print(b)             # bytes('0102')
   b.add(0x01020304, -4)
   print(b)             # bytes('010201020304')

Método ``asstring``
^^^^^^^^^^^^^^^^^^^

Convierte un búfer de bytes en una cadena. El búfer se convierte tal
cual sin ninguna consideración de codificación. Si el búfer contiene
caracteres NULL, la cadena se truncará.

.. code:: python

   b=bytes("3344")
   print(b.asstring())   # '3D'

Método ``fromstring``
^^^^^^^^^^^^^^^^^^^^^

Convierte un búfer de bytes en una cadena. El búfer se convierte tal
cual sin ninguna consideración de codificación. Si el búfer contiene
caracteres NULL, la cadena se truncará.

.. code:: python

   b=bytes().fromstring("Hola")
   print(b)              # bytes('48656C6C6F')

Métodos de manipulación de bits ``setbits, getbits``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Puede leer y escribir a nivel de subbytes, especificando de qué bit a
qué bit. El desplazamiento está en bits, no en bytes. Agregue el número
de bytes \* 8.

::

   b.setbits(<offset_bits>, <len_bits>, <value>)
   b.getbits(<offset_bits>, <len_bits>)

Codificación en base64, método ``tob64``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Convierte una matriz de bytes en una cadena base64.

.. code:: python

   b = bytes('deadbeef0011')
   s = b.tob64()
   print(s)               # 3q2+7wAR

Decodificación en base64, método ``fromb64``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Convierte una cadena base64 en una matriz de bytes.

.. code:: python

   s = '3q2+7wAR'
   b = bytes().fromb64(s)
   print(b)               # bytes('DEADBEEF0011')

Métodos ``getfloat`` y ``setfloat``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Similar a ``get/set``, permite leer o escribir un valor en coma flotante
de 32 bits.

::

   b.getfloat(<offset>)
   b.getfloat(<offset>, <number>)

.. code:: berry

   b = bytes("00000000")
   b.getfloat(0)     # 0
   b.setfloat(0, -1.5)
   print(b)  # bytes('0000C0BF')
   b.getfloat(0)  # -1.5

Método ``_buffer``
^^^^^^^^^^^^^^^^^^

**Característica avanzada**: devuelve la dirección del búfer en la
memoria, para usar con código C.

.. code:: berry

   b = bytes('1122')
   b._buffer()  # <ptr: 0x600000c283c0>

Método ``_change_buffer``
^^^^^^^^^^^^^^^^^^^^^^^^^

**Característica avanzada**: funciona solo para búferes mapeados (es
decir, ``b.ismapped() == true``), permite reasignar el búfer a una nueva
dirección de memoria. Esto permite reutilizar el objeto bytes() sin
reasignar una nueva instancia.

.. code:: berry

   #  este ejemplo usa la asignación de punteros, use con mucho cuidado
   b1 = bytes("11223344")
   b2 = bytes("AABBCCDD")
   b1._buffer()  # <ptr: 0x600000c2c390>
   b2._buffer()  # <ptr: 0x600000c24270>

   # ahora creamos c como un búfer asignado de 4 bytes a la dirección de b1
   c = bytes(b1._buffer(), 4)
   print(c)  # bytes('11223344') -- asignado a b1
   c._buffer()   # <ptr: 0x600000c2c390>

   #  cambiemos un byte para probarlo
   c[0] = 254
   print(c)  # bytes('FE223344')
   print(b1) # bytes('FE223344') -- b1 ha cambiado

   #  reasignar c al mapa b2
   c._change_buffer(b2._buffer())
   print(c)  # bytes('AABBCCDD')
   c._buffer()  # <ptr: 0x600000c24270>

Módulos de expansión
--------------------

Módulo JSON
~~~~~~~~~~~

JSON es un formato ligero de intercambio de datos. Es un subconjunto de
JavaScript. Utiliza un formato de texto que es completamente
independiente del lenguaje de programación para representar datos. Berry
proporciona un módulo JSON para proporcionar soporte para datos JSON. El
módulo JSON solo contiene dos funciones, “cargar” y “volcar”, que se
utilizan para analizar cadenas JSON y multiplicar objetos Berry y
serializar un objeto Berry en texto JSON.

Función ``load``
^^^^^^^^^^^^^^^^

.. code:: python

   load(text)

**Descripción**

Esta función se usa para convertir el texto JSON de entrada en un objeto
Berry y devolverlo. Las reglas de conversión se muestran en la Tabla 1.1
. Si hay un error de sintaxis en el texto JSON, la función devolverá
``nil``.

============= =====================
**Tipo JSON** **Tipo Berry**
============= =====================
``nulo``      ``nil``
``número``    ``entero`` o ``real``
``cadena``    ``cadena``
``matriz``    ``lista``
``objeto``    ``mapa``
============= =====================

*Tabla 9: Reglas de conversión de tipo JSON a tipo Berry*

**Ejemplo**

.. code:: python

   import json
   json.load('0') # 0
   json.load('[{"nombre": "liu", "edad": 13}, 10.0]') # [{'nombre':'liu','edad': 13}, 10]

Función ``dump``
^^^^^^^^^^^^^^^^

.. code:: python

   dump(objectp, ['formato'])

**Descripción**

Esta función se usa para serializar el objeto Berry en texto JSON. Las
reglas de conversión para la serialización se muestran en la Tabla 10 .

================= =============
**Tipo berry**    **Tipo JSON**
================= =============
``cero``          ``nulo``
``entero``        ``número``
``verdadero``     ``número``
``lista``         ``matriz``
``mapa``          ``objeto``
``mapa`` Clave de ``cadena``
otro              ``cadena``
================= =============

*Tabla 10: Reglas de conversión de tipo Berry a tipo JSON*

**Ejemplo**

.. code:: python

   import json
   json.dump('string') #'"string"'
   json.dump('string') #'"string"'
   json.dump({0:'item 0','list': [0, 1, 2]}) #'{"0":"item 0","list":[0,1,2]}'
   json.dump({0:'item 0','list': [0, 1, 2],'func': print},'format')
   #-
   {
       "0": "item 0",
       "list": [
           0,
           1,
           2
       ],
       "func": "<function: 00410310>"
   }
   -#

Módulo matemático
~~~~~~~~~~~~~~~~~

Este módulo se utiliza para proporcionar soporte para funciones
matemáticas, como las funciones trigonométricas y las funciones de raíz
cuadrada de uso común. Para usar el módulo matemático, primero use la
instrucción ``import math``. Todos los ejemplos de esta sección asumen
que el módulo se ha importado correctamente.

Constante ``pi``
^^^^^^^^^^^^^^^^

El valor de Pi *π* es un tipo de número real, aproximadamente igual a
3.141592654.

**Ejemplo**

.. code:: python

   math.pi # 3.14159

Función ``abs``
^^^^^^^^^^^^^^^

.. code:: python

   abs(valor)

**Descripción**

Esta función devuelve el valor absoluto del parámetro, que puede ser un
número entero o un número real. Si no hay parámetros, la función
devuelve ``0``, si hay varios parámetros, solo se procesa el primer
parámetro. El tipo de retorno de la función ``abs`` es un número real.

**Ejemplo**

.. code:: python

   math.abs(-1) # 1
   math.abs(1.5) # 1.5

Función ``ceil``
^^^^^^^^^^^^^^^^

.. code:: python

   ceil(valor)

**Descripción**

Esta función devuelve el valor redondeado hacia arriba del parámetro, es
decir, el valor entero más pequeño mayor o igual que el parámetro. El
parámetro puede ser un número entero o un número real. Si no hay
parámetros, la función devuelve ``0``, si hay varios parámetros, solo se
procesa el primero. El tipo de retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.ceil(-1.2) # -1
   math.ceil(1.5) # 2

Función ``floor``
^^^^^^^^^^^^^^^^^

.. code:: python

   floor(valor)

**Descripción**

Esta función devuelve el valor redondeado hacia abajo del parámetro, que
no es mayor que el valor entero máximo del parámetro. El parámetro puede
ser un número entero o un número real. Si no hay parámetros, la función
devuelve ``0``, si hay varios parámetros, solo se procesa el primero. El
tipo de retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.floor(-1.2) # -2
   math.floor(1.5) # 1

Función ``sin``
^^^^^^^^^^^^^^^

.. code:: python

   sin(valor)

**Descripción**

Esta función devuelve el valor de la función seno del parámetro. El
parámetro puede ser un número entero o un número real, y la unidad son
los radianes. Si no hay parámetros, la función devuelve ``0``, si hay
varios parámetros, solo se procesa el primer parámetro. El tipo de
retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.sin(1) # 0.841471
   math.sin(math.pi * 0.5) # 1

Función ``cos``
^^^^^^^^^^^^^^^

.. code:: python

   cos(valor)

**Descripción**

Esta función devuelve el valor de la función coseno del parámetro. El
parámetro puede ser un número entero o un número real en radianes. Si no
hay parámetros, la función devuelve ``0``, si hay varios parámetros,
solo se procesa el primero. El tipo de retorno de la función es un
número real.

**Ejemplo**

.. code:: python

   math.cos(1) # 0.540302
   math.cos(math.pi) # -1

Función ``tan``
^^^^^^^^^^^^^^^

.. code:: python

   tan(valor)

**Descripción**

Esta función devuelve el valor de la función tangente del parámetro. El
parámetro puede ser un número entero o un número real, en radianes. Si
no hay parámetros, la función devuelve ``0``, si hay varios parámetros,
solo se procesa el primero. El tipo de retorno de la función es un
número real.

**Ejemplo**

.. code:: python

   math.tan(1) # 1.55741
   math.tan(math.pi / 4) # 1

Función ``asin``
^^^^^^^^^^^^^^^^

.. code:: python

   asin(valor)

**Descripción**

Esta función devuelve el valor de la función arco seno del parámetro. El
parámetro puede ser un número entero o un número real. El rango de
valores es [−1,1]. Si no hay parámetros, la función devuelve ``0``, si
hay varios parámetros, solo se procesa el primero. El tipo de retorno de
la función es un número real y la unidad es radianes.

**Ejemplo**

.. code:: python

   math.asin(1) # 1.5708
   math.asin(0.5) * 180 / math.pi # 30

Función ``acos``
^^^^^^^^^^^^^^^^

.. code:: python

   acos(valor)

**Descripción**

Esta función devuelve el valor de la función de arco coseno del
parámetro. El parámetro puede ser un número entero o un número real. El
rango de valores es [−1,1]. Si no hay parámetros, la función devuelve
``0``, si hay varios parámetros, solo se procesa el primero. El tipo de
retorno de la función es un número real y la unidad es radianes.

**Ejemplo**

.. code:: python

   math.acos(1) # 0
   math.acos(0) # 1.5708

Función ``atán``
^^^^^^^^^^^^^^^^

.. code:: python

   atan(valor)

**Descripción**

Esta función devuelve el valor de la función arco tangente del
parámetro. El parámetro puede ser un número entero o un número real. El
rango de valores es [−∞,+∞]. Si no hay parámetros, la función devuelve
``0``, si hay varios parámetros, solo se procesa el primero. El tipo de
retorno de la función es un número real y la unidad es radianes.

**Ejemplo**

.. code:: python

   math.atan(1) * 180 / math.pi # 45

Función ``sinh``
^^^^^^^^^^^^^^^^

.. code:: python

   sinh(valor)

**Descripción**

Esta función devuelve el valor de función de seno hiperbólico del
parámetro. Si no hay parámetros, la función devuelve ``0``, si hay
varios parámetros, solo se procesa el primer parámetro. El tipo de
retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.sinh(1) # 1.1752

Función ``cosh``
^^^^^^^^^^^^^^^^

.. code:: python

   cosh(valor)

**Descripción**

Esta función devuelve el valor de la función coseno hiperbólico del
parámetro. Si no hay parámetros, la función devuelve ``0``, si hay
varios parámetros, solo se procesa el primero. El tipo de retorno de la
función es un número real.

**Ejemplo**

.. code:: python

   math.cosh(1) # 1.54308

Función ``tanh``
^^^^^^^^^^^^^^^^

.. code:: python

   tanh(valor)

**Descripción**

Esta función devuelve el valor de la función tangente hiperbólica del
parámetro. Si no hay parámetros, la función devuelve ``0``, si hay
varios parámetros, solo se procesa el primero. El tipo de retorno de la
función es un número real.

**Ejemplo**

.. code:: python

   math.tanh(1) # 0.761594

Función ``sqrt``
^^^^^^^^^^^^^^^^

.. code:: python

   sqrt(valor)

**Descripción**

Esta función devuelve la raíz cuadrada del argumento. El parámetro de
esta función no puede ser negativo. Si no hay parámetros, la función
devuelve ``0``, si hay varios parámetros, solo se procesa el primero. El
tipo de retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.sqrt(2) # 1.41421

Función ``exp``
^^^^^^^^^^^^^^^

.. code:: python

   exp(valor)

**Descripción**

Esta función devuelve el valor de la función exponencial del parámetro
en función de la constante natural *e*. Si no hay parámetros, la función
devuelve ``0``, si hay varios parámetros, solo se procesa el primero. El
tipo de retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.exp(1) # 2.71828

Función ``log``
^^^^^^^^^^^^^^^

.. code:: python

   log(valor)

**Descripción**

Esta función devuelve el logaritmo natural del argumento. El parámetro
debe ser un número positivo. Si no hay parámetros, la función devuelve
``0``, si hay varios parámetros, solo se procesa el primero. El tipo de
retorno de la función es un número real.

**Ejemplo**

.. code:: python

   log(valor)

Función ``log10``
^^^^^^^^^^^^^^^^^

.. code:: python

   log10(valor)

**Descripción**

Esta función devuelve el logaritmo del parámetro en base 10. El
parámetro debe ser un número positivo. Si no hay parámetros, la función
devuelve ``0``, si hay varios parámetros, solo se procesa el primero. El
tipo de retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.log10(10) # 1

Función ``deg``
^^^^^^^^^^^^^^^

.. code:: python

   deg(valor)

**Descripción**

Esta función se utiliza para convertir radianes en ángulos. La unidad
del parámetro es radianes. Si no hay parámetros, la función devuelve
``0``, si hay varios parámetros, solo se procesa el primero. El tipo de
retorno de la función es un número real y la unidad es un ángulo.

**Ejemplo**

.. code:: python

   math.deg(math.pi) # 180

Función ``rad``
^^^^^^^^^^^^^^^

.. code:: python

   rad(valor)

**Descripción**

Esta función se utiliza para convertir ángulos a radianes. La unidad del
parámetro es el ángulo. Si no hay parámetros, la función devuelve ``0``,
si hay varios parámetros, solo se procesa el primero. El tipo de retorno
de la función es un número real y la unidad es radianes.

**Ejemplo**

.. code:: python

   math.rad(180) # 3.14159

Función ``pow``
^^^^^^^^^^^^^^^

.. code:: python

   pow(x, y)

**Descripción**

El valor de retorno de esta función es el resultado de la expresión *x*
\ *y*\  , que es el parámetro ``x`` elevado a ``y``. Si los parámetros
no están completos, la función devuelve ``0``, si hay parámetros
adicionales, solo se procesan los dos primeros parámetros. El tipo de
retorno de la función es un número real.

**Ejemplo**

.. code:: python

   math.pow(2, 3) # 8

Función ``srand``
^^^^^^^^^^^^^^^^^

.. code:: python

   srand(valor)

**Descripción**

Esta función se utiliza para establecer la semilla del generador de
números aleatorios. El tipo del parámetro debe ser un número entero.

**Ejemplo**

.. code:: python

   math.srand(2)

Función ``rand``
^^^^^^^^^^^^^^^^

.. code:: python

   rand()

**Descripción**

Esta función se utiliza para obtener un número entero aleatorio.

**Ejemplo**

.. code:: python

   math.rand()

Módulo de tiempo
~~~~~~~~~~~~~~~~

Este módulo se utiliza para proporcionar funciones relacionadas con el
tiempo.

Función ``time``
^^^^^^^^^^^^^^^^

.. code:: python

   time()

**Descripción**

Devuelve la marca de tiempo actual. La marca de tiempo es el tiempo
transcurrido desde Unix Epoch (1 de enero de 1970 00:00:00 UTC), en
segundos.

.. _función-dump-1:

Función ``dump``
^^^^^^^^^^^^^^^^

.. code:: python

   dump(ts)

**Descripción**

La marca de tiempo de entrada ``ts`` se convierte en un ``map`` de
tiempo, y la correspondencia clave-valor se muestra en la siguiente
tabla:

+----------+----------+----------+----------+----------+----------+
|**clave** | **valor**| **clave**| **valor**| **clave**| **valor**|
+==========+==========+==========+==========+==========+==========+
| ``       | Año      | ``'      | Mes      | `        | Día      |
| 'year'`` | (desde   | month'`` | (1-12)   | `'day'`` | (1-31)   |
|          | 1900)    |          |          |          |          |
+----------+----------+----------+----------+----------+----------+
| ``       | Hora     | `        | Puntos   | `        | Segundos |
| 'hour'`` | (0-23)   | `'min'`` | (0-59)   | `'sec'`` | (0-59)   |
+----------+----------+----------+----------+----------+----------+
| ``'we    | Semana   |          |          |          |          |
| ekday'`` | (1-7)    |          |          |          |          |
+----------+----------+----------+----------+----------+----------+

*Tabla 11: La relación clave-valor del valor de retorno de la función
``time.dump``*

Función ``clock``
^^^^^^^^^^^^^^^^^

.. code:: python

   clock()

**Descripción**

Esta función devuelve el tiempo transcurrido desde el inicio de la
ejecución del intérprete hasta que se llama a la función en segundos. El
valor de retorno de esta función es del tipo “real” y su precisión de
tiempo está determinada por la plataforma específica.

Módulo de cadena (string)
~~~~~~~~~~~~~~~~~~~~~~~~~

El módulo ``cadena`` proporciona funciones de procesamiento de cadenas.

Para usar el módulo de cadena, primero use la instrucción
``import string``. Todos los ejemplos de esta sección asumen que el
módulo se ha importado correctamente.

Función ``count``
^^^^^^^^^^^^^^^^^

::

   string.count(s, sub[, inicio[, fin]])

Cuenta el número de ocurrencias de la subcadena en la cadena s. Busque
desde la posición entre ``inicio`` y ``fin`` de ``s`` (el valor
predeterminado es 0 y tamaño(s)).

Función ``split``
^^^^^^^^^^^^^^^^^

::

   string.split(s, pos)

Divide la cadena s en dos subcadenas en la posición ``pos`` y devuelve
la lista de esas cadenas.

::

   string.split(s, sep[, num])

Divide la cadena s en subcadenas dondequiera que ocurra sep, y devuelve
la lista de esas cadenas. Dividir como máximo un número de veces (el
valor predeterminado es string.count(s, sep)).

Función ``find``
^^^^^^^^^^^^^^^^

::

   string.find(s, sub[, inicio[, fin]])

Compruebe si la cadena s contiene la subcadena sub. Si se especifican el
inicio y el final (el valor predeterminado es 0 y el tamaño(s)), se
buscarán en este rango.

Función ``hex``
^^^^^^^^^^^^^^^

::

   hex(numero)

Convertir número a cadena hexadecimal.

Función ``byte``
^^^^^^^^^^^^^^^^

::

   byte(s)

Obtiene el valor del código del primer byte de la cadena s.

Función ``char``
^^^^^^^^^^^^^^^^

::

   char(numero)

Convierte el número usado como código en un carácter.

Función ``format``
^^^^^^^^^^^^^^^^^^

::

   string.format(fmt[, args])

Devuelve una cadena formateada. El patrón que comienza con ‘%’ en la
plantilla de formato fmt será reemplazado por el valor de [args]:
%[flags][fieldwidth][.precision]type

+----+-----------------------------------------------------------------+
| Ti | Descripción                                                     |
| po |                                                                 |
+====+=================================================================+
| %d | Entero decimal                                                  |
+----+-----------------------------------------------------------------+
| %o | Entero octal                                                    |
+----+-----------------------------------------------------------------+
| %x | Entero hexadecimal en minúsculas                                |
+----+-----------------------------------------------------------------+
| %X | Entero hexadecimal en mayúsculas                                |
+----+-----------------------------------------------------------------+
| %x | Entero octal en minúsculas                                      |
+----+-----------------------------------------------------------------+
| %X | Entero octal en mayúsculas                                      |
+----+-----------------------------------------------------------------+
| %f | Punto flotante en la forma [-]nnnn.nnnn                         |
+----+-----------------------------------------------------------------+
| %e | Punto flotante en exp. forma [-]n.nnnn e [+|-]nnn, mayúsculas   |
| %E | si %E                                                           |
+----+-----------------------------------------------------------------+
| %g | Punto flotante como %f si −4 < exp. ≤ precision, sino como %e;  |
| %G | mayúsculas si %G                                                |
+----+-----------------------------------------------------------------+
| %c | Carácter que tiene el código pasado como entero                 |
+----+-----------------------------------------------------------------+
| %s | Cadena sin ceros incrustados                                    |
+----+-----------------------------------------------------------------+
| %q | Cadena entre comillas dobles, con caracteres especiales         |
|    | escapados                                                       |
+----+-----------------------------------------------------------------+
| %% | El carácter ‘%’ (escapado)                                      |
+----+-----------------------------------------------------------------+

+------+---------------------------------------------------------------+
| Tipo | Descripción                                                   |
+======+===============================================================+
| -    | Justificación a la izquierda, el valor predeterminado es      |
|      | justificación a la derecha                                    |
+------+---------------------------------------------------------------+
| +    | antepone el signo (se aplica a los números)                   |
+------+---------------------------------------------------------------+
| (    | Antepone signo si es negativo, de lo contrario espacio        |
| espa |                                                               |
| cio) |                                                               |
+------+---------------------------------------------------------------+
| #    | Agrega “0x” antes de %x, fuerza el punto decimal; para %e,    |
|      | %f, deja ceros finales para %g                                |
+------+---------------------------------------------------------------+

+------------+---------------------------------------------------------+
| Ancho de   | Descripción                                             |
| campo y    |                                                         |
| precisión  |                                                         |
+============+=========================================================+
| n          | Pone al menos n caracteres, rellena con espacios en     |
|            | blanco                                                  |
+------------+---------------------------------------------------------+
| 0n         | Pone al menos n caracteres, teclado izquierdo con ceros |
+------------+---------------------------------------------------------+
| .n         | Usa al menos n dígitos para números enteros, redondee a |
|            | n decimales para punto flotante o no más de n           |
|            | caracteres. para cadenas                                |
+------------+---------------------------------------------------------+

Módulo ``os``
~~~~~~~~~~~~~

El módulo OS proporciona funciones relacionadas con el sistema, como
funciones relacionadas con archivos y rutas. Estas funciones están
relacionadas con la plataforma. Actualmente, los códigos de estilo
Windows VC y POSIX se implementan en el intérprete de Berry. Si se
ejecuta en otras plataformas, no se garantiza que se proporcionen las
funciones en el módulo del sistema operativo.

[COMPLETAR]

Módulo ``global``
~~~~~~~~~~~~~~~~~

El módulo ``global`` proporciona una forma de acceder a las variables
globales a través de un módulo. El compilador Berry comprueba que existe
un global al compilar el código. Sin embargo, hay casos en los que los
globales se crean dinámicamente por código y aún no se conocen en tiempo
de compilación. El uso del módulo ``global`` da total libertad para
acceder a variables globales estáticas o dinámicas.

Acceder a un global es simplay hecho con ``global.<name>`` para leer y
escribir. También puede usar la sintaxis especial ``global.(nombre)`` si
``nombre`` es una variable que contiene el nombre del global como
cadena.

Ejemplo:

.. code:: berry

   > import global
   > a = 1
   > global.a
   1
   >
   > b
   syntax_error: stdin:1: 'b' no declarado (primer uso en esta función)
   > global.b = 2
   > b
   2
   > global.b
   2
   > var nombre = "b"
   > global.(nombre)
   2

Llamar a ``global()`` devuelve la lista de todos los nombres globales
actualmente definidos (los componentes integrados no están incluidos).

.. code:: berry

   > import global
   > a = 1
   > global.b = 2
   > global()
   ['_argv', 'b', 'global', 'a']

``global.contains(<nombre)> -> bool`` proporciona una manera fácil de
saber si un nombre global ya está definido.

.. code:: berry

   > import global
   > global.contains("g")
   false
   > g = 1
   > global.contains("g")
   true

Módulo ``introspect``
~~~~~~~~~~~~~~~~~~~~~

El módulo ``introspect`` proporciona primitivas para acceder
dinámicamente a variables o módulos. Usar con ``import introspect``.

``introspect.members(objeto: clase o módulo o instancia o nil) -> lista``
devuelve la lista de nombres de miembros para la ``clase``,
``instancia`` o ``módulo``. Tenga en cuenta que no incluye miembros
virtuales potenciales creados a través de ``member`` y ``setmember``.

``introspect.members()`` devuelve la lista de variables globales (sin
incluir las incorporadas) y es equivalente a ``global()``

``introspect.get(objeto: clase o instancia o módulo, nombre:cadena) -> cualquiera``
e
``introspect.set(objeto: clase o instancia o módulo, nombre:cadena, valor:cualquiera) -> nil``
permite leer y escribir cualquier miembro por su nombre.

``introspect.get(o, "a")`` es equivalente a ``oa``,
``introspect.set(o, "a", 1)`` es equivalente a ``oa = 1``. También hay
una sintaxis alternativa: ``o.("a")`` es equivalente a ``oa`` y
``o.("a) = 1`` es equivalente a ``oa = 1``.

``introspect.module(nombre:cadena) -> any`` es equivalente a
``import nombre`` excepto que no crea la variable global o local, sino
que devuelve el módulo. Esta es la única manera de cargar un módulo con
un nombre dinámico, ``import nombre`` solo toma un nombre estático.

``introspect.toptr(addr:int) -> comptr`` convierte un número entero en
un puntero comptr. ``introspect.fromptr(addr:comptr) -> int`` hace lo
contrario y convierte un puntero en un int. Advertencia: usar con
cuidado. En plataformas donde ``int`` y ``void*`` no tienen el mismo
tamaño, estas funciones seguramente darán resultados inutilizables.

``introspect.ismethod(f:function) -> bool`` comprueba si la función
proporcionada es un método de una instancia (tomando a sí mismo como
primer argumento) o una función simple. Esto se usa principalmente para
evitar un error común de pasar un método de instancia como callback,
donde debe usar un cierre que capture la instancia como
``/ -> self.do()``.

Módulo ``solidify``
~~~~~~~~~~~~~~~~~~~

Este módulo permite solidificar el bytecode de Berry en flash. Esto
permite ahorrar RAM ya que el código está en ROM. Esto lo convierte en
una buena alternativa a las funciones nativas de C.

Ver 8.4 Solidificación
