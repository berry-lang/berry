9. FFI
======

La **Interfaz de Función Externa** (FFI) es una interfaz para la
interacción entre diferentes lenguajes. Berry proporciona un conjunto de
FFI para realizar la interacción con el lenguaje C, este conjunto de
interfaces también es muy fácil de usar en C++. La mayoría de las
interfaces FFI son funciones y sus declaraciones se colocan en el
archivo *berry.h*. Para reducir la cantidad de RAM utilizada, FFI
también proporciona un mecanismo para generar una tabla hash fija
durante la compilación de C. Este mecanismo debe utilizar herramientas
externas para generar código C.

9.1 Conceptos básicos
---------------------

La función interactiva más importante en FFI debería ser la función de
llamar al código Berry y la función C mutuamente. Para darnos cuenta de
cómo dos lenguajes llaman a las funciones del otro, primero debemos
entender el mecanismo de paso de parámetros de la función de Berry.

9.1.1 Máquina virtual
~~~~~~~~~~~~~~~~~~~~~

A diferencia de los lenguajes compilados, el lenguaje Berry no puede
ejecutarse directamente en una máquina física, sino en un entorno de
software específico, que es **Máquina virtual** (VM). Similar a una
computadora real, el código fuente en forma de texto no se puede
ejecutar en una máquina virtual, sino que un compilador debe convertirlo
en “código de bytes”. La máquina virtual Berry se define como una
estructura C ``bvm``, el contenido de esta estructura es invisible para
FFI. A través de algunas funciones de FFI, podemos crear e inicializar
una máquina virtual. Introduciremos el uso de máquinas virtuales a
través de un ejemplo sencillo:

.. code:: c

   void berry_test(void)
   {
       bvm *vm = be_vm_new(); // Construir una VM
       be_loadstring(vm, "print('Hola Berry')"); // Compilar código de prueba
       be_pcall(vm, 0); // Función de llamada
       be_vm_delete(vm); // Destruir la VM
   }

Este código da un ejemplo completo del uso de una máquina virtual.
Primero, se llama a la función ``be_vm_new`` para construir una nueva
máquina virtual, y luego todas las operaciones se completan en este
objeto de máquina virtual. La función ``be_vm_new`` vinculará
automáticamente la biblioteca estándar al crear una máquina virtual. La
función de las líneas 4 a 5 es compilar el código fuente de una cadena
en una función Berry y luego llamarla. Finalmente, se llama a la función
``be_vm_delete`` en la línea 6 para destruir la máquina virtual. Al
ejecutar esta función obtendrá una línea de salida en la terminal:

``Hola Berry``

En todos los escenarios, la construcción de la máquina virtual, la carga
de la biblioteca y el proceso de destrucción son los mismos que en las
líneas 3, 4 y 6 del ejemplo anterior. Si es necesario, la forma de
compilar o cargar el código fuente puede ser diferente. Por ejemplo,
para el código fuente en forma de archivo, se puede compilar a través de
la función ``be_loadfile``. El código fuente se compilará en una función
Berry y la función se almacenará en la parte superior de la pila. La
función Berry se puede ejecutar llamando a la función FFI ``be_pcall`` o
``be_call``. También puede usar REPL a través de la función ``be_repl``.
La interfaz del REPL se describirá en los capítulos correspondientes.

9.1.2 Pila virtual
~~~~~~~~~~~~~~~~~~

Berry usa una pila virtual y funciones nativas escritas en C para pasar
valores. Cada elemento de la pila es un valor Berry. Cuando el código
Berry llama a una función nativa, siempre crea una nueva pila y empuja
todos los parámetros a la pila. Esta pila virtual también se puede usar
en código C para almacenar datos, y el recolector de elementos no
utilizados no reclamará el valor almacenado en la pila.

.. figure:: https://user-images.githubusercontent.com/49731213/135971121-059e665c-7f65-4a6a-bbe4-da9acf4cf8b5.png
   :alt: Virtual_Stack

   Virtual_Stack

La pila virtual utilizada por Berry se muestra en la figura anterior.

La pila virtual crece de izquierda a derecha. Cuando el código Berry
llama a una función nativa, obtendrá una pila inicial. La posición del
primer valor de la pila se llama **base**, y la última posición se llama
**superior** (top), en la función nativa solo el valor desde la parte
inferior de la pila hasta la posición anterior a la parte superior de la
pila puede ser accesible. La posición de la parte inferior de la pila es
fija, mientras que la posición de la parte superior de la pila se puede
mover y la parte superior de la pila siempre está vacía. El motivo de
esta propiedad es que después de insertar el nuevo valor en la pila
virtual, en la posición original de la parte superior de la pila se
escribirá el nuevo valor, el puntero superior de la pila avanzará a la
siguiente posición; por el contrario, si se extrae el valor en la parte
superior de la pila virtual, el puntero superior de la pila disminuirá
de 1. En este momento, aunque la posición del puntero superior de la
pila es objetivamente un valor, este no es válido y se puede borrar en
cualquier momento, por lo que la posición del puntero en la parte
superior de la pila aún está vacía. Cuando la pila virtual está vacía,
el puntero inferior ``base`` es igual al puntero superior ``top``. La
pila virtual no sigue estrictamente las reglas de funcionamiento de la
pila: además de empujar y sacar, también se puede acceder a la pila
virtual por índice, e incluso insertar o eliminar valores en cualquier
posición. Hay dos formas de indexar elementos en la pila: una se basa en
la parte inferior de la pila **Índice absoluto**, el valor del índice
absoluto es un número entero positivo a partir de 1; el otro se basa en
la parte superior de la pila **Índice relativo**, el valor del índice
relativo es un número entero negativo a partir de − 1. Tome la Figura
anterior como ejemplo, el valor de índice 1, 2…8 es un índice absoluto,
y el índice absoluto de un elemento es la distancia desde el elemento
hasta el final de la pila. El valor de índice − 1, − 2… − 8 es un índice
relativo, y el valor de índice relativo de un elemento es el número
negativo de la distancia desde el elemento hasta la parte superior de la
pila. Si un valor de índice *index* es válido, entonces el elemento al
que se refiere debe estar entre la parte inferior de la pila y la parte
superior de la pila, lo que significa se cumple que la expresión:

``1 ≤ abs(*index*) ≤ *top* − *base* + 1``.

Por conveniencia, estipulamos que el puntero inferior de la pila ‘base’
se usa como referencia, y su índice absoluto 1, y el valor anterior de
‘base’ no se considera (por lo general, ‘base’ no es la posición
inferior de todo el pila). Por ejemplo, cuando regresa una función
nativa, la ubicación donde se almacena el valor de retorno está justo
antes de ``base``, y la función nativa no suele acceder a estas
ubicaciones.

Operar con pila virtual
~~~~~~~~~~~~~~~~~~~~~~~

Índice y tamaño de pila
^^^^^^^^^^^^^^^^^^^^^^^

Como se mencionó anteriormente, se pueden usar dos métodos de indexación
para acceder a la pila virtual y el valor del índice debe ser válido. Al
mismo tiempo, en muchos casos también es necesario introducir nuevos
valores en la pila. En este caso, el programador debe asegurarse de que
la pila no se desborde. Por defecto, Berry garantiza el espacio
``BE_STACK_FREE_MIN`` para que lo usen las funciones nativas. Este valor
se puede modificar en el archivo *berry.h*. Su valor por defecto suele
ser 10, que debería ser suficiente en la mayoría de los casos. Si
realmente necesita expandir la pila, puede llamar a la función FFI
``be_stack_require``. El prototipo de esta función es:

.. code:: c

   void be_stack_require(bvm *vm, int count);

El parámetro ``count`` es la cantidad de espacio necesario. Cuando el
espacio restante en la pila virtual sea insuficiente, la capacidad de la
pila se expandirá; de lo contrario, esta función no hará nada.

**Advertencia**: si se produce un desbordamiento de la pila, o si se
utiliza un índice no válido para acceder a la pila, el programa fallará.
Puede activar el interruptor de depuración ``BE_DEBUG`` (sección
[sección::BE_DEBUG] ), que activará la función de aserción, y puede
obtener información de depuración en tiempo de ejecución para detectar
errores como desbordamiento de pila o índice no válido.

Obtener valor de la pila
^^^^^^^^^^^^^^^^^^^^^^^^

Hay un conjunto de funciones en FFI para obtener valores de la pila
virtual. Estas funciones generalmente convierten los valores en la pila
en valores simples compatibles con el lenguaje C y luego regresan. Los
siguientes son FFI de uso común para obtener valores de la pila:

.. code:: c

   bint be_toint(bvm *vm, int index);
   breal be_toreal(bvm *vm, int index);
   int be_tobool(bvm *vm, int index);
   const char* be_tostring(bvm *vm, int index);
   void* be_tocomptr(bvm *vm, int index);

La forma de parámetro de estas funciones es la misma, pero el valor de
retorno es diferente. Las primeras cuatro funciones son fáciles de
entender. Al igual que sus nombres, la función de ``be_toint`` es
convertir los valores en la pila virtual a valores enteros de C
(``bint`` suele ser un alias de tipo ``int``) y devolverlos. La función
de la última función ``be_tocomptr`` es sacar un valor de puntero de
tipo general de la pila virtual. El significado específico de este
puntero se explica por el propio programa C.

Estas funciones utilizan la misma forma de interpretar los parámetros:
el parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del elemento a recuperar, que puede ser un índice
relativo o un índice absoluto. No puede usar FFI para eliminar los tipos
de datos complejos de Berry de la pila virtual, por lo que no puede
eliminar un tipo de “map” o un tipo de “class” de la pila. Uno de los
beneficios de este diseño es que no es necesario considerar la
recolección de elementos no utilizados en las funciones nativas.

Función nativa
~~~~~~~~~~~~~~

Una **Función nativa** está implementada por lenguaje C y puede ser
llamada por código Berry. La función nativa puede ser una función
ordinaria. En este caso, llamar a la función nativa no generará ningún
espacio asignado dinámicamente, al igual que una llamada de función C
normal. Las funciones nativas también pueden ser cierres, y se debe
asignar espacio para variables libres al crear cierres nativos. En
circunstancias normales, las funciones nativas simples son suficientes
para satisfacer las necesidades. Ahorran más recursos que los cierres
nativos y son más fáciles de usar.

Definir una función nativa
^^^^^^^^^^^^^^^^^^^^^^^^^^

La función nativa en sí es una función C, pero todas tienen una forma
específica. La definición de la función nativa es:

.. code:: c

   int a_native_function(bvm *vm)
   {
       // hacer algo ...
   }

La función nativa debe ser una función C cuyo parámetro sea un puntero a
``bvm`` y el valor de retorno sea ``int``. Las funciones de Berry deben
devolver un valor y las funciones nativas no son una excepción. A
diferencia del valor de retorno del lenguaje C, el valor de retorno de
la función nativa no es el valor transportado por la instrucción C
``return``. Puede usar estos FFI para devolver el valor de la función
nativa, y también hacen que la función C devuelva:

.. code:: c

   be_return(bvm *vm);
   be_return_nil(bvm *vm);

Estos FFI son en realidad dos macros, y no es necesario usar la
instrucción C ``return`` al usarlos. ``be_return`` pondrá la parte
superior de la pila virtual

Usar una función nativa
^^^^^^^^^^^^^^^^^^^^^^^

Después de definir la función nativa, debe agregarse al intérprete de
alguna manera antes de que pueda llamarse en código Berry. Una de las
formas más sencillas es agregar funciones nativas a la tabla de objetos
integrada de Berry. El proceso de configuración de objetos nativos como
objetos incorporados de Berry se denomina **registración**. El FFI de la
función nativa registrada es:

.. code:: c

   void be_regfunc(bvm *vm, const char *nombre, bntvfunc f);

``vm`` es la instancia actual de la máquina virtual, ``nombre`` es el
nombre de la función nativa y ``f`` es el puntero de la función nativa.
El comportamiento específico de esta función está relacionado con el
valor de la macro ``BE_USE_PRECOMPILED_OBJECT`` (aunque el FFI todavía
está disponible cuando se utiliza la técnica de construcción en tiempo
de compilación, no puede registrar dinámicamente las variables
integradas. En este caso, consulte el método de registro de los objetos
integrados. 1.3 ). La definición del tipo de función nativa ``bntvfunc``
es:

.. code:: c

   typedef int (*bntvfunc)(bvm*);

De hecho, el tipo ``bntvfunc`` es el tipo de puntero de función con el
parámetro ``bvm`` y el tipo de valor devuelto ``int``. La función
``be_regfunc`` debe llamarse antes de analizar el código fuente de
Berry.

También puede insertar la función nativa en la pila virtual y luego usar
una función FFI ``be_call`` para llamarla. Un uso más común es usar el
objeto de función nativa en la pila virtual como valor de retorno.

Ejemplo completo
^^^^^^^^^^^^^^^^

Finalizamos esta sección con un ejemplo sencillo. Aquí, vamos a
implementar una función ``add`` que suma dos números y devuelve el
resultado del cálculo. Primero, definimos una función nativa para
implementar esta función:

.. code:: c

   static int l_add(bvm *vm)
   {
       int top = be_top(vm); // Obtener el número de argumentos
       /* Verificar el número y tipo de argumentos */
       if (top == 2 && be_isnumber(vm, 1) && be_isnumber(vm, 2)) {
           breal x = be_toreal(vm, 1); // Obtener el primer argumento
           breal y = be_toreal(vm, 2); // Obtener el segundo argumento
           be_pushreal(vm, x + y); // Empuje el resultado a la pila
           be_return(vm); // Devuelve el valor en la parte superior de la pila
       }
       be_return_nil(vm); // Devuelve nil cuando algo sale mal
   }

Por lo general, las funciones nativas no necesitan usarse fuera del
archivo C, por lo que generalmente se declaran como tipos ``estáticos``.
Utilice la función ``be_top`` para obtener el índice absoluto de la
parte superior de la pila virtual (valor ``top``), que es la capacidad
de la pila. Podemos llamar a ``be_top`` antes de que la función nativa
realice la operación de pila virtual, en este momento la capacidad de la
pila virtual es igual a la cantidad de parámetros reales. Para la
función ``add``, necesitamos dos parámetros para participar en la
operación, así que verifica si el número de parámetros es 2 en la cuarta
línea (``top == 2``). Y necesitamos verificar si los dos parámetros son
de tipo numérico, por lo que debemos llamar a la función ``be_isnumber``
para verificar. Si todo es correcto, los parámetros se sacarán de la
pila virtual, luego el resultado del cálculo se colocará en la pila y
finalmente se devolverá usando ``be_return``. Si la verificación del
parámetro falla, se llamará a ``be_return_nil`` para devolver el valor
de ``nil``.

A continuación, registre esta función nativa en la tabla de objetos
integrada. Para simplificar, lo registramos después de cargar la
biblioteca:

.. code:: c

   bvm *vm = be_vm_new(); // Construir una VM
   be_regfunc(vm, "myadd", l_add); // Registrar la función nativa "myadd"

La segunda línea es donde se registra la función nativa y la llamamos
``myadd``. En este punto, la definición y el registro de la función
nativa están completos. Como verificación, puede compilar el intérprete,
luego ingresar el REPL y ejecutar algunas pruebas. Debería obtener
resultados como este:

.. code:: berry

   > myadd
   <function: 0x562a210f0f90>
   > myadd(1.0, 2.5)
   3.5
   > myadd(2.5, 2)
   4.5
   > myadd(1, 2)
   3

Tipos y Funciones
-----------------

Tipos
~~~~~

Esta sección presentará algunos tipos que deben usarse en FFI y son
generalmente utilizados por funciones FFI. Generalmente, los tipos y
declaraciones en FFI se pueden encontrar en el archivo *berry.h*. A
menos que se especifique lo contrario en esta sección, la definición o
declaración se proporciona en *berry.h* de forma predeterminada.

El tipo ``bvm`` se utiliza para almacenar la información de estado de la
máquina virtual Berry. Los detalles de este tipo no son visibles para
los programas externos. Por lo tanto, esta definición solo se puede
encontrar en el archivo *berry.h*:

.. code:: c

   typedef struct bvm bvm;

La mayoría de las funciones de FFI usan el tipo ``bvm`` como primer
parámetro, porque todas operan en la máquina virtual internamente.
Ocultar la implementación interna de ``bvm`` ayuda a reducir el
acoplamiento entre el estándar FFI y la VM. Fuera del intérprete,
normalmente solo se utilizan punteros ``bvm``. Para crear un nuevo
objeto ``bvm``, use la función ``be_vm_new`` y destruya el objeto
``bvm`` usando la función ``be_vm_delete``.

La definición del tipo de función nativa es:

.. code:: c

   typedef int (*bntvfunc)(bvm*);

Este tipo es un puntero de función nativo y algunas FFI que registran o
agregan funciones nativas a la máquina virtual usan parámetros de este
tipo. Las variables o parámetros de este tipo deben inicializarse con un
nombre de función cuyo parámetro sea del tipo ``bvm`` y cuyo valor de
retorno sea del tipo ``int``.

Este tipo se usa cuando se registran funciones nativas en lotes o se
construyen clases nativas. Se define como:

.. code:: c

   typedef struct {
       const char *nombre; // El nombre de la función u objeto
       bntvfunc funcion; // El puntero de función
   } bnfuncinfo;

El miembro ``nombre`` de ``bnfuncinfo`` representa el nombre de una
función u objeto, y el miembro ``funcion`` es un puntero de función
nativo.

Este tipo es un tipo entero integrado de Berry. Se define en el
documento *berry.h*. Por defecto, ``bint`` se implementa usando el tipo
``long long``, y la implementación de ``bint`` se puede modificar
cambiando el archivo de configuración.

Este es el tipo de número real incorporado de Berry, que en realidad es
el tipo de punto flotante en lenguaje C. ``breal`` se define como:

.. code:: c

   #if BE_SINGLE_FLOAT != 0
       typedef float breal;
   #else
       typedef double breal;
   #endif

Puede usar la macro ``BE_SINGLE_FLOAT`` para controlar la implementación
específica de ``breal``: cuando el valor de ``BE_SINGLE_FLOAT`` es
``0``, se usará la implementación de tipo ``doble`` ``breal``, de lo
contrario, la implementación de tipo ``float`` se utilizará para
``breal``.

[sección::código de error]

Este tipo de enumeración se utiliza en algunos valores de retorno de
FFI. La definición de este tipo es:

.. code:: c

   enum berrorcode {
       BE_OK = 0,
       BE_IO_ERROR,
       BE_SYNTAX_ERROR,
       BE_EXEC_ERROR,
       BE_MALLOC_FAIL,
       BE_EXIT
   };

El significado de estos valores de enumeración son:

-  ``BE_OK``: No hay ningún error, la función se ejecuta con éxito.

-  ``BE_IO_ERROR``: Ocurrió un error de lectura de archivo cuando el
   intérprete estaba leyendo el archivo fuente. El error generalmente es
   causado por el ausencia del expediente.

-  ``BE_SYNTAX_ERROR``: Ocurrió un error de sintaxis cuando el
   intérprete estaba compilando el código fuente. Después de que ocurre
   este error, el intérprete no generará bytecode, por lo que no puede
   continuar ejecutándose el código de bytes.

-  ``BE_EXEC_ERROR``: Error de tiempo de ejecución. Cuando se produce
   este error, la ejecución de El código Berry se detiene y el entorno
   se restaura al máximo punto de recuperación reciente.

-  ``BE_MALLOC_FAIL``: Falló la asignación de memoria. Este error es
   causado por espacio de pila insuficiente.

-  ``BE_EXIT``: Indica que el programa sale y el valor no es un error.
   Ejecutar la función ``exit`` de Berry hace que el intérprete devuelva
   este valor.

Cabe señalar que cuando se produce un error ``BE_MALLOC_FAIL``, ya no se
puede realizar la asignación de memoria dinámica, lo que significa que
ya no se pueden asignar objetos de cadena, por lo que la función que
devuelve este error generalmente no brinda información más detallada
sobre el error.

Funciones y Macros
~~~~~~~~~~~~~~~~~~

Esta función se utiliza para crear una nueva instancia de máquina
virtual. El prototipo de función es:

.. code:: c

   bvm* be_vm_new(void);

El valor de retorno de la función es un puntero a la instancia de la
máquina virtual. ``be_vm_new`` es la primera función llamada cuando se
crea el intérprete de Berry. Esta función hará mucho trabajo: solicitar
memoria para la máquina virtual, inicializar el estado y los atributos
de la máquina virtual, inicializar el GC (recolector de basura), la
biblioteca estándar se carga en la instancia de la máquina virtual, etc.

La función ``be_vm_delete`` se usa para destruir una instancia de
máquina virtual. El prototipo de la función es:

.. code:: c

   void be_vm_delete(bvm *vm);

El parámetro ``vm`` es el puntero del objeto de la máquina virtual que
se va a destruir. La destrucción de la máquina virtual liberará todos
los objetos de la máquina virtual, incluidos los valores de la pila y
los objetos administrados por el GC. El puntero de la máquina virtual
después de la destrucción será un valor no válido y ya no se podrá hacer
referencia a él.

Esta función se utiliza para cargar un fragmento de código fuente del
búfer y compilarlo en un código de bytes. El prototipo de la función es:

.. code:: c

   int be_loadbuffer(bvm *vm, const char *name, const char *buffer, size_t length);

El parámetro ``vm`` es el puntero de la máquina virtual. ``name`` es una
cadena, que generalmente se usa para marcar la fuente del código fuente.
Por ejemplo, la entrada del código fuente del dispositivo de entrada
estándar puede pasar la cadena ``"stdin"`` a este parámetro, y la
entrada del código fuente del archivo puede ser el nombre del archivo y
se pasa a este parámetro. El parámetro ``buffer`` es el búfer para
almacenar el código fuente. La organización de este búfer es muy similar
a la cadena de C. Es una secuencia continua de caracteres, pero el búfer
al que apunta ``buffer`` no requiere caracteres ``'\0'`` como
terminador. El parámetro ``longitud`` indica la longitud del búfer. Esta
longitud se refiere al número de bytes de texto de código fuente en el
búfer.

Para dar un ejemplo simple, si queremos usar la función
``be_loadbuffer`` para compilar una cadena, el uso general es:

.. code:: c

   const char *str = "print('Hola Berry')";
   be_loadbuffer(vm, "cadena", str, strlen(str));

Aquí usamos la cadena ``"cadena"`` para representar el código fuente,
también puede modificarla a cualquier valor. Tenga en cuenta que la
función ``strlen`` de la función de biblioteca estándar de C se usa aquí
para obtener la longitud del búfer de cadena (en realidad, el número de
bytes en la cadena).

Si la compilación es exitosa, ``be_loadbuffer`` compilará el código
fuente en una función Berry y lo colocará en la parte superior de la
pila virtual. Si la compilación encuentra un error, ``be_loadbuffer``
devolverá un valor de error de tipo ``berrorcode`` [ver Sección código
de error] y, si es posible, almacenará la cadena de mensaje de error
específica en la parte superior de la pila virtual.

``be_loadstring`` es una macro definida como:

.. code:: c

   #define be_loadstring(vm, str) be_loadbuffer((vm), "string", (str), strlen(str))

Esta macro es solo un contenedor simple para la función
``be_loadbuffer``. El parámetro ``vm`` es un puntero a la instancia de
la máquina virtual, y el parámetro ``str`` es un puntero a la cadena de
código fuente. Es muy conveniente usar ``be_loadstring`` para compilar
cadenas, por ejemplo:

.. code:: c

   be_loadstring(vm, "print('Hola Berry')");

Esta forma de escribir es más concisa que usar ``be_loadbuffer``, pero
debe asegurarse de que la cadena termine con un carácter ``'\0'``.

Esta función se utiliza para compilar un archivo de código fuente. El
prototipo de función es:

.. code:: c

   int be_loadfile(bvm *vm, const char *nombre);

La funcionalidad de esta función es similar a la función
``be_loadbuffer``, excepto que la función se compilará leyendo el
archivo de código fuente. El parámetro ``vm`` es el puntero de la
instancia de la máquina virtual y el parámetro ``nombre`` es el nombre
del archivo de origen. Esta función llamará a la interfaz de archivo y,
de forma predeterminada, utilizará funciones como ``fopen`` en la
biblioteca estándar de C para manipular archivos.

Si usa la interfaz de archivo de la biblioteca estándar de C, puede usar
nombres de archivo de ruta relativa o ruta absoluta. Si el archivo no
existe, ``be_loadfile`` devolverá un error ``BE_IO_ERROR`` (Ve Seccion
de código de error) y colocará el mensaje de error en la parte superior
de la pila. Otros mensajes de error son los mismos que los de la función
``be_loadbuffer``. Se recomienda usar la función ``be_loadfile`` para
compilar el archivo fuente, en lugar de leer todos los archivos fuente
en un búfer, y luego llamar a la función ``be_loadbuffer`` para compilar
el código fuente. El primero leerá el archivo fuente en segmentos y solo
creará un pequeño búfer de lectura en la memoria, ahorrando así más
memoria.

La función ``be_top`` devuelve el valor de índice absoluto del elemento
superior en la pila virtual. Este valor es también el número de
elementos en la pila virtual (la capacidad de la pila virtual). Llame a
esta función antes de agregar o quitar elementos en la pila virtual para
obtener la cantidad de parámetros de la función nativa. El prototipo de
esta función es:

.. code:: c

   int be_top(bvm *vm);

Esta función se suele utilizar para obtener el número de parámetros de
una función nativa. Cuando se usa para este propósito, se recomienda
llamar a ``be_top`` en la parte superior del cuerpo de la función
nativa. P.ej:

.. code:: c

   static int native_function_example(bvm *vm)
   {
       int argc = be_top(vm); // Obtener el número de argumentos
       // ...
   }

La función ``be_typename`` convierte el tipo del objeto Berry en una
cadena y lo devuelve. Por ejemplo, devuelve ``"int"`` para un objeto
entero y ``"function"`` para un objeto función. El prototipo de esta
función es:

.. code:: c

   const char* be_typename(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del objeto a operar. La función ``type`` en la
biblioteca estándar de Berry se implementa llamando a ``be_typename``.
Consulte la sección ``baselib_type`` para conocer la cadena de retorno
correspondiente al tipo de parámetro.

La función ``be_classname`` se utiliza para obtener el nombre de clase
de un objeto o clase. El prototipo de función es:

.. code:: c

   const char* be_classname(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del objeto a operar. Si el valor en ``index``
es una instancia, la función ``be_classname`` devolverá la cadena del
nombre de la clase a la que pertenece la instancia, y si el valor en
``index`` es una clase, devolverá directamente la cadena del nombre de
la clase. En otros casos ``be_classname`` devolverá ``NULL``.

La función ``be_strlen`` devuelve la longitud de la cadena Berry
especificada. El prototipo de función es:

.. code:: c

   int be_strlen(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del objeto a operar. Esta función devuelve el
número de bytes en la cadena en ``index`` (los caracteres ``'\0'`` al
final de la cadena Berry no se cuentan). Si el valor de la posición
``index`` no es una cadena, la función ``be_strlen`` devolverá ``0``.

Aunque la cadena ``Berry`` es compatible con el formato de cadena C, no
se recomienda utilizar la función ``strlen`` de la biblioteca estándar
de C para medir la longitud de la cadena Berry. Para cadenas Berry,
``be_strlen`` es más rápido que ``strlen`` y tiene mejor compatibilidad.

La función ``be_strconcat`` se utiliza para empalmar dos cadenas Berry.
El prototipo de función es:

.. code:: c

   void be_strconcat(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual.
Esta función concatenará la cadena en la posición del parámetro de
``index`` con la cadena en la posición superior de la pila, y luego
colocará la cadena resultante en la posición indexada por ``index``.

La función ``be_pop`` extrae el valor en la parte superior de la pila.
El prototipo de función es:

.. code:: c

   void be_pop(bvm *vm, int n);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
y el parámetro ``n`` es la cantidad de valores que se extraerán de la
pila. Tenga en cuenta que el valor de ``n`` no puede exceder la
capacidad de la pila.

La función ``be_remove`` elimina un valor de la pila. Esta función
eliminará un valor de la pila.

.. code:: c

   void be_remove(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual
y el parámetro ``index`` es el índice del objeto que se eliminará.
Después de que el valor en ``index`` se mueva, los siguientes valores se
completarán y la capacidad de la pila se reducirá en uno. El valor de
``index`` no puede exceder la capacidad de la pila.

La función ``be_absindex`` devuelve el valor de índice absoluto de un
valor de índice dado, y su prototipo de función es:

.. code:: c

   int be_absindex(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual
y el parámetro ``index`` es el valor del índice de entrada. Si ``index``
es positivo, el valor de retorno de ``be_absindex`` es el valor de
``index``. Si ``index`` es negativo, el valor de retorno de be_absindex
es el valor de índice absoluto correspondiente a ``index``. Cuando
``index`` es un valor negativo (índice relativo), su posición de índice
no puede ser inferior a la parte inferior de la pila.

La función ``be_newlist`` crea un nuevo valor de ``list``, y su
prototipo de función es:

.. code:: c

   void be_newlist(bvm *vm);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual.
Después de llamar con éxito a esta función, el nuevo valor de ``list``
se colocará en la parte superior de la pila. El valor ``list`` es una
representación interna de una lista, que no debe confundirse con una
instancia de la clase ``list``.

La función ``be_newmap`` crea un nuevo valor ``map``, y su prototipo de
función es:

.. code:: c

   void be_newmap(bvm *vm);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual.
Después de llamar con éxito a esta función, el nuevo valor del ``map``
se colocará en la parte superior de la pila. El valor ``map`` es una
representación interna de una lista, que no debe confundirse con una
instancia de la clase ``map``.

La función ``be_getglobal`` empuja la variable global con el nombre
especificado a la pila. Su prototipo de función es:

.. code:: c

   void be_getglobal(bvm *vm, const char *name);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual
y el parámetro ``name`` es el nombre de la variable global. Después de
llamar a esta función, la variable global llamada ``name`` se colocará
en la parte superior de la pila virtual.

La función ``be_setmember`` se utiliza para establecer el valor de la
variable miembro de la clase de objeto de instancia. El prototipo de
función es:

.. code:: c

   void be_setmember(bvm *vm, int index, const char *k);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
el parámetro ``index`` es el índice del objeto de la instancia y el
parámetro ``k`` es el nombre del miembro. Esta función copiará el valor
en la parte superior de la pila al miembro ``k`` de la instancia de
posición de índice. Tenga en cuenta que el elemento superior de la pila
no aparecerá automáticamente.

La función ``be_getmember`` se utiliza para obtener el valor de la
variable miembro de la clase de objeto de instancia. El prototipo de
función es:

.. code:: c

   void be_getmember(bvm *vm, int index, const char *k);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
el parámetro ``index`` es el índice del objeto de la instancia y el
parámetro ``k`` es el nombre del miembro. Esta función coloca el valor
del miembro de la instancia de posición de índice ``k`` en la parte
superior de la pila virtual.

La función ``be_getindex`` se utiliza para obtener el valor de ``list``
o ``map``. El prototipo de función es:

.. code:: c

   void be_getindex(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
y el parámetro ``index`` es el índice del objeto a operar. Esta función
se usa para obtener un elemento del contenedor ``map`` o ``list``
(valores internos, no instancias de las clases ``map`` o ``list``), y el
índice del elemento se almacena en la parte superior de la pila (el
índice relativo es -1). Después de llamar a esta función, el valor
obtenido del contenedor se colocará en la parte superior de la pila. Si
no hay ningún subíndice señalado por el contenedor, el valor ``nil`` se
colocará en la parte superior de la pila. Por ejemplo, si el elemento
con el índice 1 en la pila virtual es una ``list`` y queremos extraer el
elemento con el índice 0, entonces podemos usar el siguiente código:

.. code:: c

   be_pushint(vm, 0); //  Inserte el valor de índice 0 en la pila virtual
   be_getindex(vm, 1); //  Obtener un elemento del contenedor de lista

Primero colocamos el valor entero ``0`` en la pila, y este valor se
usará como índice para obtener el elemento del contenedor ``list``. La
segunda línea de código implementa para obtener elementos del contenedor
``list``. El valor de índice del contenedor ``list`` en el ejemplo es 1
en la pila virtual. El elemento recuperado se almacena en la parte
superior de la pila y podemos usar el índice relativo -1 para acceder a
él.

La función ``be_setindex`` se utiliza para establecer un valor en
``list`` o ``map``. El prototipo de función es:

.. code:: c

   void be_setindex(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
y el parámetro ``index`` es el subíndice del objeto a operar. Esta
función se utiliza para escribir un elemento del contenedor ``map`` o
``list``. El índice del valor que se va a escribir en la pila virtual es
-1, y el índice del subíndice de la posición de escritura en la pila
virtual es -2. Si el elemento con el subíndice especificado no existe en
el contenedor, la operación de escritura fallará.

Suponiendo que la posición con el índice ``1`` en la pila virtual tiene
un valor de ``map``, y tiene un elemento con un subíndice de
``"prueba"``, un ejemplo de configuración del elemento en el subíndice
de ``"prueba"`` a ``100`` es:

.. code:: c

   be_pushstring(vm, "prueba"); // Empuja el índice "índice"
   be_pushint(vm, 100);         // Empuja el valor 100
   be_setindex(vm, 1);          // Establece el par clave-valor a map["prueba"] = 100

Primero debemos empujar el subíndice y el valor que se escribirá en la
pila en orden. Para ``map``, es un par clave-valor. En el ejemplo, las
dos primeras líneas de código completan estas tareas. La tercera línea
llama a la función ``be_setindex`` para escribir el valor en el objeto
``map``.

La función ``be_getupval`` se utiliza para leer un valor ascendente del
cierre nativo. El prototipo de función es:

.. code:: c

   void be_getupval(bvm *vm, int index, int pos);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el valor de índice de cierre nativo del valor ascendente
que se va a leer; ``pos`` es la posición del upvalue en la tabla upvalue
de cierre nativa (la numeración comienza desde 0). El valor leído se
colocará en la parte superior de la pila virtual.

La función ``be_setupval`` se utiliza para establecer un valor superior
del cierre nativo. El prototipo de función es:

.. code:: c

   void be_setupval(bvm *vm, int index, int pos);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el valor del índice de cierre nativo que se escribirá en
upvalue; ``pos`` es la posición del upvalue en la tabla upvalue de
cierre nativa (la numeración comienza desde 0). Esta función obtiene un
valor de la parte superior de la pila virtual y lo escribe en el valor
superior de destino. Una vez completada la operación, el valor superior
de la pila no se extraerá de la pila.

La función ``be_getsuper`` se utiliza para obtener el objeto principal
de la clase base o la instancia de la clase. El prototipo de función es:

.. code:: c

   void be_getsuper(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es la clase u objeto a operar. Si el valor en ``index`` es una
clase con una clase base, la función colocará su clase base en la parte
superior de la pila; si el valor en ``index`` es un objeto con un objeto
padre, la función tomará su padre. El objeto se coloca en la parte
superior de la pila; de lo contrario, se coloca un valor de ``nil`` en
la parte superior de la pila.

La función ``be_data_size`` se utiliza para obtener el número de
elementos contenidos en el contenedor. El prototipo de función es:

.. code:: c

   int be_data_size(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto contenedor que se va a operar. Si el
valor en ``index`` es un valor Map o List, la función devuelve el número
de elementos contenidos en el contenedor; de lo contrario, devuelve
``-1``.

La función ``be_data_push`` se usa para agregar un nuevo elemento al
final del contenedor. El prototipo de función es:

.. code:: c

   void be_data_push(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto contenedor que se va a operar. El
objeto en ``index`` debe ser un valor de Lista. Esta función obtiene un
valor de la parte superior de la pila y lo agrega al final del
contenedor. Una vez completada la operación, el valor en la parte
superior de la pila no se extraerá de la pila.

La función ``be_data_insert`` se utiliza para insertar un par de
elementos en el contenedor. El prototipo de función es:

.. code:: c

   void be_data_insert(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto contenedor que se va a operar. El
objeto en ``index`` debe ser un valor de lista o un valor de mapa. El
elemento insertado forma un par de pares clave-valor. El valor se
almacena en la parte superior de la pila y la clave se almacena en el
índice anterior en la parte superior de la pila. Cabe señalar que la
clave insertada en el contenedor Mapa no puede ser un valor “nil” y la
clave insertada en el contenedor Lista debe ser un valor entero. Si la
operación es exitosa, la función devolverá ``bture``, de lo contrario
devolverá ``bfalse``.

La función ``be_data_remove`` se utiliza para eliminar un elemento del
contenedor. El prototipo de función es:

.. code:: c

   void be_data_remove(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto contenedor que se va a operar. El
objeto en ``index`` debe ser un valor de lista o un valor de mapa. Para
el contenedor de mapas, la llave para eliminar el elemento se almacena
en la parte superior de la pila virtual (debe empujarse antes de llamar
a la función); para el contenedor de lista, el índice del elemento que
se va a eliminar se almacena en la parte superior de la pila virtual
(debe estar antes de la llamada a la función). Si la operación es
exitosa, la función devolverá ``btrue``, de lo contrario devolverá
``bfalse``.

La función ``be_data_resize`` se utiliza para restablecer la capacidad
del contenedor. El prototipo de función es:

.. code:: c

   void be_data_resize(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto contenedor que se va a operar. Esta
función solo está disponible para contenedores de lista y la nueva
capacidad se almacena en la parte superior de la pila virtual (debe ser
un número entero).

La función ``be_iter_next`` se utiliza para obtener el siguiente
elemento del iterador. El prototipo de función es:

.. code:: c

   int be_iter_next(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del iterador a operar. El objeto iterador puede
ser un iterador de un contenedor List o un contenedor Map. Para el
iterador List, esta función empuja el valor del resultado de la
iteración a la parte superior de la pila, mientras que para el iterador
Map, empuja el valor clave a la posición anterior y la parte superior de
la pila, respectivamente. Llamar a esta función actualizará el iterador.
Si la función devuelve ``0``, la llamada falla, devuelve ``1`` para
indicar que el iterador actual es un iterador de lista y devuelve ``2``
para indicar que el iterador actual es un iterador de mapa.

La función ``map_hasnext`` se usa para probar si hay otro elemento en el
iterador. El prototipo de función es:

.. code:: c

   int map_hasnext(bvm *vm, int index)

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del iterador a operar. El objeto iterador puede
ser un iterador de un contenedor List o un contenedor Map. Si hay más
elementos iterables en el iterador, devuelve ``1``, de lo contrario,
devuelve ``0``.

La función ``be_refcontains`` se usa para probar si hay una referencia
al objeto especificado en la pila de referencia. Debe usarse junto con
``be_refpush`` y ``be_refpop``. Esta API puede evitar la recursividad al
atravesar objetos que tienen sus propias referencias. El prototipo de
función es:

.. code:: c

   int be_refcontains(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto a operar. Esta función se utiliza para
el valor de un tipo de instancia. Si hay una referencia al objeto en la
pila de referencia, devuelve ``1``, de lo contrario, devuelve ``0``.

La función ``be_refpush`` inserta la referencia del objeto especificado
en la pila de referencia. El prototipo de función es:

.. code:: c

   int be_refpush(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``index`` es el índice del objeto a operar. Esta función se utiliza para
el valor de un tipo de instancia.

La función ``be_refpop`` extrae el objeto en la parte superior de la
pila de referencia. Esta función eliminará un valor de la pila.

.. code:: c

   int be_refpop(bvm *vm);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual.
Esta función se usa en pares con ``be_refpush``. El siguiente es el uso
de la API de la pila de referencia para evitar el problema del recorrido
recursivo infinito cuando se hace referencia al objeto mismo:

.. code:: c

   int list_traversal(bvm *vm)
   {
       // ...
       if (be_refcontains(vm, 1)) {
           be_return(vm);
       }
       be_refpush(vm, 1);
       // Atravesando el contenedor, puede llamar a list_traversal recursivamente.
       be_refpop(vm);
       be_return(vm);
   }

Este es un proceso transversal simplificado del contenedor List. Para
obtener el código completo, consulte el código fuente de la función
``m_tostring`` en *be_listlib.c*. Asumimos que el índice del objeto List
es ``1``. Primero, verificamos si la Lista ya existe en la pila de
referencia (línea 4), y si la referencia ya existe, regresa
directamente, de lo contrario, continúa con el procesamiento posterior.
Para hacer utilizable ``be_refcontains``, necesitamos usar
``be_refpush`` y ``be_refpop`` para procesar la pila de referencia antes
y después de la operación transversal real (líneas 7 y 9).

La función ``be_stack_require`` prueba la cantidad de espacio libre en
la pila y expande el espacio de la pila si es insuficiente. El prototipo
de función es:

.. code:: c

   void be_stack_require(bvm *vm, int count);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``count`` es la capacidad de pila libre requerida. Si la capacidad libre
de la pila virtual asignada por la VM a la función nativa es inferior a
este valor, se realizará una operación de expansión.

La función ``be_isnil`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es ``nil``, si lo es, devuelve ``1``, de lo
contrario, devuelve ``0``. El prototipo de esta función es:

.. code:: c

   int be_isnil(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isbool`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es de tipo ``bool``, si lo es, la función
devuelve ``1``, de lo contrario devuelve ``0``. El prototipo de esta
función es:

.. code:: c

   int be_isbool(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isint`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es de tipo entero, si lo es, devuelve
``1``, de lo contrario, devuelve ``0``. El prototipo de esta función es:

.. code:: c

   int be_isint(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isreal`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es un tipo de número real, si lo es,
devuelve ``1``, de lo contrario, devuelve ``0``. El prototipo de esta
función es:

.. code:: c

   int be_isreal(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isnumber`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es un número entero o un tipo de
número real, si lo es, devuelve ``1``, de lo contrario, devuelve ``0``.
El prototipo de esta función es:

.. code:: c

   int be_isnumber(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isstring`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es un tipo de cadena, si lo es,
devuelve ``1``, de lo contrario, devuelve ``0``. El prototipo de esta
función es:

.. code:: c

   int be_isstring(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isclosure`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es un tipo de cierre, si lo es,
devuelve ``1``, de lo contrario, devuelve ``0``. El prototipo de esta
función es:

.. code:: c

   int be_isclosure(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isntvclos`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es un tipo de cierre primitivo,
si lo es, devuelve ``1``, de lo contrario, devuelve ``0``. El prototipo
de esta función es:

.. code:: c

   int be_isntvclos(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isfunction`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es un tipo de función, si lo es,
devuelve ``1``, de lo contrario, devuelve ``0``. El prototipo de esta
función es:

.. code:: c

   int be_isfunction(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir. Hay tres tipos de funciones:
cierre, función nativa y cierre nativo.

La función ``be_isproto`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es de tipo ``proto``, si lo es, devuelve
``1``, de lo contrario, devuelve ``0``. El prototipo de esta función es:

.. code:: c

   int be_isproto(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir. El tipo ``proto`` es el
prototipo de función del cierre de Berry.

La función ``be_isclass`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es de tipo ``class``, si lo es, devuelve
``1``, de lo contrario devuelve ``0``. El prototipo de esta función es:

.. code:: c

   int be_isclass(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isinstance`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es de tipo ``instance``, si lo
es, devuelve ``1``, de lo contrario devuelve ``0``. El prototipo de esta
función es:

.. code:: c

   int be_isinstance(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_isbytes`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es una instancia o subinstancia de la clase
``bytes``; si lo es, devuelve ``1``; de lo contrario, devuelve ``0``. El
prototipo de esta función es:

.. code:: c

   int be_isbytes(bvm *vm, int index);

La función ``be_islist`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es de tipo ``list``, si lo es, devuelve
``1``, de lo contrario devuelve ``0``. El prototipo de esta función es:

.. code:: c

   int be_islist(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_ismap`` devuelve si el valor indexado por el parámetro
``index`` en la pila virtual es de tipo ``map``, si lo es, devuelve
``1``, de lo contrario devuelve ``0``. El prototipo de esta función es:

.. code:: c

   int be_ismap(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

La función ``be_iscomptr`` devuelve si el valor indexado por el
parámetro ``index`` en la pila virtual es un tipo de puntero universal,
si lo es, devuelve ``1``, de lo contrario, devuelve ``0``. El prototipo
de esta función es:

.. code:: c

   int be_iscomptr(bvm *vm, int index);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual,
e ``index`` es el índice del valor a medir.

.. code:: c

   bint be_toint(bvm *vm, int index);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un tipo entero. Esta función no comprueba la
corrección del tipo. Si el valor es una instancia, se llama al método
``toint()`` si existe.

.. code:: c

   breal be_toreal(bvm *vm, int index);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un tipo de número de punto flotante. Esta
función no comprueba la exactitud del tipo.

.. code:: c

   bint be_toindex(bvm *vm, int index);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un tipo entero. Esta función no comprueba la
corrección del tipo. A diferencia de ``be_toint``, el tipo de valor de
retorno de ``be_toindex`` es ``int``, mientras que el valor de retorno
del primero es ``bint``.

.. code:: c

   bbool be_tobool(bvm *vm, int index);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un tipo booleano. Si el valor indexado no es de
tipo booleano, se convertirá de acuerdo con las reglas de la sección
type_bool, y el proceso de conversión no hará que cambie el valor
indexado. Si el valor es una instancia, se llama al método ``tobool()``
si existe.

.. code:: c

   const char* be_tostring(bvm *vm, int index);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un tipo de cadena. Si el valor indexado no es un
tipo de cadena, el valor indexado se convertirá en una cadena y el
proceso de conversión reemplazará el valor en la posición indexada en la
pila virtual con la cadena convertida. La cadena devuelta por esta
función siempre termina con los caracteres ``'\0'``. Si el valor es una
instancia, se llama al método ``tostring()`` si existe.

.. code:: c

   void* be_tocomptr(bvm* vm, int index);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un tipo de puntero general. Esta función no
comprueba la exactitud del tipo.

.. code:: c

   const void* be_tobytes(bvm *vm, int index, size_t *len);

Obtiene el valor de la posición de índice de ``index`` de la pila
virtual y devuelve como un búfer de bytes. Se devuelve el puntero del
búfer y el tamaño se almacena en ``*len`` (a menos que ``len`` sea
NULL). Esta función funciona solo para instancias de la clase ``bytes``,
o devuelve ``NULL``.

.. code:: c

   void be_pushnil(bvm *vm);

Inserta un valor ``nil`` en la pila virtual.

.. code:: c

   void be_pushbool(bvm *vm, int b);

Inserta un valor booleano en la pila virtual. El parámetro ``b`` es el
valor booleano que se insertará en la pila. Cuando el valor es ``0``,
significa falso, de lo contrario es verdadero.

.. code:: c

   void be_pushint(bvm *vm, bint i);

Inserta un valor entero ``i`` en la pila virtual.

.. code:: c

   void be_pushreal(bvm *vm, breal r);

Inserta un valor de punto flotante ``r`` en la pila virtual.

.. code:: c

   void be_pushstring(bvm *vm, const char *str)

Empuja la cadena ``str`` en la pila virtual. El parámetro ``str`` debe
apuntar a una cadena C que termina con un carácter nulo ``'\0'``, y no
se puede pasar un puntero nulo.

.. code:: c

   void be_pushnstring(bvm *vm, const char *str, size_t n);

Inserta la cadena ``str`` de longitud ``n`` en la pila virtual. La
longitud de la cadena está sujeta al parámetro ``n`` y el carácter nulo
no se usa como marca final de la cadena.

.. code:: c

   const char* be_pushfstring(bvm *vm, const char *formato, ...);

Empuja la cadena formateada en la pila virtual. El parámetro ``formato``
es una cadena formateada que contiene el texto que se insertará en la
pila, y el parámetro ``formato`` contiene una etiqueta, que puede ser
reemplazada por el valor especificado por el parámetro adicional
subsiguiente y formateada según sea necesario. De acuerdo con la
etiqueta de la cadena ``formato``, se pueden requerir una serie de
parámetros adicionales, y cada parámetro adicional reemplazará la
etiqueta ``%`` correspondiente en el parámetro ``formato``.

.. container::
   :name: tab::format_specifier

   +------------+-----------------------------------------------------+---+
   | **espec    | **Descripción**                                     |   |
   | ificador** |                                                     |   |
   +============+=====================================================+===+
   | ``d``      | Formato como entero con signo decimal (los números  |   |
   |            | positivos no generan signo)                         |   |
   +------------+-----------------------------------------------------+---+
   | ``f``      | Número de punto flotante de precisión simple o      |   |
   |            | doble con formato decimal                           |   |
   +------------+-----------------------------------------------------+---+
   | ``g``      | Número de punto flotante de precisión simple o      |   |
   |            | doble con formato exponencial                       |   |
   +------------+-----------------------------------------------------+---+
   | ``s``      | Formatear como cadena                               |   |
   +------------+-----------------------------------------------------+---+
   | ``c``      | Formatear como un solo carácter                     |   |
   +------------+-----------------------------------------------------+---+
   | ``p``      | Formatear como dirección de puntero                 |   |
   +------------+-----------------------------------------------------+---+
   | ``%``      | Escapado como carácter ``%`` (sin parámetro)        |   |
   +------------+-----------------------------------------------------+---+

   *Tabla 12: Descripción del parámetro de la etiqueta ``formato``*

La función ``be_pushfstring`` es similar a la función estándar de C
``printf``, pero la función de formato de cadenas es relativamente
básica y no admite operaciones como personalizar el ancho y los lugares
decimales. Un ejemplo típico es:

.. code:: c

   be_pushfstring(vm, "%s: %d", "hola", 12); // ¡Bien, funciona!
   be_pushfstring(vm, "%s: %.5d", "hola", 12); // Error, el ancho especificado no es compatible.

Esto significa que ``be_pushfstring`` solo puede realizar operaciones de
formateo simples. Si no se pueden cumplir los requisitos, se recomienda
utilizar cadenas con formato ``sprintf`` para las operaciones.

.. code:: c

   void be_pushvalue(bvm *vm, int index);

Empuja el valor con el índice ``index`` en la parte superior de la pila
virtual.

.. code:: c

   void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);

Empuja un cierre nativo en la parte superior de la pila virtual. El
parámetro ``f`` es el puntero de función C del cierre nativo, y
``nupvals`` es el número de valor superior del cierre.

.. code:: c

   void be_pushntvfunction(bvm *vm, bntvfunc f);

Empuja una función nativa en la parte superior de la pila virtual y el
parámetro ``f`` es el puntero de la función nativa.

.. code:: c

   void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);

Empuja una clase nativa en la parte superior de la pila virtual. El
parámetro ``name`` es el nombre de la clase nativa y el parámetro
``lib`` es la descripción del atributo de la clase nativa.

.. code:: c

   void be_pushcomptr(bvm *vm, void *ptr);

Empuja un puntero general en la parte superior de la pila virtual. El
puntero general ``ptr`` apunta a una determinada área de datos de C.
Dado que el recolector de elementos no utilizados de Berry no mantiene
el contenido señalado por este puntero, los usuarios deben mantener el
ciclo de vida de los datos ellos mismos.

.. code:: c

   void* be_pushbytes(bvm *vm, const void *buf, size_t len);

Empuja un búfer ``bytes ()`` que comience en la posición ``buf`` y de
tamaño ``len``. El búfer se copia en la memoria asignada de Berry, no
necesita mantener el búfer válido después de esta llamada.

.. code:: c

   bbool be_pushiter(bvm *vm, int index);

Empuja un iterador en la parte superior de la pila virtual.

La función ``be_pusherror`` inserta un mensaje de error en la parte
superior de la pila. Después de ejecutar el FFI, el intérprete volverá
directamente a la posición que puede manejar el error, y el código
inmediatamente siguiente no se ejecutará. El prototipo de función es:

.. code:: c

   void be_pusherror(bvm *vm, const char *msg);

El parámetro ``vm`` es el puntero de la instancia de la máquina virtual;
``msg`` es la cadena que contiene la información del error.

Mueve el valor en el índice ``desde`` a la posición del índice
``hasta``. Esta función no elimina el valor de la posición del índice
``desde``, solo modifica el valor de la posición del índice ``hasta``.

Tecnología de construcción en tiempo de compilación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La tecnología de construcción en tiempo de compilación se implementa
principalmente mediante *coc*, que se encuentra en la ruta *coc/coc* del
directorio del código fuente del intérprete. La herramienta *coc* se usa
para generar cadenas constantes y objetos constantes como código C, y se
compilará en constantes cuando se compile el intérprete. En principio,
la herramienta *coc* generará código a partir de la información de
declaración del objeto constante (de acuerdo con un formato específico).
El proceso calculará automáticamente el valor Hash y generará la tabla
Hash.

El archivo *Makefile* en el directorio raíz del proyecto del intérprete
compilará automáticamente esta herramienta y la ejecutará antes de
compilar el código fuente del intérprete. El contenido de *Makefile*
asegura que cuando se usa el comando ``make``, el código para construir
el objeto en tiempo de compilación siempre se actualizará a través de la
herramienta (si necesita actualizarse). El código para construir objetos
en tiempo de compilación se puede generar manualmente a través del
comando ``make prebuild``, que se almacena en la carpeta *generate*.

La construcción en tiempo de compilación se puede activar o desactivar
modificando la macro ``BE_USE_PRECOMPILED_OBJECT``. En cualquier caso,
se llama a la herramienta *coc* para generar códigos de objetos
constantes (los códigos no se usan cuando la construcción en tiempo de
compilación está desactivada).

Usar el comando ``coc``
^^^^^^^^^^^^^^^^^^^^^^^

La herramienta ``coc`` se utiliza para generar código para objetos
constantes. El formato del comando es

.. code:: bash

   tools/coc/coc -o <dst_path> <src_path(s)> -c <include_path>

La ruta de salida *dst_path* se utiliza para almacenar el código
generado, y la ruta de origen *src_path* es una lista de rutas que deben
escanearse en busca del código fuente (utilice espacios para separar
varias rutas). ``include_path`` contiene un archivo de encabezado C
escaneado para detectar directivas de compilación. ``coc`` intenta
compilar solo las constantes necesarias. Dado que *generate* se usa como
la ruta del código generado en el código fuente del intérprete,
*dst_path* debe ser *generate*. Tomando el proyecto de intérprete
estándar como ejemplo, el comando para usar la herramienta en
``map_build`` debe ser

.. code:: bash

   tools/coc/coc -o generate default src -c default/berry_conf.h

El significado de este comando es: la ruta de salida es *generate*, y la
ruta de origen es *src* y *default*.

Ruta de salida
^^^^^^^^^^^^^^

Estrictamente hablando, la carpeta *generate* utilizada como ruta de
salida no se puede colocar en ningún lado, debe almacenarse en un
directorio principal que contenga la ruta. La ruta de inclusión se
refiere a la ruta donde se buscará el archivo de encabezado en el
proyecto. Tomando el código fuente del intérprete estándar como ejemplo,
la ruta de inclusión es *src* y *default*. Por lo tanto, en el proyecto
de intérprete estándar, la carpeta *generate* se almacena en el
directorio raíz del código fuente del intérprete (el directorio
principal de *src* y *default*).

El motivo de las reglas anteriores es que los siguientes códigos se
utilizan en el código fuente del intérprete para hacer referencia a
objetos constantes:

.. code:: c

   #include "../generate/be_fixed_xxx.h"

Si los lectores quieren definir objetos constantes por sí mismos,
también necesitan usar dicho código para incluir los archivos de
encabezado correspondientes. Esta sección presentará cómo usar estas
herramientas para definir y usar objetos constantes.

Tabla de cadenas en tiempo de compilación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

La tabla de cadenas en tiempo de compilación se utiliza para almacenar
cadenas constantes. Las cadenas constantes son objetos que son
transparentes para el script. No se crean ni destruyen cuando el
intérprete se está ejecutando, pero siempre se almacenan como constantes
en el segmento de datos del programa del intérprete. Si necesita usar
una cadena como cadena constante, puede agregar el prefijo
``be_const_str_`` delante de la cadena en el código fuente del
intérprete, y la declaración se puede colocar en cualquier parte del
archivo fuente (incluidos los comentarios). Por ejemplo, para crear una
cadena constante con el contenido ``"cadena"``, debe declarar el símbolo
``be_const_str_cadena`` en el archivo fuente, y este símbolo también es
el nombre de la variable que hace referencia a la cadena constante en el
código C.

Todas las palabras clave crearán cadenas constantes. Si modifica el
código relacionado con la palabra clave en el intérprete de Berry,
también se debe modificar el código correspondiente en *coc*.

Si la cadena contiene símbolos especiales, se “escapean” automáticamente
como ``_XHH``, donde ``HH`` es la representación hexadecimal (en
mayúsculas) del carácter. Por ejemplo ``"`` está representado por
``_X3A``. Esta representación es biactiva, por lo que es fácil
convertirla a la cadena original y desde ella.

Usar cadena constante
^^^^^^^^^^^^^^^^^^^^^

Normalmente, no hay necesidad de declarar cadenas constantes
manualmente, ni de usarlas manualmente. Si realmente necesita llamar a
la cadena constante manualmente, incluya el archivo de encabezado
*be_constobj.h* para usar todas las variables de cadena constante (este
archivo de encabezado tiene declaraciones para todas las cadenas
constantes). El uso típico de cadenas constantes es construir objetos en
tiempo de compilación. La declaración y definición de cadenas constantes
en este proceso son manejadas automáticamente por la herramienta.

En cualquier caso, la función FFI ``be_pushstring`` debe usarse
directamente para crear una cadena. Cuando una cadena tiene una cadena
constante, no creará repetidamente un nuevo objeto de cadena, sino que
usará directamente la cadena constante correspondiente.

De forma predeterminada, todas las cadenas utilizadas se referencian en
una tabla hash global ``m_const_string_table``. Sin embargo, algunos
proyectos pueden tener muchas variantes de compilación para las que no
se necesitan algunos conjuntos de cadenas. Si todas las constantes de
cadena se almacenan en todas las variantes, esto crea una pérdida de
tamaño de flash. Por esta razón, algunas cadenas pueden declararse como
cadenas “débiles” en el sentido de tener una referencia “débil”. En tal
caso, la constante de cadena se declara en código C, pero no se incluye
en el objeto de mapa global. Esto significa que el enlazador puede optar
por no incluir las constantes de cadena si ningún código hace referencia
a ellas. La desventaja es que si crea dinámicamente un objeto de cadena
con el mismo valor, se crea un nuevo objeto en la memoria (mientras que
no lo haría para una constante de cadena normal). Para indicar cadenas
débiles, use el modificador ``strings: weak`` (ver más abajo).

Construir objeto en tiempo de compilación
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los objetos construidos en tiempo de compilación también se denominan
objetos constantes. La estructura de datos de estos objetos se construye
cuando se compila el intérprete y no se puede modificar en tiempo de
ejecución. ``map_build`` define un conjunto de reglas de declaración en
la herramienta para generar código C para objetos constantes. La
información de declaración del objeto constante se almacena directamente
en el archivo fuente (*\*.c*). Para distinguirlo de otro contenido, se
debe incluir una información de declaración completa en el siguiente
código de arranque:

::

   @const_object_info_begin
   @const_object_info_end

La información de declaración de objeto constante no se ajusta a la
sintaxis del lenguaje C, por lo que debe colocarse en un comentario de
varias líneas (incluido con ``/* */``). Todos los objetos constantes
tienen la misma forma de declaración. La estructura de declaración de un
objeto constante se denomina “bloque de declaración de objeto”, que se
compone de

::

   type object_name (attributes) {
       member_fields
   }

``type`` es el tipo de objeto constante, puede ser ``map``, ``class``,
``module`` o ``vartab``. ``object_name`` es el nombre de la variable del
objeto constante en lenguaje C. ``attributes`` es la lista de atributos
de los objetos constantes. Un atributo se compone de nombre de atributo
y valor de atributo. El nombre del atributo y el valor del atributo
están separados por punto y coma, y varios atributos están separados por
comas. Por ejemplo, la lista de atributos ``scope: global, name: map``
significa que el atributo ``scope`` de un objeto constante es
``global``, y el atributo ``name`` es ``map``. También ``strings: weak``
indica que se debe generar constantes de cadena débiles para los nombres
de los campos de miembros o cualquier constante de cadena.
``member_fields`` es la lista de dominios miembros de objetos
constantes. Un miembro se compone de nombre y valor, separados por
comas. Cada línea puede declarar un miembro y varios miembros deben
declararse en varias líneas.

La herramienta **coc** utiliza expresiones regulares para analizar el
bloque de declaración de objetos. En el proceso de análisis, primero se
comparará todo el bloque de declaración del objeto y se comparará la
información “tipo” y “nombre_del_objeto”. Para la información de
``atributos`` y ``member_fields``, sae hará un análisis adicional. Para
facilitar la implementación, la herramienta no tiene requisitos
estrictos sobre la sintaxis del bloque de declaración de objetos y
carece de un mecanismo completo de manejo de errores, por lo que debe
asegurarse de que la sintaxis sea correcta al escribir el bloque de
declaración de objetos.

Para facilitar la comprensión, ilustramos con una clase constante
simple:

.. code:: c

   /* @const_object_info_begin
   class be_class_map (scope: global, name: map) {
       .data, var
       init, func(m_init)
       tostring, func(m_tostring)
   }
   @const_object_info_end */
   #include "../generate/be_fixed_be_class_map.h"

En este ejemplo, la información de declaración de toda la clase
constante está en el comentario del lenguaje C, por lo que no afectará
la compilación del código C. El bloque de declaración de objetos se
coloca entre ``@const_object_info_begin`` y ``@const_object_info_end``
para garantizar que la herramienta **coc** detecte el bloque de
declaración de objetos.

Dado que es una declaración de clase constante, el valor de *tipo* en el
bloque de declaración de objeto es ``class``, y ``be_class_map`` es el
nombre de variable del objeto constante en el código C. Se declaran dos
atributos en la lista de atributos del objeto (la parte encerrada entre
paréntesis), y el significado de estos atributos se presentará en la
sección “Clase de construcción en tiempo de compilación” de esta
sección. Tres miembros están definidos en la lista de miembros entre
llaves, varios miembros están separados por saltos de línea y el nombre
del miembro y el valor del miembro están separados por una coma. Existen
varios formatos legales para los nombres de miembros:

-  Formato de nombre de variable Berry: comienza con una letra o guión
   bajo, seguido de varias letras, guiones bajos o números.

-  Utilice “``.``” como primer carácter, seguido de letras, guiones
   bajos o números.

-  Operadores sobrecargables, como “``+``”, “``-``” y “``<<``”, etc.

El valor de un miembro puede ser de los siguientes tipos:

-  ``var``: Este símbolo se compilará en un objeto entero
   (``be_const_var``), y el valor del objeto entero se incrementa
   automáticamente desde ``0``. ``var`` está diseñado para la
   declaración de variables miembro en la clase, y es automática La
   función de numeración se utiliza para realizar el número de serie de
   las variables miembro.

-  ``func(símbolo)``: Declara funciones miembro nativas o métodos de
   objetos constantes. El símbolo se compilará en una función nativa con
   el valor (``be_const_func``), ``symbol`` es el puntero de función
   nativo correspondiente al miembro. ``m_init`` y ``m_tostring`` en el
   ejemplo son dos funciones nativas.

-  ``closure (símbolo)``: Declara funciones o métodos miembro de
   bytecode precompilados de objetos constantes. El símbolo se compilará
   en una función nativa con el valor (``be_const_closure``),
   ``símbolo`` es el nombre de la función solidificada. Ver
   ``módulo solidificar`` para saber cómo solidificar objetos.

-  ``nil()``: este símbolo se compilará en un valor nulo
   (``be_const_nil``).

-  ``int(valor)``: este símbolo se compilará en un objeto entero
   (``be_const_int``), el valor del objeto entero es ``valor``.

-  ``real(valor)``: Este símbolo se compilará en un número real
   (``be_const_real``), el valor del objeto de número real es ``valor``.

-  ``comptr(valor)``: Este símbolo se compilará en un puntero objeto
   (``be_const_comptr``), el valor del puntero es ``valor`` y se puede
   utilizar para pasar la dirección de una estructura global de C.

-  ``class (símbolo)``: este símbolo se compilará en un objeto de clase
   (``be_const_class``). ``símbolo`` es un puntero a este tipo de
   objeto, y el puntero debe apuntar a un objeto de tipo constante.

-  ``module (símbolo)``: este símbolo se compilará en un objeto de
   módulo (``be_const_module``). ``símbolo`` es un puntero al objeto del
   módulo, y el puntero debe apuntar a un objeto de módulo constante.

-  ``ctype_func(símbolo)``: Este símbolo se compilará en una función
   nativa (``be_const_ctype_func``). ``símbolo`` es un puntero al mapeo
   C definición. Esta característica es utilizada por
   `berry_mapping <https://github.com/berry-lang/berry_mapping>`__

Para usar el objeto ``be_class_map``, debemos incluir el archivo de
encabezado correspondiente en el código C para garantizar que el objeto
se compilará. La práctica habitual es incluir el archivo de cabecera
correspondiente cerca del bloque de declaración del objeto. En el
ejemplo, la línea 8 lo contiene. El archivo de encabezado
correspondiente se puede usar para construir objetos ``be_class_map`` en
tiempo de compilación.

Después de procesarlo con la herramienta **coc**, cada bloque de
declaración de objeto se compilará en un archivo de encabezado llamado
*be_fixed_be_xxx.h*, donde *xxx* es el nombre de la variable C del
objeto. Para compilar objetos constantes en código C, debemos incluir
los archivos de encabezado correspondientes. Por lo general, se
recomienda incluir los archivos de encabezado correspondientes cerca del
bloque de declaración de objetos. La octava línea del ejemplo contiene
*be_fixed_be_class_map.h* para construir el objeto ``be_class_map`` en
tiempo de compilación.

Construir mapa en tiempo de compilación
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Los mapas construidos en tiempo de compilación también son objetos
``map`` constantes. Por lo general, no se declaran directamente mediante
bloques de declaración de objetos, sino que se declaran en otras
estructuras de construcción en tiempo de compilación. Al construir el
``map`` constante, la información del tipo de objeto constante debe ser
``map``, que admite un atributo ``scope``. Cuando el valor del atributo
‘scope’ es ‘local’, el objeto constante es ‘estático’, cuando el
atributo es ‘global’, es ‘externo’, y el valor de este atributo es
‘local’ por defecto. Los ``member_fields`` del objeto ``map`` constante
admiten especificaciones comunes de nombre de miembro/valo, y los
valores de miembro solo se almacenan como datos sin una interpretación
especial. El siguiente es un ejemplo del uso del bloque de declaración
de objetos para declarar directamente un objeto ``map`` constante:

::

   map map_name (scope: local/global) {
       init, func(m_init)
   }

Construcción de Clases en tiempo de compilación
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Para construir una clase en tiempo de compilación, use el bloque de
declaración de objetos para declarar, y la información de tipo del
objeto es ``class``. Las propiedades declaradas de este objeto son
``scope`` y ``name``. ``scope`` es el alcance de la variable C del
objeto de declaración de atributos, cuando el valor es ``local``
(predeterminado), el alcance es ``static``, cuando es ``global``, el
alcance es ``extern``; ``name`` es el valor del atributo es ese nombre
de clase, la clase anónima puede omitir este parámetro. Dado que la
lista de atributos de una clase solo almacena métodos e índices de
variables miembro, los ``member_fields`` de la clase construida en
tiempo de compilación solo pueden usar los valores ``var`` y ``func()``.
Un bloque de declaración de clase de construcción simple en tiempo de
compilación es:

::

   class be_class_map (scope: global, name: map) {
       .data, var
       init, func(m_init)
       tostring, func(m_tostring)
   }

Construcción de Módulos en tiempo de compilación
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

La información de tipo del bloque de declaración del bloque de
construcción en tiempo de compilación es ``module``.

.. code:: c

   module math (scope: global) {
       sin, func(m_sin)
       cos, func(m_cos)
       pi, real(M_PI)
   }

Construcción de un dominio integrado en tiempo de compilación
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code:: c

   vartab m_builtin (scope: local) {
       assert, func(l_assert)
       print, func(l_print)
       list, class(be_class_list)
   }
