.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

8. Características avanzadas
============================

8.1 Modo ``estricto``
---------------------

Berry permite total libertad del desarrollador. Pero después de un poco
de experiencia en la codificación con Berry, encontrará que hay errores
comunes que son difíciles de encontrar y que el compilador podría
ayudarlo a detectar. El modo ``estricto`` realiza verificaciones
adicionales **en tiempo de compilación** sobre algunos errores comunes.

Este modo está habilitado con ``import strict`` o cuando se ejecuta
Berry con la opción ``-s``: ``berry -s``

``var`` obligatorio para variables locales
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Este es el error más común, una variable asignada sin ``var`` es global
si ya existe una variable global o local en caso contrario. El modo
estricto rechaza la asignación si no hay un global con el mismo nombre.

No más permitido:

.. code:: berry

   def f()
     i = 0    # this is a local variable
     var j = 0
   end

| syntax_error: stdin:2: strict: no global 'i', ¿quiso decir 'var i'?
|

Pero todavía funciona para globales:

.. code:: berry

   g_i = 0
   def f()
     g_i = 1
   end

Sin anulación de elementos integrados
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Berry permite anular una función incorporada. Sin embargo, esto
generalmente no es deseable y es una fuente de errores difíciles de
encontrar.

::

   map = 1
   syntax_error: stdin:1: estricto: redefinición de 'map' incorporado

Múltiples ``var`` con el mismo nombre no permitidos en el mismo ámbito
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Berry toleraba la declaración múltiple de una variable local con el
mismo nombre. Esto ahora se considera como un error (incluso sin modo
estricto).

::

   def f()
     var a
     var a   # redefinición de a
   end
   syntax_error: stdin:3: redefinición de 'a'

No ocultar la variable local del alcance externo
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

En Berry puedes declarar variables locales con el mismo nombre en el
ámbito interno. La variable en el ámbito interno oculta la variable del
ámbito externo durante la duración del ámbito.

La única excepción son las variables que comienzan con el punto ‘.’ que
se pueden enmascarar desde el alcance externo. Este es el caso de la
variable local oculta ``.it`` cuando se incrustan múltiples ``for``.

::

   def f()
     var a    # variable en el ámbito externo
     if a
       var a  # redefinición de a en ámbito interno
     end
   end
   syntax_error: stdin:4: estricto:  redefinición de 'a' desde el ámbito externo

8.2 Miembros virtuales
----------------------

Los miembros virtuales le permiten agregar de forma dinámica y
programática miembros y métodos a clases y módulos. Ya no está limitado
a los miembros declarados en el momento de la creación.

Esta función está inspirada en ``__getattr__()`` / ``__setattr__()`` de
Python. La motivación proviene de la integración de LVGL a Berry en
Tasmota. La integración necesita cientos de constantes en un módulo y
miles de métodos asignados a funciones C. La creación estática de
atributos y métodos funciona, pero consume una cantidad significativa de
espacio de código.

Esta característica permite crear dos métodos:

+-----+----------------------------------------------------------------+
| Mét | Descripción                                                    |
| odo |                                                                |
| Be  |                                                                |
| rry |                                                                |
+=====+================================================================+
| `   | ``(nombre:cadena) -> any``\  Debería devolver el valor del     |
| `me | ``nombre``                                                     |
| mbe |                                                                |
| r`` |                                                                |
+-----+----------------------------------------------------------------+
| `   | ``(nombre:cadena, valor:any) especificado -> nil``\  Debería   |
| `se | almacenar el ‘valor’ en el miembro virtual con el ‘nombre’     |
| tme | especificado                                                   |
| mbe |                                                                |
| r`` |                                                                |
+-----+----------------------------------------------------------------+

Módulo ``undefined``
~~~~~~~~~~~~~~~~~~~~

La función ``member()`` debe ser capaz de distinguir entre un miembro
con un valor ``nil`` y el miembro que no existe. Para evitar cualquier
ambigüedad, la función ``member()`` puede indicar que el miembro no
existe de dos maneras:

-  generar una excepción - o ``import undefined`` y devolver el módulo
   ``undefined``. Esto se usa como un marcador para que la VM sepa que
   el atributo no existe, mientras se beneficia de excepciones
   consistentes.

Ejemplo de un objeto dinámico al que puede agregar miembros, pero
devolvería un error si el miembro no se agregó previamente.

.. code:: berry

   class dyn
       var _attr
       def init()
           self._attr = {}
       end
       def setmember(nombre, valor)
           self._attr[nombre] = valor
       end
       def member(nombre)
           if self._attr.contains(nombre)
               return self._attr[nombre]
           else
               import undefined
               return undefined
           end
       end
   end

Ejemplo de uso:

.. code:: berry

   a = dyn()
   a.a

attribute_error: el objeto 'dyn' no tiene el atributo 'a'
stack traceback:
stdin:1: en función `main`

   a.a = 1
   a.a
   
1

   a.a = nil
   a.a

Llamada implícita de ``member()``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cuando se ejecuta el siguiente código ``a.b``, Berry VM hace lo
siguiente:

-  Obtiene el objeto llamado ``a`` (local o global), genera una
   excepción si no existe
-  Comprueba si el objeto ``a`` es de tipo ``módulo``, ``instancia`` o
   ``clase``. Genera una excepción de lo contrario
-  Comprueba si el objeto ``a`` tiene un miembro llamado ``b``. En caso
   afirmativo, devuelve su valor, en caso negativo, procede a
   continuación
-  Si el objeto ``a`` es del tipo ``clase``, genera una excepción porque
   los miembros virtuales no funcionan para métodos estáticos (clase)
-  Comprueba si el objeto ``a`` tiene un miembro llamado ``member`` y es
   una ``función``. En caso afirmativo, lo llama con el parámetro
   ``"b"`` como cadena. Si no, genera una excepción
-  Comprueba el valor de retorno. Si es el módulo ``undefined`` genera
   una excepción que indica que el miembro no existe

Llamada implícita de ``setmember()``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cuando se ejecuta el siguiente código ``ab = 0`` (mutador), Berry VM
hace lo siguiente:

-  Obtiene el objeto llamado ``a`` (local o global), genera una
   excepción si no existe
-  Comprueba si el objeto ``a`` es de tipo ``módulo``, ``instancia`` o
   ``clase``. Genera una excepción de lo contrario

   -  Si ``a`` es del tipo ``clase``, comprueba si existe el miembro
      ``b``. En caso afirmativo, cambia su valor. Si no, genera una
      excepción. (los miembros virtuales no funcionan para clases o
      métodos estáticos)
   -  Si ``a`` es del tipo ``instancia``, comprueba si existe el miembro
      ``b``. En caso afirmativo, cambia su valor. Si no, procede a
      continuación

      -  Comprueba si ``a`` tiene un miembro llamado ``setmember``. Si
         es así, lo llama, si no, genera una excepción.

   -  Si ``a`` es de tipo ``módulo``. Si el módulo no es de solo
      lectura, crea o cambia el valor (``setmember`` nunca se llama para
      un módulo de escritura). Si el módulo es de solo lectura, entonces
      se llama a ``setmember`` si existe.

Manejo de excepciones
~~~~~~~~~~~~~~~~~~~~~

Para indicar que un miembro no existe, ``member()`` devolverá
``undefined`` después de ``import undefined``. También puede generar una
excepción en ``member()``, pero tenga en cuenta que Berry podría
intentar llamar a métodos como ``tostring()`` que aterrizarán en su
método ``member()`` si no existen como métodos estáticos. Para indicar
que un miembro no es válido, ``setmember()`` debe generar una excepción
o devolver ``undefined``. Devolver cualquier otra cosa como ``nil``
indica que la asignación fue exitosa. Tenga en cuenta que puede recibir
nombres de miembros que no sean identificadores válidos de Berry. La
sintaxis ``a.("<->")`` llamará a ``a.member("<->")`` con un nombre de
miembro virtual que no es léxicamente válido, es decir, no se puede
llamar en código normal, excepto mediante el uso indirecto formas como
``introspect`` o ``member()``.

Especificidades para las clases
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

El acceso a los miembros del objeto de clase no desencadena miembros
virtuales. Por lo tanto, no es posible tener métodos estáticos
virtuales.

Especificidades de los módulos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los módulos admiten la lectura de miembros estáticos con ``member()``.
Al escribir en un miembro, el comportamiento depende de si el módulo es
de escritura (en la memoria) o de solo lectura (en el firmware). Si se
puede escribir en el módulo, los nuevos miembros se agregan directamente
al módulo y nunca se llama a ``setmember()``. Si el módulo es de solo
lectura, se llama a ``setmember()`` cada vez que intenta cambiar o crear
un miembro. Entonces es su responsabilidad almacenar los valores en un
objeto separado como un global.

Ejemplo
~~~~~~~

.. code:: python

   class T
       var a
       def init()
           self.a = 'a'
       end

       def member(nombre)
           return "miembro "+nombre
       end

       def setmember(nombre, valor)
           print("Almacenar '"+nombre+"': "+str(valor))
       end
   end
   t=T()

Ahora intentémoslo:

.. code:: berry

   t.a

'a'

.. code:: berry

   t.b

'miembro b'

.. code:: berry

   t.foo

'miembro foo'

.. code:: berry

   t.bar = 2

Almacenar 'bar': 2

Esto también funciona para los módulos:

.. code:: berry

   m = module()
   m.a = 1
   m.member = def (nombre)
       return "miembro "+nombre
   end
   m.setmember(nombre, valor)
       print("Almacenar '"+nombre+"': "+str(valor))
   end

Intentemoslo:

.. code:: berry

   m.a

1

.. code:: berry

   m.b

'miembro b'

.. code:: berry

   m.c = 3   # la asignación es válida por lo que no se llama a `setmember()
   m.c

3

Ejemplo más avanzado:

.. code:: berry

   class A
       var i

       def member(n)
         if n == 'ii' return self.i end
         return nil     # lo hacemos explícito aquí, pero esta línea es opcional
       end

       def setmember(n, v)
         if n == 'ii' self.i = v end
       end
     end
   a=A()

   a.i      # devuelve nil
   a.ii     # i llama implícitamente `a.member("ii")`
   
| attribute_error: el objeto 'A' no tiene atributo 'ii'
| stack traceback:
| stdin:1: en función `main`
|

.. code:: berry

   # devuelve un excepción ya que el miembro es nulo (considerado inexistente)

   a.ii = 42    # llama implícitamente `a.setmember("ii", 42)`
   a.ii         # llama implícitamente `a.member("ii")` and returns `42`
   
42

.. code:: berry

   a.i          #  la variable concreta también fue cambiada
   
42

8.3 Cómo empaquetar un módulo
-----------------------------

Esta guía lo lleva a través de las diferentes opciones de empaquetado de
código para su reutilización utilizando la directiva de “import” de
Berry.

Comportamiento de ``import``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cuando se utiliza ``import <modulo> [as <nombre> ]``, suceden los
siguientes pasos:

-  Hay una caché global de todos los módulos ya importados. Si
   ``<modulo>`` ya fue importado, ``import`` devuelve el valor en caché
   ya devuelto por la primera llamada a ``import``. No se realizan otras
   acciones.
-  ``import`` busca un módulo de nombre ``<modulo>`` en el siguiente
   orden:

1. en módulos nativos incrustados en el firmware en tiempo de
   compilación
2. en el sistema de archivos, comenzando con el directorio actual, luego
   iterando en todos los directorios desde ``sys.path``: busque el
   archivo ``<nombre>``, entonces ``<nombre>.bec`` (código de bytes
   compilado), luego ``<nombre>.be``. Si ``BE_USE_SHARED_LIB`` está
   habilitado, también busca bibliotecas compartidas como
   ``<nombre>.so que`` o ``<nombre>.dll`` aunque esta opción
   generalmente no está disponible en MCU.

-  Se ejecuta el código cargado. El código debe terminar con una
   declaración ``return``. El objeto devuelto se almacena en la memoria
   caché global y se pone a disposición de la persona que llama (en el
   ámbito local o global).
-  Si el objeto devuelto es un ``módulo`` y si el módulo posee un
   miembro ``init``, entonces se toma un paso adicional. La función
   ``<modulo>.init(m)`` se llama pasando como argumento el propio objeto
   del módulo. El valor devuelto por ``init()`` reemplaza el valor en el
   caché global. Tenga en cuenta que ``init()`` se llama como máximo una
   vez durante la primera ``importación``.

Nota: una función ``init(m)`` implícita siempre está presente en todos
los módulos, incluso si no se declaró ninguno. Esta función implícita no
tiene ningún efecto.

Empaquetado de un módulo
~~~~~~~~~~~~~~~~~~~~~~~~

Aquí hay un ejemplo simple de un módulo:

Archivo ``demo_modulo.be``:

.. code:: berry

   # modulo simple
   # use `import demo_modulo`

   demo_module = module("demo_module")

   demo_modulo.foo = "bar"

   demo_modulo.decir_hola = def ()
       print("Hola Berry!")
   end

   return demo_modulo      # devuelve el módulo como salida de import

Ejemplo de uso:

.. code:: berry

   import demo_modulo

   demo_modulo
   <module: demo_modulo>

   demo_module.decir_hola()
   
Hola Berry!

.. code:: berry

   demo_modulo.foo
   
'bar'

.. code:: berry

   demo_modulo.foo = "baz"   # el módulo se puede escribir, aunque esto es muy desaconsejado
   demo_modulo.foo
   
'baz'

Empaquetar un singleton (mónada)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

El problema de usar módulos es que no tienen variables de instancia para
realizar un seguimiento de los datos. Están diseñados esencialmente para
bibliotecas sin estado.

A continuación, encontrará una forma elegante de empaquetar una clase
única devuelta como una “declaración de importación”.

Para ello, utilizamos diferentes trucos. Primero, declaramos la clase
para el singleton como una clase interna de una función, esto evita que
se contamine el espacio de nombres global con esta clase. Es decir, la
clase no será accesible por otro código.

En segundo lugar, declaramos una función ``init()`` del módulo que crea
la clase, crea la instancia y la devuelve.

Según este esquema, ``import <modulo>`` en realidad devuelve una
instancia de una clase oculta.

Ejemplo de ``demo_monad.be``:

.. code:: berry

   # monada simple
   # use `import demo_monad`

   demo_monad = module("demo_monad")

   #  el módulo tiene un solo miembro `init()` y delega todo a la clase interna
   demo_monad.init = def (m)

       # inncer class
       class my_monad
           var i

           def init()
               self.i = 0
           end

           def say_hello()
               print("Hola Berry!")
           end
       end

       # rdevolver una sola instancia para esta clase
       return my_monad()
   end

   return demo_monad      # evuelve el módulo como la salida de importación, que eventualmente se reemplaza por el valor de retorno de 'init()'

Ejemplo:

.. code:: berry

   import demo_monad
   demo_monad
   <instance: my_monad()>     # es una instancia no un modulo

   demo_monad.say_hello()
   
Hola Berry!

.. code:: berry

   demo_monad.i = 42        #  puedes usarlo como cualquier instancia
   demo_monad.i
   
42

.. code:: berry

   demo_monad.j = 0         # hay una fuerte verificación de miembros en comparación con los módulos 
   
Attribute_error: la clase 'my_monad' no puede asignarse al atributo 'j'
stack traceback:
stdin:1: en función `main`

8.4 Solidificación
------------------

La solidificación es el proceso de capturar estructuras y códigos Berry
compilados (clases, módulos, mapas, listas…) y almacenarlos en el
firmware. Reduce drásticamente el uso de la memoria, pero tiene algunas
limitaciones.

Módulo ``solidify``
~~~~~~~~~~~~~~~~~~~

La solidificación es manejada por el módulo ``solidify``. Este módulo no
está compilado por defecto debido a su tamaño (~10kB). Debe compilar con
la directiva ``#define BE_USE_SOLIDIFY_MODULE 1``.

El módulo tiene un solo miembro ``dump(x)`` que toma un solo argumento
(el objeto a solidificar) y envía a ``stdout`` el código solidificado.

De forma predeterminada, solidify agrega todas las constantes de cadena
al grupo global. En su lugar, puede generar cadenas débiles (elegibles
para la poda por parte del enlazador) estableciendo el segundo argumento
en “verdadero”.

Por defecto, ``solidify.dump`` genera el código solidificado en la
salida estándar. Puede especificar un archivo como tercer argumento. El
archivo debe estar abierto en modo de escritura y no está cerrado para
que pueda concatenar varios objetos.

``solidify.dump(object:any, [, strings_weak:bool, file_out:file]) -> nil``

Solidificación de funciones
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Puede solidificar una sola función.

Ejemplo:

.. code:: berry

   > def f() return "hello" end
   > import solidify
   > solidify.dump(f)

.. code:: c

   /********************************************************************
   ** Solidified function: f
   ********************************************************************/
   be_local_closure(f,   /* name */
     be_nested_proto(
       0,                          /* nstack */
       0,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str(hello),
       }),
       &be_const_str_f,
       &be_const_str_solidified,
       ( &(const binstruction[ 1]) {  /* code */
         0x80060000,  //  0000  RET    1   K0
       })
     )
   );
   /*******************************************************************/

Para compilar utilizando cadenas débiles (es decir, cadenas que el
enlazador puede eliminar si el objeto no está incluido en el ejecutable
de destino), use ``solidify.dump(f, true)``:

.. code:: c

   /********************************************************************
   ** Solidified function: f
   ********************************************************************/
   be_local_closure(f,   /* name */
     be_nested_proto(
       0,                          /* nstack */
       0,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str_weak(hello),
       }),
       be_str_weak(f),
       &be_const_str_solidified,
       ( &(const binstruction[ 1]) {  /* code */
         0x80060000,  //  0000  RET    1   K0
       })
     )
   );
   /*******************************************************************/

Solidificación de clases
~~~~~~~~~~~~~~~~~~~~~~~~

Cuando solidifica una clase, incrusta todos los subelementos. También se
agrega un código auxiliar ``C`` para crear la clase y agregarla al
ámbito global.

.. code:: berry

   >  class demo
         var i
         static foo = "bar"

         def init()
             self.i = 0
         end

         def say_hello()
             print("Hello Berry!")
         end
     end
   > import solidify
   > solidify.dump(demo)

.. code:: c


   /********************************************************************
   ** Solidified function: init
   ********************************************************************/
   be_local_closure(demo_init,   /* name */
     be_nested_proto(
       1,                          /* nstack */
       1,                          /* argc */
       2,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 2]) {     /* constants */
       /* K0   */  be_nested_str(i),
       /* K1   */  be_const_int(0),
       }),
       &be_const_str_init,
       &be_const_str_solidified,
       ( &(const binstruction[ 2]) {  /* code */
         0x90020101,  //  0000  SETMBR R0  K0  K1
         0x80000000,  //  0001  RET    0
       })
     )
   );
   /*******************************************************************/

   /********************************************************************
   ** Solidified function: say_hello
   ********************************************************************/
   be_local_closure(demo_say_hello,   /* name */
     be_nested_proto(
       3,                          /* nstack */
       1,                          /* argc */
       2,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str(Hello_X20Berry_X21),
       }),
       &be_const_str_say_hello,
       &be_const_str_solidified,
       ( &(const binstruction[ 4]) {  /* code */
         0x60040001,  //  0000  GETGBL R1  G1
         0x58080000,  //  0001  LDCONST    R2  K0
         0x7C040200,  //  0002  CALL   R1  1
         0x80000000,  //  0003  RET    0
       })
     )
   );
   /*******************************************************************/

   /********************************************************************
   ** Solidified class: demo
   ********************************************************************/
   be_local_class(demo,
       1,
       NULL,
       be_nested_map(4,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(i, -1), be_const_var(0) },
           { be_const_key(say_hello, 2), be_const_closure(demo_say_hello_closure) },
           { be_const_key(init, -1), be_const_closure(demo_init_closure) },
           { be_const_key(foo, 1), be_nested_str(bar) },
       })),
       (bstring*) &be_const_str_demo
   );
   /*******************************************************************/

   void be_load_demo_class(bvm *vm) {
       be_pushntvclass(vm, &be_class_demo);
       be_setglobal(vm, "demo");
       be_pop(vm, 1);
   }

Las subclases también son compatibles.

.. code:: berry

   > class demo_sub : demo
         var j

         def init()
             super(self).init()
             self.j = 1
         end
     end
   > solidify.dump(demo_sub)

.. code:: c


   /********************************************************************
   ** Solidified function: init
   ********************************************************************/
   be_local_closure(demo_sub_init,   /* name */
     be_nested_proto(
       3,                          /* nstack */
       1,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 3]) {     /* constants */
       /* K0   */  be_nested_str(init),
       /* K1   */  be_nested_str(j),
       /* K2   */  be_const_int(1),
       }),
       &be_const_str_init,
       &be_const_str_solidified,
       ( &(const binstruction[ 7]) {  /* code */
         0x60040003,  //  0000  GETGBL R1  G3
         0x5C080000,  //  0001  MOVE   R2  R0
         0x7C040200,  //  0002  CALL   R1  1
         0x8C040300,  //  0003  GETMET R1  R1  K0
         0x7C040200,  //  0004  CALL   R1  1
         0x90020302,  //  0005  SETMBR R0  K1  K2
         0x80000000,  //  0006  RET    0
       })
     )
   );
   /*******************************************************************/

   /********************************************************************
   ** Solidified class: demo_sub
   ********************************************************************/
   extern const bclass be_class_demo;
   be_local_class(demo_sub,
       1,
       &be_class_demo,
       be_nested_map(2,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(init, -1), be_const_closure(demo_sub_init_closure) },
           { be_const_key(j, 0), be_const_var(0) },
       })),
       be_str_literal("demo_sub")
   );
   /*******************************************************************/

   void be_load_demo_sub_class(bvm *vm) {
       be_pushntvclass(vm, &be_class_demo_sub);
       be_setglobal(vm, "demo_sub");
       be_pop(vm, 1);
   }

Solidificación de módulos
~~~~~~~~~~~~~~~~~~~~~~~~~

Cuando solidifica un módulo, incrusta todos los subelementos. También
funciona con listas o mapas incrustados.

.. code:: berry

   > def say_hello() print("Hello Berry!") end
   > m = module("demo_module")
   > m.i = 0
   > m.s = "foo"
   > m.f = say_hello
   > m.l = [0,1,"a"]
   > m.m = {"a":"b", "2":3}
   > import solidify
   > solidify.dump(m)

.. code:: c

   /********************************************************************
   ** Solidified function: say_hello
   ********************************************************************/
   be_local_closure(demo_module_say_hello,   /* name */
     be_nested_proto(
       2,                          /* nstack */
       0,                          /* argc */
       0,                          /* varg */
       0,                          /* has upvals */
       NULL,                       /* no upvals */
       0,                          /* has sup protos */
       NULL,                       /* no sub protos */
       1,                          /* has constants */
       ( &(const bvalue[ 1]) {     /* constants */
       /* K0   */  be_nested_str(Hello_X20Berry_X21),
       }),
       &be_const_str_say_hello,
       &be_const_str_solidified,
       ( &(const binstruction[ 4]) {  /* code */
         0x60000001,  //  0000  GETGBL R0  G1
         0x58040000,  //  0001  LDCONST    R1  K0
         0x7C000200,  //  0002  CALL   R0  1
         0x80000000,  //  0003  RET    0
       })
     )
   );
   /*******************************************************************/

   /********************************************************************
   ** Solidified module: demo_module
   ********************************************************************/
   be_local_module(demo_module,
       "demo_module",
       be_nested_map(5,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(l, -1), be_const_simple_instance(be_nested_simple_instance(&be_class_list, {
           be_const_list( *     be_nested_list(3,
       ( (struct bvalue*) &(const bvalue[]) {
           be_const_int(0),
           be_const_int(1),
           be_nested_str(a),
       }))    ) } )) },
           { be_const_key(m, 3), be_const_simple_instance(be_nested_simple_instance(&be_class_map, {
           be_const_map( *     be_nested_map(2,
       ( (struct bmapnode*) &(const bmapnode[]) {
           { be_const_key(a, -1), be_nested_str(b) },
           { be_const_key(2, -1), be_const_int(3) },
       }))    ) } )) },
           { be_const_key(i, 4), be_const_int(0) },
           { be_const_key(f, -1), be_const_closure(demo_module_say_hello_closure) },
           { be_const_key(s, -1), be_nested_str(foo) },
       }))
   );
   BE_EXPORT_VARIABLE be_define_const_native_module(demo_module);
   /********************************************************************/

limitaciones de la solidificación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La solidificación funciona para muchos objetos: ``clase``, ``módulo``,
``funciones`` y constantes incrustadas u objetos como ``int``, ``real``,
``string``, ``list`` y ``map``.

Limitaciones:

-  Los upvals no son compatibles. No puede solidificar un cierre que
   captura upvals del alcance externo
-  La captura de variables globales requiere compilar con la opción
   ``-g`` “globales con nombre” (habilitada de forma predeterminada en
   Tasmota)
-  Las constantes de cadena están limitadas a 255 bytes, cadenas largas
   (más de 255 caracteres no son compatibles, porque nadie nunca los
   necesitó)
-  Los objetos solidificados son de solo lectura, esto tiene algunas
   consecuencias en las clases. Puede solidificar una clase con sus
   miembros estáticos cuando se crea, pero no puede solidificar una
   función que crea una clase derivada de otra clase o con miembros
   estáticos. La razón principal es que la configuración de la
   superclase o la asignación de miembros estáticos se implementa
   mediante el código mutante en la nueva clase, que no puede funcionar
   en una clase no mutante de solo lectura.
-  El código solidificado puede depender del tamaño de “int” y “real” y
   es posible que no se transfiera a través de MCU con tipos de
   diferentes tamaños. Es posible que deba volver a solidificar para
   cada objetivo.
