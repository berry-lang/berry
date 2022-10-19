.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

Guía de portabilidad
====================

Archivo de configuración
------------------------

El archivo de encabezado de configuración del intérprete de Berry es
*berry_conf.h*. Este archivo incluye un lote de macros para la
configuración y define algunos contenidos relacionados con la
plataforma.

*berry_conf.h* Archivo
~~~~~~~~~~~~~~~~~~~~~~

Interruptor de macro de configuración
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Las macros de configuración presentadas en esta sección generalmente se
usan para compilar conmutadores de algunos códigos fuente. Para
facilitar la descripción, llamamos a esta macro “cambio de macro”. Para
el interruptor de macro, “on” se refiere a establecer el interruptor de
macro en un valor distinto de cero, y “off” se refiere a establecer el
valor del interruptor de macro en ``0``.

Algunos interruptores macro tienen varios estados, no solo “encendido” o
“apagado”. Estos interruptores macro se usan generalmente para
configuraciones con múltiples opciones. También hay algunas macros de
configuración que no son conmutadores de macro. No importa cuál sea el
valor de estas macros, no habrá código y, por lo tanto, no participará
en la compilación. Estas macros se utilizan generalmente para configurar
el valor de la cantidad.

[sección::BE_DEBUG]

Este conmutador de macro se utiliza para activar o desactivar la función
de depuración del propio intérprete. Cuando el valor de ``BE_DEBUG`` es
``0``, la depuración se desactiva; de lo contrario, se activará. La
función de depuración mencionada aquí se refiere a la depuración del
intérprete, no a la función de depuración del programa Berry. El valor
predeterminado de ``BE_DEBUG`` es ``0``. Si usa *Makefile* que viene con
el proyecto del intérprete para compilar, este interruptor de macro se
activará automáticamente cuando use el comando ``make debug``.

Cuando se abre esta macro, se activarán algunas aserciones y se generará
un mensaje de error cuando el intérprete encuentre un error que la
aserción pueda detectar. Puede abrir ``BE_DEBUG`` al depurar el
intérprete y cerrarlo al compilar la versión.

Este conmutador de macro configura el tipo de punto flotante utilizado
por el tipo ``breal``. Cuando el valor de la macro es ``0``, se usará el
tipo ``doble`` para implementar ``breal``, de lo contrario, se usará el
tipo ``float`` para implementar ``breal``. Este interruptor de macro se
puede activar en algunos entornos con bajo rendimiento o configuración
de memoria. En la implementación predeterminada, este interruptor de
macro está desactivado.

Esta macro configura la implementación del tipo ``bint``. Cuando el
valor de la macro es ``0``, se usará el tipo ``int`` para implementar
``bint``, cuando el valor sea ``1``, se usará el tipo ``long`` para
implementar ``bint``, y cuando el el valor es ``2``, se usará 64 ] Tipo
de entero con signo de bit (``__int64`` en Windows, ``long long`` en
otras plataformas) implementa ``bint``. El valor predeterminado de esta
macro es ``2``. Si desea reducir el uso de la memoria, puede establecer
esta macro en ``0`` o ``1`` para habilitar el tipo de entero de 32 bits.

Esta macro se usa para configurar la información de depuración en tiempo
de ejecución del código Berry. Tiene 3 valores disponibles: establecer
en ``0`` para desactivar la salida del nombre de archivo y el número de
línea de la información de depuración en tiempo de ejecución, y
establecer en ``1`` para mostrar el nombre de archivo y el número de
línea en la información de depuración en tiempo de ejecución, establecer
a ``2`` Al usar ``uint16_t`` (entero de 16 bits) escriba para almacenar
la información del número de fila. Su valor por defecto es ``1``.

Establecer esta macro en ``0`` no almacenará el nombre del archivo ni la
información del número de línea, por lo que el consumo de memoria es
mínimo. Cuando se establece en ``2``, consume menos memoria, pero si el
programa es demasiado largo, ``uint16_t`` se desbordará.

Este modificador de macro configura la función de construir objetos en
tiempo de compilación. Activar esta macro significa que la construcción
de objetos en tiempo de compilación está habilitada. Esta macro está
activada de forma predeterminada. Cuando esta macro está activada, los
objetos nativos de la biblioteca estándar se generarán en tiempo de
compilación y cuando esta macro está desactivada, los objetos de la
biblioteca estándar se generarán en tiempo de ejecución.

Las funciones ``be_regfunc`` y ``be_regclass`` se verán afectadas por
esta macro. La tabla de objetos integrada no se puede modificar cuando
se utiliza la construcción de objetos en tiempo de compilación. En este
momento, estas dos funciones no pueden registrar objetos en el ámbito
integrado, pero registran objetos en el ámbito global.

Los objetos construidos durante el tiempo de compilación se almacenan
junto con el código y no ocuparán recursos de RAM (o el área de lectura
y escritura en la memoria). La tecnología de construcción durante el
tiempo de compilación también puede reducir el tiempo de inicio del
intérprete, por lo que se recomienda abrir esta macro. Consulte la
sección [section::precompiled_build] para obtener más detalles sobre las
técnicas de construcción en tiempo de compilación.

Esta macro define la capacidad máxima de pila de Berry, que se refiere a
la cantidad de objetos de Berry. Cuando el código Berry usa más de esta
cantidad de pila, dejará de ejecutar el programa y devolverá un mensaje
de error. El valor predeterminado de esta macro es ``2000``, que puede
modificarse según la capacidad de memoria del sistema.

Este valor no afecta el uso de memoria de la pila Berry, porque la
capacidad de la pila Berry se ajusta dinámicamente, por lo que no
importa cuánto se establezca, no puede ayudar a reducir el uso de
memoria. Su función principal es terminar la ejecución cuando el
programa Berry consume demasiada pila. Es muy probable que estos
programas sean incorrectos, por ejemplo, las llamadas a funciones
recursivas sin condiciones de devolución seguirán consumiendo la pila.

Esta macro define el espacio mínimo disponible en la pila de Berry y su
valor predeterminado es ``10``. La función nativa puede insertar un
valor en la pila de Berry. En este momento, la pila no crecerá
automáticamente, así que asegúrese de que haya suficiente espacio en la
pila para que la use la función nativa. No se recomienda modificar este
valor, sino usar la función ``be_stack_require`` donde realmente se
necesita más espacio de pila.

Para detectar errores de desbordamiento de pila al depurar el
intérprete, puede abrir la macro ``BE_DEBUG`` (sección
[sección::BE_DEBUG] ).

Cuando se abre esta macro, el objeto de cadena corta guardará el valor
hash de la cadena para mejorar la velocidad de ejecución, pero el tamaño
de cada objeto de cadena aumentará en 4 bytes. Esta macro está
desactivada de forma predeterminada y las pruebas actuales no han
encontrado que abrir esta macro traiga una mejora significativa.

Este conmutador de macro se utiliza para activar o desactivar el módulo
``string``, que está activado de forma predeterminada.

Este interruptor de macro se usa para habilitar o deshabilitar el módulo
``json``, que está activado de forma predeterminada.

Este conmutador de macro se utiliza para habilitar o deshabilitar el
módulo “matemático”, que está activado de forma predeterminada.

Este interruptor de macro se usa para habilitar o deshabilitar el módulo
``time``, que está activado de forma predeterminada.

Este conmutador de macro se utiliza para activar o desactivar el módulo
``os``, que está activado de forma predeterminada.

Esta macro determina la función ``abortar`` utilizada internamente por
el intérprete de Berry. Por defecto o cuando la macro no está definida,
se utilizará la función ``abortar`` en la biblioteca estándar de C. Esta
macro se define como ``abortar`` por defecto. Si el usuario necesita
especificar explícitamente la función ``abortar`` utilizada por el
intérprete, reemplace la definición de macro con la función requerida
por el usuario. Esta función debe tener la misma forma que la
declaración de la función ``abortar`` en la biblioteca estándar.

Esta macro determina la función ``exit`` utilizada internamente por el
intérprete de Berry. Por defecto o cuando la macro no está definida, se
utilizará la función ``exit`` en la biblioteca estándar de C. Esta macro
se define como ``salir`` por defecto. Si el usuario necesita especificar
explícitamente la función ``salir`` utilizada por el intérprete,
reemplace la definición de macro con la función requerida por el
usuario. Esta función debe tener la misma forma que la declaración de la
función ``exit`` en la biblioteca estándar.

Esta macro determina la función ``malloc`` utilizada internamente por el
intérprete de Berry. Por defecto o cuando la macro no está definida, se
utilizará la función ``malloc`` en la biblioteca estándar de C. Esta
macro se define como ``malloc`` por defecto. Si el usuario necesita
especificar explícitamente la función ``malloc`` utilizada por el
intérprete, reemplace la definición de macro con la función requerida
por el usuario. Esta función debe tener la misma forma que la
declaración de la función ``malloc`` en la biblioteca estándar.

Esta macro determina la función ``libre`` utilizada internamente por el
intérprete de Berry. Por defecto o cuando la macro no está definida, se
utilizará la función ``free`` en la biblioteca estándar de C. Esta macro
se define como “gratis” por defecto. Si el usuario necesita especificar
explícitamente la función “libre” utilizada por el intérprete, reemplace
la definición de macro con la función requerida por el usuario. Esta
función debe tener la misma forma que la declaración de la función
“libre” en la biblioteca estándar.

Esta macro determina la función ``realloc`` utilizada internamente por
el intérprete de Berry. Por defecto o cuando la macro no está definida,
se utilizará la función ``realloc`` en la biblioteca estándar de C. Esta
macro se define como ``realloc`` por defecto. Si el usuario necesita
especificar explícitamente la función ``realloc`` utilizada por el
intérprete, reemplace la definición de macro con la función requerida
por el usuario. Esta función debe tener la misma forma que la
declaración de la función ``realloc`` en la biblioteca estándar.

Esta macro se utiliza para definir la implementación de la función de
aserción. De forma predeterminada, la función ``assert`` en la
biblioteca estándar de C se usa para implementar la afirmación. Si el
sistema de destino tiene inconvenientes para usar la función
``assert()`` en la biblioteca estándar para hacer una afirmación, puede
modificar la definición de la macro ``be_assert``. Una función de
aserción correcta debe usar la siguiente declaración:

.. code:: c

   void assert(int condition);

Entre ellos, ``condición`` es la condición de afirmación. Si no se
cumple la condición, se emitirá un mensaje de error y el programa
finalizará. Por supuesto, la función “afirmar” generalmente se
implementa mediante una macro.

Archivo *berry_port.c*
----------------------

Este archivo implementa las funciones de E/S de bajo nivel del
intérprete de Berry, incluida la entrada y salida estándar y la
compatibilidad con el sistema de archivos. El archivo *berry_port.c* en
el directorio *predeterminado* contiene un conjunto de soporte de E/S
portátil. Las operaciones de archivo y la entrada y salida estándar se
implementan mediante API en la biblioteca estándar de C. Las operaciones
de ruta y carpeta son compatibles con las API estándar de Windows y
POSIX. Este archivo también implementa un conjunto de funciones de
operación de E/S basadas en FatFs para que los usuarios las usen
directamente. Si necesita usar el intérprete de Berry en otros entornos,
estas funciones deben implementarse por separado (es posible que solo
deban implementarse parcialmente).

Esta sección presentará las funciones de las funciones implementadas en
el archivo *berry_port.c* y guiará a los usuarios para implementar su
propia versión.

.. code:: c

   void be_writebuffer(const char *buffer, size_t length);

Envíe un dato al dispositivo de salida estándar, el parámetro “búfer” es
la primera dirección del bloque de datos de salida y “longitud” es la
longitud del bloque de datos de salida. Esta función genera el archivo
``stdout`` de forma predeterminada. Dentro del intérprete, esta función
generalmente se usa como una salida de flujo de caracteres, no como un
flujo binario.

Las funciones ``be_writebuffer`` son muy versátiles y deben
implementarse.

.. code:: c

   char* be_readstring(char *buffer, size_t size);

Ingrese un dato del dispositivo de entrada estándar y lea como máximo
una fila de datos cada vez que se llame a esta función. El parámetro
``buffer`` es el búfer de datos pasado por la persona que llama, y la
capacidad del búfer es ``tamaño``. Esta función dejará de leer y
regresará cuando se agote la capacidad del búfer; de lo contrario,
regresará cuando se lea un carácter de nueva línea o un carácter de fin
de archivo. Si la función se ejecuta con éxito, usará directamente el
parámetro ``buffer`` como valor de retorno, de lo contrario devolverá
``NULL``.

Esta función agregará los saltos de línea de lectura a los datos leídos,
y cada vez que se llame a la función ``be_readstring``, continuará
leyendo desde la posición actual. Esta función solo se llama en la
implementación de la función nativa ``input``, y es posible que la
función ``be_readstring`` no se implemente cuando no sea necesaria.

.. code:: c

   void* be_fopen(const char *filename, const char *modes);

Para abrir un archivo, ``filename`` es el nombre del archivo que se
abrirá y ``modos`` es el método de apertura. La función devolverá un
identificador de archivo o un puntero a la estructura de operación del
archivo. El uso de esta función es similar a la función ``fopen`` en la
biblioteca estándar de C. El nombre del archivo es una cadena de estilo
C (que termina con un carácter ``\0``), y el patrón debe admitir al
menos las siguientes condiciones:

-  ``r``, ``rt``: Para abrir un archivo de texto en modo de solo
   lectura, el archivo debe existir.

-  ``r+``, ``rt+``: Abre un archivo de texto en modo lectura-escritura y
   crea un nuevo archivo si el archivo no existe.

-  ``rb``: abre un archivo binario en modo de solo lectura, el archivo
   debe existir.

-  ``rb+``: abre un archivo binario en modo de lectura y escritura y
   crea un nuevo archivo si el archivo no existe.

-  ``w``, ``wt``: Crear y abrir un archivo de texto en modo de solo
   escritura, y el el archivo existente será eliminado.

-  ``w+``, ``wt+``: Crea y abre un archivo de texto en modo
   lectura-escritura, y el el archivo existente será eliminado.

-  ``wb``: crea y abre un archivo binario en modo de solo escritura, y
   el el archivo existente será eliminado.

-  ``wb+``: Crea y abre un archivo binario en modo lectura-escritura, y
   el el archivo existente será eliminado.

De forma predeterminada, la función ``fopen`` en la biblioteca estándar
de C se usa para implementar ``be_fopen``. Si utiliza otros métodos para
lograrlo, debe asegurarse de que se puedan lograr los modos de
funcionamiento anteriores. Si no se requieren operaciones de archivo,
esta función se puede dejar en blanco. Las operaciones de archivos aquí
incluyen todos los escenarios, como usar la función ``abrir`` en el
script, cargar el script desde un archivo (usando la función
``be_loadfile``), etc.

.. code:: c

   int be_fclose(void *hfile);

Cierra un archivo, ``hfile`` es el identificador de archivo cerrado. La
función de esta función es similar a la función ``fclose`` en la
biblioteca estándar de C.

.. code:: c

   size_t be_fwrite(void *hfile, const void *buffer, size_t length);

Escribe un dato en el archivo especificado. El parámetro ``hfile`` es el
identificador del archivo que se escribirá, ``buffer`` es el puntero de
los datos que se escribirán, ``length`` es el número de datos que se
escribirán (en bytes).

.. code:: c

   size_t be_fread(void *hfile, void *buffer, size_t length);

Leer un fragmento de datos del archivo especificado. El parámetro
``hfile`` es el identificador del archivo que se leerá, ``buffer`` es el
puntero al búfer de lectura y ``length`` es el número de bytes que se
leerán.

.. code:: c

   char* be_fgets(void *hfile, void *buffer, int size);

Lea una línea del archivo, similar a la función ``fgets`` en la
biblioteca estándar de C. El parámetro ``hfile`` es el identificador del
archivo que se va a leer, ``buffer`` es el puntero del búfer de lectura
y ``size`` es la capacidad del búfer de lectura. Esta función regresará
cuando se lean los bytes ``size - 1``, los caracteres de nueva línea y
los caracteres de fin de archivo, y el valor de retorno sea ``buffer``.

.. code:: c

   int be_fseek(void *hfile, long offset);

Establezca la posición del puntero de lectura y escritura del archivo.
El parámetro ``hfile`` es el identificador de archivo que se va a
utilizar y ``offset`` es el valor que se va a establecer.

.. code:: c

   long int be_ftell(void *hfile);

Obtenga el puntero de lectura y escritura actual del archivo, el
parámetro ``hfile`` es el identificador del archivo que se va a operar,
y el valor de retorno de esta función es el puntero de lectura y
escritura del archivo.

.. code:: c

   long int be_fflush(void *hfile);

Escriba los datos del búfer de archivo en el archivo. El parámetro
``hfile`` es el archivo a operar.

.. code:: c

   size_t be_fsize(void *hfile);

Obtenga el tamaño del archivo. El parámetro ``hfile`` es el archivo a
operar.
