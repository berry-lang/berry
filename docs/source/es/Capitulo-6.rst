.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

6. Función orientada a objetos
==============================

Por consideraciones de optimización, Berry no consideró los tipos
simples como objetos. Estos tipos simples incluyen ``nil``, numéricos,
booleanos y cadena. Pero Berry proporciona clases para implementar el
mecanismo de objetos. Entre los tipos de datos básicos de Berry,
``list``, ``map`` y ``range`` son objetos de clase. Un objeto es una
colección que contiene datos y métodos, donde los datos se componen de
algunas variables y los métodos son funciones. El tipo de un objeto se
denomina clase y la entidad de un objeto se denomina instancia.

6.1 Clase e instancia
---------------------

6.1.1 Declaración de clase
~~~~~~~~~~~~~~~~~~~~~~~~~~

Para usar una clase, primero debe declararla. La declaración de una
clase comienza con la palabra clave ``class``. Las variables miembro y
los métodos de la clase deben especificarse en la declaración. Este es
un ejemplo de declaración de una clase:

.. code:: berry

   class persona
       static var mayor = 18
       var nombre, edad
       def init(nombre, edad)
           self.nombre = name
           self.edad = edad
       end
       def tostring()
           return 'nombre: '+ str(self.nombre) + ', edad:' + str(self.edad)
       end
       def es_adulto()
           return self.edad >= self.mayor
       end
   end

Las variables miembro de clase se declaran con la palabra clave ``var``,
mientras que los métodos miembro se declaran con la palabra clave
``def``. Actualmente, Berry no admite la inicialización de variables
miembro en el momento de la definición, por lo que el constructor debe
realizar la inicialización de las variables miembro. Las propiedades de
la clase no se pueden modificar después de completar la declaración, por
lo que la clase es un objeto de solo lectura.

Este diseño es para garantizar que la clase se pueda construir
estáticamente en el lenguaje C cuando se implemente el intérprete y se
pueda usar la propiedad ``const`` modificada para ahorrar RAM

La clase de Berry no admite restricciones de acceso y todas las
propiedades de la clase son visibles desde el exterior. En las clases
nativas, puede usar algunos trucos para hacer que las propiedades sean
invisibles para el código Berry (por lo general, hacer que el nombre del
miembro comience con un punto “.”). Puede usar algunas convenciones para
restringir el acceso a los miembros de la clase, como la convención de
que los atributos que comienzan con un guión bajo son atributos
privados. Esta convención no tiene ningún uso a nivel gramatical, pero
favorece la estructura lógica del código.

Instanciar
~~~~~~~~~~

La clase en sí es solo una descripción abstracta. Tomando los autos como
ejemplo, conozco el concepto de autos, y cuando realmente queremos usar
autos, necesitamos autos reales. El uso de las clases es similar. No
solo usaremos esta descripción abstracta, sino que necesitaremos
producir un objeto concreto basado en esta descripción. Este proceso se
llama **Instanciación de la clase**, abreviado como instanciación, y el
objeto concreto producido por la instanciación se llama **Instancia**.
La clase en sí no tiene datos, y la creación de instancias produce una
instancia basada en la información descrita por la clase y proporciona
datos específicos a la instancia.

Método y parámetros ``self``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los métodos de clase son esencialmente funciones. A diferencia de las
funciones ordinarias, los métodos pasan implícitamente un parámetro
``self``, y siempre es el primer parámetro, que almacena una referencia
a la instancia actual. Debido a la existencia de parámetros ``self``, el
número de parámetros del método será uno más que el número de parámetros
definidos en la declaración. Aquí usamos un ejemplo simple para
demostrar:

.. code:: berry

   class Test
       def metodo()
           return self
       end
   end
   objecto = Test()
   print(objecto)
   print(objecto.metodo())

Este ejemplo define una clase ``Test``, que tiene un método ``metodo``,
que devuelve su parámetro ``self``. Las dos últimas líneas de la rutina
imprimen el valor de la instancia ‘objeto’ de la clase ``Test`` y el
valor de retorno del método ‘metodo’ respectivamente. El resultado de
ejecución de este ejemplo es:

::

   <instance: Test()>
   <instance: Test()>

Se puede ver que el parámetro ``self`` del método y el nombre de la
instancia de uso (``objecto`` en el ejemplo) representan el mismo objeto
y ambos son referencias de instancia. Use ``self`` para acceder a los
miembros o atributos de la instancia en el método.

Métodos sintéticos
~~~~~~~~~~~~~~~~~~

Puede declarar métodos y miembros dinámicos sintéticos usando **Miembros
virtuales** como se describe en el Capítulo 8.2.

Variables de clase ``static``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Las variables o funciones se pueden declarar ``static``. Las variables
estáticas tienen el mismo valor para todas las instancias de la misma
clase. Se declaran como ``static a = 1`` o ``static var a = 1``. Las
variables estáticas se inicializan justo después de la creación de la
clase.

Métodos de clase ``static``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los métodos se pueden declarar ``static``, lo que significa que actúan
como una función regular y no toman ``self`` como primer argumento.
Dentro de los métodos estáticos, no se declara ninguna variable “auto”
implícita. Los métodos estáticos se pueden llamar a través de la clase o
a través de una instancia.

.. code:: berry

   class static_demo
         static def incremento_static(i)
             return i + 1
         end
         def incremento_instancia(i)
             return i + 1
         end
     end
   a = static_demo()
   static_demo.incremento_static(1)    # llamada via clase
   
2

.. code:: berry

   a.incremento_static(1)              # llamada via instancia
   static_demo.incremento_instancia(1)
   
| type_error: unsupported operand type(s) for +: 'nil' and 'int'
|     stack traceback:
|     stdin:6: in function `increment_instancia`
| stdin:1: in function `main`
|

.. code:: berry

   a.increment_instancia(1)
  
2

Constructor y Destructor
~~~~~~~~~~~~~~~~~~~~~~~~

Constructor
^^^^^^^^^^^

El constructor de la clase es el método ``init``. Se llama al
constructor cuando se crea una instancia de la clase. Por lo tanto, el
constructor generalmente se usa para la inicialización de miembros, por
ejemplo:

.. code:: berry

   class Test
       var a
       def init()
           self.a ='esto es una prueba'
       end
   end

El constructor de este ejemplo inicializa el miembro ``a`` de la clase
``Test`` con la cadena ``'esto es una prueba'``. Si instanciamos la
clase, podemos obtener el valor del miembro ``a``:

.. code:: berry

   class Test
       var a
       def init()
           self.a ='esta es una prueba'
       end
   end

Destructor
^^^^^^^^^^

El destructor de la clase es el método ``deinit``. Se llama al
destructor cuando se destruye la instancia. El destructor se usa
generalmente para completar algún trabajo de limpieza. Debido a que el
mecanismo de recolección de basura libera automáticamente la memoria de
los objetos inútiles, no hay necesidad de liberar la memoria en el
destructor (y tampoco hay forma de hacerlo en el destructor). En la
mayoría de los casos, no hay necesidad de usar un destructor, a menos
que cierta clase requiera cierto procesamiento cuando se destruye. Un
ejemplo típico es que un objeto de archivo debe cerrar el archivo cuando
se destruye.

Herencia de clases
------------------

Berry solo admite herencia simple, es decir, una clase solo puede tener
una clase base, y la clase base usa el operador ``:`` para declarar:

.. code:: berry

   class Test: Base
       ...
   end

Aquí la clase ``Test`` hereda de la clase ``Base``. La subclase heredará
todos los métodos y propiedades de la clase base y puede anularlos en la
subclase. Este mecanismo se llama **Sobrecarga**. En circunstancias
normales, solo sobrecargaremos métodos, no propiedades.

El mecanismo de herencia de la clase Berry es relativamente simple. Las
subclases contendrán referencias a la clase base y los objetos de
instancia son similares. Al instanciar una clase con una clase base, en
realidad se generan múltiples objetos. Estos objetos se encadenarán de
acuerdo con la relación de herencia y, finalmente, obtendremos el objeto
de instancia al final de la cadena de herencia.

Sobrecarga de método
--------------------

La **Sobrecarga** significa que la subclase y la clase base usan el
mismo método de nombre, y el método de la subclase anulará el mecanismo
del método de la clase base. Para ser precisos, las variables miembro
también se pueden sobrecargar, pero esta sobrecarga no tiene sentido. La
sobrecarga de métodos se divide en sobrecarga de métodos ordinarios y
sobrecarga de operadores.

Sobrecarga de método común
~~~~~~~~~~~~~~~~~~~~~~~~~~

Sobrecarga del operador
~~~~~~~~~~~~~~~~~~~~~~~

Puede usar la sobrecarga de operadores de la clase para hacer que la
instancia admita la operación del operador integrado. Por ejemplo, para
una clase sobrecargada con el operador de suma, podemos usar el operador
de suma para realizar operaciones en la instancia. Un operador
sobrecargado es un método con un nombre especial, y la forma de función
sobrecargada de un operador binario es

.. code::

      ´def’ operador ´(´ otro ´)´
        bloque
      ´end’

**operador** es un operador binario sobrecargado. El operando izquierdo
del operador binario es el objeto ``self`` y el operando derecho es el
valor del parámetro **otro**. La forma de función sobrecargada del
operador unario es

.. code::

      ´def’ operador ´()´
        bloque
      ´end’

**operador** es un operador unario sobrecargado. Para distinguirlo del
operador de resta, el signo menos unario se escribe como ``-*`` cuando
está sobrecargado. Las funciones sobrecargadas del operador deben tener
un valor de retorno, porque el valor de retorno ``nil`` predeterminado
no suele ser el resultado esperado. Tomemos una clase entera como
ejemplo para ilustrar el uso de la sobrecarga de operadores. Primero
defina la clase ``integer``:

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

La clase ``integer`` sobrecarga los operadores suma, multiplicación y
simbólicos, y el método ``tostring`` hace que la instancia use la
función ``print`` para generar el resultado. Podemos usar una simple
línea de código para probar la función de sobrecarga de operadores de la
clase:

.. code:: berry

   integer(1) + integer(2) * -integer(3) # -5

El resultado de esta línea de código es una instancia de ``integer``. El
valor del miembro ``value`` de esta instancia es ``-5``, que es el mismo
resultado de las mismas cuatro operaciones aritméticas con números
enteros.

Los operadores lógicos no se pueden sobrecargar directamente. Si
necesita una instancia para admitir operaciones lógicas, debe
implementar el método ``tobool``. El método no tiene parámetros y el
valor devuelto debe ser de tipo booleano. La operación lógica de la
instancia en realidad se realiza convirtiendo la instancia en un valor
booleano, por lo que la operación lógica de la instancia está
completamente en línea con la naturaleza de la operación lógica general.
El operador de subíndice no se sobrecarga directamente, pero se
implementa mediante los métodos ``item`` y ``setitem``. El método
``item`` se utiliza para la lectura de subíndices, su primer parámetro
es el valor del subíndice y el valor de retorno es el resultado de la
operación del subíndice; ``setitem`` se utiliza para la escritura de
subíndices, y su primer parámetro es el valor del subíndice, el segundo
parámetro es el valor que se va a escribir; este método no utiliza el
valor de retorno.

Al operador sobrecargado se le puede asignar cualquier significado,
incluso sin satisfacer las propiedades habituales de los operadores.
Dada la versatilidad del código y la dificultad de comprensión, no se
recomienda que los usuarios den a los operadores sobrecargados una
función alejada del significado general.

Sobrecarga del operador de asignación compuesto
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

El operador de asignación compuesto no se puede sobrecargar
directamente, pero podemos lograr el propósito de “sobrecargar” el
operador de asignación compuesto sobrecargando el operador binario
correspondiente al operador de asignación compuesto. Por ejemplo,
después de sobrecargar el operador “``+``”, puede usar el operador
“``+=``” para instancias de clases relacionadas. Vale la pena señalar
que el uso de operaciones de asignación compuestas en la instancia hará
que las variables de la instancia vinculada pierdan su referencia a la
instancia.

.. code:: berry

   class integer
       var valor
       def init(x)
           self.valor = x
       end
       def +(other)
           return integer(self.valor + other.valor)
       end
   end
   a = integer(4) # a: <instance: 0x55edff400a78>
   a += integer(5) # a: <instance: 0x55edff4011b8>
   print(a.valor) # 9

Después de que se ejecuta la línea 11 de código, la instancia enlazada
en la variable ``a`` realmente ha cambiado. Esta línea de código es
equivalente a ``a = integer(4) + integer(5)``. Si el operador binario de
la sobrecarga de clase no modifica el estado de la instancia, entonces
el operador de asignación compuesto correspondiente no modificará
ninguna instancia (puede generar nuevas instancias).

Instancia
---------

Una **Instancia** es un objeto generado después de la instanciación de
la clase. Una clase se puede instanciar varias veces para generar
diferentes instancias. Las instancias de Berry están referenciadas por
la clase a la que pertenecen y los campos de datos correspondientes.
Todas las instancias de una clase se referirán a esta clase, pero los
campos de datos de estas instancias son independientes entre sí.

Objeto de clase base de acceso
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La función integrada ``super`` se utiliza para acceder a objetos de
clase superior. Se puede utilizar en clases o instancias.

La magia ocurre cuando llamas a un método de la superclase para que se
comporte como intuitivamente crees que lo haría. Por ejemplo, el patrón
común para ``init()`` es el siguiente:

.. code:: berry

   def init(<args>)
       # hacer cosas antes de super init
       super(self).init(<args>)
       # hacer cosas después de super init
   end

Tenga en cuenta que las clases siempre contienen métodos ``init()``
implícitos que no hacen nada, por lo que siempre puede llamar a init
desde la superclase incluso si no se declaró ningún método ``init()``.

Ejemplo completo:

.. code:: berry

   class A
       var val
       def init(val)
           # super(self).init(val)    # esto sería válido pero inútil
           self.val = val
       end
       def tostring()
           return "val=" + str(self.val)
       end
   end

   class B: A
       var magia    # verdadero si el valor es 42
       def init(val)
           super(self).init(val)     # llamar a superinit
           self.magia = (val == 42)
       end
       def tostring()
           if self.magia
               return "magia!"
           else
               return super(self).tostring()
           end
       end
   end

   ####### Ejemplo de uso

   > b1 = B(1)
   > b1
   val=1
   > b42 = B(42)
   > b42
   magia!

**Características avanzadas**: Al llamar a
``super(self).<method> (<args> )`` ocurre algo de magia. Cuando se llama
al supermétodo, los argumentos ``self`` se refieren a la clase
específica más baja. Sin embargo, el ``<method>`` no se busca desde la
clase de ``self`` (que siempre es la más baja), sino desde la superclase
de la clase que contiene el método que se está ejecutando actualmente.

Ejemplo:

.. code:: berry

   > class A
         def init()
             print("In A::init, self es de tipo", classname(self))
         end
     end
   > class B:A
         def init()
             print("In B::init, self es de tipo", classname(self))
             super(self).init()
         end
     end
   > class C:B
         def init()
             print("En C::init, self es de tipo", classname(self))
             super(self).init()
         end
     end
   > c = C()
   En C::init, self es de tipo C
   In B::init, self es de tipo C
   In A::init, self es de tipo C
   >

Explicación:

-  llamando a ``C:init()`` en ``instancia<C>`` - en ``C:init()``
   ``self`` es ``instancia<C>``, ``super(self).init()`` se refiere a la
   superclase de ``C`` (método actual), es decir, ``B``, por lo que
   ``B:init()`` se llama con ``instance<C>`` argumento - en ``B:init()``
   ``self`` es ``instancia<C>``, ``super(self).init()`` se refiere a la
   superclase de ``B`` (método actual), es decir, ``A``, por lo que
   ``A:init()`` se llama con ``instance<C>`` argumento - en ``A:init()``
   ``self`` es ``instancia<C>``, imprimir y devolver

Nota: por compatibilidad con versiones anteriores, super puede tomar un
segundo argumento ``super(instancia, clase)`` para especificar la clase
donde resolver el método. Esta función no debe usarse más, ya que es
propensa a errores.
