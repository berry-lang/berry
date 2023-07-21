3. Expresiones
==============

3.1 Conceptos básicos
---------------------

Una expresión (Declaración) se compone de uno a más operandos y
operadores, y se puede obtener un resultado evaluando la expresión. Este
resultado se llama el valor de la expresión. El operando puede ser un
valor literal, una variable, una llamada de función o una subexpresión,
etc. Las expresiones y operadores simples también se pueden combinar en
expresiones más complejas. Similar a las cuatro operaciones aritméticas,
la precedencia de los operadores afecta el orden de evaluación de las
expresiones. Cuanto mayor sea la precedencia del operador, antes se
evaluará la expresión.

Operadores y expresiones
~~~~~~~~~~~~~~~~~~~~~~~~

Berry proporciona algunos operadores unarios y operadores binarios. Por
ejemplo, el operador AND lógico ``&&`` es un operador binario, y el
operador de negación lógica ``!`` es un operador unario. Algunos
operadores pueden ser operadores unarios u operadores binarios. El
significado específico de tales operadores depende del contexto. Por
ejemplo, el operador ``-`` es un símbolo unario en la expresión ``-1``,
pero es un signo menos binario en la expresión ``1-2``.

Expresión de combinación de operadores
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Tanto el lado izquierdo como el derecho de un operador binario pueden
ser subexpresiones, por lo que puede usar operadores binarios para
combinar expresiones. Una expresión más compleja suele tener varios
operadores y operandos. En este momento, el orden de evaluación de cada
subexpresión en la expresión puede afectar el valor de la expresión. La
precedencia y asociatividad de los operadores garantizan la unicidad del
orden de evaluación de la expresión. Por ejemplo, una expresión
combinada:

.. code:: python

   1 + 10/2 * 3

Las cuatro operaciones aritméticas de uso diario calcularán primero la
expresión de división ``10/2``, luego la expresión de multiplicación y
finalmente la expresión de suma. Esto se debe a que la multiplicación y
la división tienen mayor prioridad que la suma.

Tipo de operando
^^^^^^^^^^^^^^^^

En la operación de expresiones, los operandos pueden tener tipos que no
coincidan con los operadores. Además, los operadores binarios
normalmente requieren que los operandos izquierdo y derecho sean del
mismo tipo. La expresión ``'10'+10`` es incorrecta. No puede agregar una
cadena a un número entero. El problema con la expresión ``-'b'`` es que
no puedes tomar un valor negativo en una cadena. A veces, un operador
binario tiene diferentes tipos de operandos pero puede realizar
operaciones. Por ejemplo, al agregar un número entero a un número real,
el objeto entero se convertirá en un número real y se agregará a otro
objeto de número real. Los operadores lógicos AND y OR lógicos permiten
que los operandos a ambos lados del operador sean de cualquier tipo. En
expresiones lógicas, siempre se convertirán al tipo ``booleano`` de
acuerdo con ciertas reglas.

Otra situación es que los operadores pueden sobrecargarse al usar clases
personalizadas. En esencia, puede interpretar este operador
arbitrariamente y depende de usted decidir qué tipo de operando debe
ser.

3.1.1 Prioridad y asociatividad
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

En una expresión compuesta de varios operadores, la precedencia y la
asociatividad de los operadores determinan el orden de evaluación de las
expresiones. La precedencia y asociatividad de cada operador se dan en
la Tabla 2 .

La precedencia especifica el orden de evaluación entre diferentes
operadores, y las expresiones con operadores de mayor precedencia se
evaluarán primero. Por ejemplo, el proceso de evaluar la expresión
``1+2*3`` calculará primero el resultado de ``2*3`` y luego el resultado
de la expresión de suma. El uso de paréntesis puede mejorar el orden de
evaluación de las expresiones de baja prioridad. Por ejemplo, en la
evaluación de la expresión ``(1+2)*3``, primero se calcula el resultado
de la expresión ``1+2`` entre paréntesis y luego se calcula la expresión
de multiplicación fuera de los paréntesis.

La asociatividad se refiere al orden de evaluación de los operandos en
ambos lados del operador, donde los operandos pueden ser subexpresiones.
Por ejemplo, en la expresión de suma ``expr1 + expr2``, el valor de
``expr1`` se calcula primero y luego el valor de ``expr2``, porque el
operador de suma es asociativo por la izquierda.

.. container::
   :name: tab::operator_list

   +---------------+----------------+----------------+-----------------+
   | **prioridad** | **Operador**   | **Descripción**|**Asociatividad**|
   +===============+================+================+=================+
   | 1             | ``()``         | Símbolo de     | -               |
   |               |                | agrupación     |                 |
   +---------------+----------------+----------------+-----------------+
   | 2             | ``() [] .``    | Operación de   | izquierda       |
   |               |                | campo          |                 |
   +---------------+----------------+----------------+-----------------+
   | 3             | ``-! ~``       | Signo          | izquierda       |
   |               |                | negativo,      |                 |
   |               |                | negación       |                 |
   |               |                | lógica, cambio |                 |
   |               |                | de bit         |                 |
   +---------------+----------------+----------------+-----------------+
   | 4             | ``* / %``      | M              | izquierda       |
   |               |                | ultiplicación, |                 |
   |               |                | división y     |                 |
   |               |                | resto          |                 |
   +---------------+----------------+----------------+-----------------+
   | 5             | ``+ -``        | Suma, resta    | izquierda       |
   +---------------+----------------+----------------+-----------------+
   | 6             | ``<< >>``      | Mover a la     | izquierda       |
   |               |                | izquierda,     |                 |
   |               |                | mover a la     |                 |
   |               |                | derecha        |                 |
   +---------------+----------------+----------------+-----------------+
   | 7             | ``&``          | Bit a bit Y    | izquierda       |
   +---------------+----------------+----------------+-----------------+
   | 8             | ``^``          | XOR bit a bit  | izquierda       |
   +---------------+----------------+----------------+-----------------+
   | 9             | ``\|``         | Bit a bit O    | izquierda       |
   +---------------+----------------+----------------+-----------------+
   | 10            | ``..``         | Operador de    | izquierda       |
   |               |                | concatenación  |                 |
   +---------------+----------------+----------------+-----------------+
   | 11            | ``< <= > >=``  | Mayor que,     | izquierda       |
   |               |                | mayor que o    |                 |
   |               |                | igual a        |                 |
   +---------------+----------------+----------------+-----------------+
   | 12            | ``== !=``      | Igual a, no    | izquierda       |
   |               |                | igual a        |                 |
   +---------------+----------------+----------------+-----------------+
   | 13            | ``&&``         | Y lógico       | izquierda       |
   +---------------+----------------+----------------+-----------------+
   | 14            | ``\|\|``       | O lógico       | izquierda       |
   +---------------+----------------+----------------+-----------------+
   | 15            | ``? :``        | Operador       | derecha         |
   |               |                | condicional    |                 |
   +---------------+----------------+----------------+-----------------+
   | 16            | ``&= \|        | Asignación     | derecha         |
   |               | = ^= <<= >>=`` |                |                 |
   +---------------+----------------+----------------+-----------------+

   *Tabla 2: Lista de operadores*

Utilice corchetes para aumentar la prioridad
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Los paréntesis se pueden usar cuando necesitamos que los operadores con
menor precedencia se evalúen primero. Durante la evaluación de
expresiones, primero se calcula el valor de la expresión entre
paréntesis. En otras palabras, para toda la expresión, la expresión
entre paréntesis es equivalente a un operando, independientemente de la
composición de la expresión entre paréntesis.

3.2 Operador
------------

3.2.1 Operadores aritméticos
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los operadores aritméticos se utilizan para implementar operaciones
aritméticas. Estos operadores son similares a los símbolos matemáticos
que solemos usar. Los operadores aritméticos provistos por Berry se
muestran en la Tabla 3 .

.. container::
   :name: tab::arthmetic_operator

   ============ ======================== ===============
   **Operador** **Descripción**          **Ejemplo**
   ============ ======================== ===============
   ``-``        menos unario             ``- expr``
   ``+``        Concatenación más/cadena ``expr + expr``
   ``-``        signo menos              ``expr-expr``
   ``*``        Signo de multiplicación  ``expr * expr``
   ``/``        Signo de división        ``expr / expr``
   ``%``        Toma el resto            ``expr%expr``
   ============ ======================== ===============

   *Tabla 3: Operador aritmético*

Operador binario ``+`` Además de ser un signo más, también es una
concatenación de cadenas. Cuando el operando de este operador es una
cadena, la concatenación de cadenas se realizará para concatenar dos
cadenas en una cadena más larga. Para ser precisos, ``+`` como
concatenación de cadenas ya no está en la categoría de operadores
aritméticos.

El operador binario ``%`` es el símbolo de resto. Sus operandos deben
ser números enteros. El resultado de la operación de resto es el resto
después de dividir el operando izquierdo por el operando derecho. Por
ejemplo, el resultado de ``11%4`` es ``3``. El tipo de número real no
puede ser divisible, por lo que no se admite el resto.

En general, los operadores aritméticos no cumplen la ley conmutativa.
Por ejemplo, los valores de las expresiones ``2/4`` y ``4/2`` no son
iguales.

Todos los operadores aritméticos se pueden sobrecargar en la clase. Los
operadores sobrecargados no están necesariamente limitados a su diseño
funcional original, sino que son determinados por el programador.

3.2.2 Operadores relacionales
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Los operadores relacionales se utilizan para comparar la magnitud de los
operandos. Los seis operadores relacionales soportados por Berry se dan
en la Tabla 4 .

.. container::
   :name: tab::relop_operator

   ============ ================= ================
   **Operador** **Descripción**   **Ejemplo**
   ============ ================= ================
   ``<``        Menor que         ``expr < expr``
   ``<=``       Menor o igual que ``expr <= expr``
   ``==``       Igual             ``expr == expr``
   ``!=``       No es igual a     ``expr != expr``
   ``>=``       Mayor o igual a   ``expr >= expr``
   ``>``        Mayor qu e        ``-expr``
   ============ ================= ================

   *Tabla 4: Operador relacional*

Al comparar la relación de magnitud de los operandos o juzgar si los
operandos son iguales, la evaluación de la expresión relacional
producirá un resultado booleano. Cuando se cumple la relación, el valor
de la expresión de la relación es “verdadero”, de lo contrario, es
“falso”. Los operadores relacionales ``==`` y ``!=`` pueden usar
cualquier tipo de operando y permiten que los operandos izquierdo y
derecho tengan diferentes tipos. Otros operadores relacionales permiten
el uso de las siguientes combinaciones de operandos:

   **integer** relop **integer**
   
   **real** relop **real**
   
   **integer** relop **real**
   
   **real** relop **integer**
   
   **string** relop **string**

En operaciones relacionales, el signo igual ``==`` y el signo de
desigualdad ``!=`` satisfacen la ley conmutativa. Si los operandos
izquierdo y derecho son del mismo tipo o ambos son de tipo numérico
(número entero y número real), los operandos se consideran iguales según
el valor de los operandos; de lo contrario, los operandos se consideran
desiguales. La igualdad y la desigualdad son operaciones recíprocas: si
``a==b`` es verdadero, entonces ``a!=b`` es falso, y viceversa. Otros
operadores relacionales no satisfacen la ley conmutativa, pero tienen
las siguientes propiedades: ``<`` y ``>=`` son operaciones recíprocas, y
``>`` y ``<=`` son operaciones recíprocas. Las operaciones relacionales
requieren que los operandos sean del mismo tipo, de lo contrario es una
expresión incorrecta.

Las instancias pueden sobrecargar a los operadores como métodos. Si el
operador relacional está sobrecargado, el programa debe garantizar las
propiedades anteriores.

Entre los operadores relacionales, los operadores ``==`` y ``!=`` tienen
requisitos más relajados que ``<``, ``<=``, ``>`` y ``>=``, que solo
permiten comparaciones entre los mismos tipos. En el desarrollo de un
programa real, el juicio de igualdad o desigualdad suele ser más simple
que el juicio de tamaño. Es posible que algunos objetos de operación no
puedan juzgar el tamaño, pero solo pueden juzgar la igualdad o
desigualdad. Este es el caso del tipo booleano.

Operadores logicos
~~~~~~~~~~~~~~~~~~

Los operadores lógicos se dividen en tres tipos: AND lógico, OR lógico y
NOT lógico. Como se muestra en la Tabla 5 .

.. container::
   :name: tab::logic_operator

   ============ =============== =================
   **Operador** **Descripción** **Ejemplo**
   ============ =============== =================
   ``&&``       Y lógico        ``expr && expr``
   ``\|\|``     O lógico        ``expr\|\| expr``
   ``!``        Negación lógica ``!expr``
   ============ =============== =================

   *Tabla 5: Operadores logicos*

Para el operador lógico AND, cuando los valores de ambos operandos son
“verdaderos”, el valor de la expresión lógica es “verdadero”, de lo
contrario, es “falso”.

Para el operador lógico OR, cuando los valores de ambos operandos son
``falso``, el valor de la expresión lógica es ``falso``, de lo contrario
es ``verdadero``.

El papel del operador de negación lógica es cambiar el estado lógico del
operando. Cuando el valor del operando es “verdadero”, el valor de la
expresión lógica es “falso”, de lo contrario, el valor es “verdadero”.

Los operadores lógicos requieren que el operando sea de tipo booleano, y
si el operando no es de tipo booleano, se convertirá. Consulte la
sección [Capitulo-2: Tipo booleano] para conocer las reglas de
conversión.

Las operaciones lógicas utilizan una estrategia de evaluación llamada
**Evaluación de cortocircuito**. Esta estrategia de evaluación es: para
el operador lógico AND, el segundo operando se evaluará si y solo si el
operando de la izquierda es verdadero; para el operador lógico OR, si y
solo si el operando izquierdo es falso evaluará el operando derecho. La
naturaleza de la evaluación de cortocircuito hace que no se ejecute todo
el código de la expresión lógica.

Operador bit a bit
~~~~~~~~~~~~~~~~~~

Los operadores de bits pueden implementar algunas operaciones de bits
binarios, y las operaciones de bits solo se pueden usar en tipos
enteros. La información detallada de los operadores de bit se muestra en
la Tabla 6 . La operación de bits se refiere a la operación de bits
binarios directamente en números enteros. Las operaciones lógicas se
pueden extender a operaciones de bits. Tomando AND lógico como ejemplo,
podemos realizar esta operación en cada bit binario para lograr AND bit
a bit, como 110\ :sub:`b`\  AND 011\ :sub:`b`\  = 010\ :sub:`b`\ . Las operaciones
de bits también admiten operaciones de cambio, que mueven números de
forma binaria.

.. container::
   :name: tab::bitwise_operator

   ============ ======================== ================
   **Operador**                          **Ejemplo**
   ============ ======================== ================
   ``~``        Negar                    ``~expr``
   ``&``        Bit a bit y              ``expr & expr``
   ``\|``       Bit a bit o              ``expr\| expr``
   ``^``        O exclusivo bit a bit    ``expr^expr``
   ``<<``       Desplazar a la izquierda ``expr << expr``
   ``>>``       Desplazar a la derecha   ``expr >> expr``
   ============ ======================== ================

   *Tabla 6: Operador bit a bit*

Aunque solo se puede usar para números enteros, las operaciones con bits
siguen siendo versátiles. Las operaciones de bits pueden implementar
muchas técnicas de optimización. En muchos algoritmos, el uso de
operaciones de bits puede ahorrar mucho código. Por ejemplo, para
determinar si un número ``n`` es una potencia de 2, podemos juzgar si el
resultado de ``n & (n - 1)`` es ``0``. En algunos lenguajes con alta
eficiencia de ejecución, las operaciones de cambio también se pueden
usar para optimizar la multiplicación y la división (por lo general, no
hay un efecto obvio en los lenguajes de secuencias de comandos).

El operador AND bit a bit "``&``" es un operador binario, que realiza la
operación AND binaria de dos operandos enteros: solo cuando los bits
binarios correspondientes a los operandos son todos ``1``, el resultado
es ``1``. Por ejemplo, 1110\ :sub:`b`\  & 0111\ :sub:`b`\  = 0110\ :sub:`b`\ .

El operador OR bit a bit "``|``" es un operador binario, que realiza una
operación OR de bits binarios en dos operandos enteros: solo cuando los
bits binarios correspondientes a los operandos son ambos ``0``, el bit
del resultado es ``0``. Por ejemplo, 1000\ :sub:`b`\  \| 0001\ :sub:`b`\  = 1001\ :sub:`b`\ .

El operador OR exclusivo bit a bit "``^``" es un operador binario, que
realiza una operación OR exclusiva binaria en dos operandos enteros:
cuando los bits binarios correspondientes a los operandos son
diferentes, el valor de bit del resultado es ``1``. Por ejemplo, 
1100\ :sub:`b`\  \^ 0101\ :sub:`b`\  = 1001\ :sub:`b`\ .

El operador de desplazamiento a la izquierda "``<<``" es un operador
binario, que mueve el operando izquierdo hacia la izquierda el número de
bits especificado por el operando derecho sobre una base binaria. Por
ejemplo, 00001010\ :sub:`b`\  << 3 = 01010000\ :sub:`b`\ . El operador de
desplazamiento a la derecha “``>>``” es un operador binario, que
desplaza el operando izquierdo hacia la derecha el número de bits
especificado por el operando derecho en un binario. base. Por ejemplo,
10100000\ :sub:`b`\  >> 3 = 00010100\ :sub:`b`\ .

El operador de inversión bit a bit "``~``" es un operador unario, y el
resultado de la expresión es invertir el valor de cada bit binario del
operando. Por ejemplo, ``∼``\ 10100011\ :sub:`b`\  = 01011100\ :sub:`b`\ .

Los siguientes son algunos ejemplos del uso de operaciones con bits. Por
lo general, no usamos binario directamente. Los resultados de los
ejemplos se han convertido en bases comunes.

.. code:: berry

   1 << 1 # 2
   168 >> 4 # 10
   456 & 127 # 72
   456 | 127 # 511
   0xA5 ^ 0x5A # 255
   ~2 # -3

Operador de asignación
~~~~~~~~~~~~~~~~~~~~~~

El operador de asignación solo aparece en la expresión de asignación y
el operando del operador debe ser un objeto de escritura. La expresión
de asignación no tiene resultado, por lo que no se pueden utilizar
operaciones de asignación continua.

Operador de asignación simple
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

El operador de asignación simple ``=`` se puede utilizar para la
asignación de variables. Si la variable de operando de la izquierda no
está definida, se definirá la variable. El operador de asignación se
utiliza para vincular el valor del operando derecho con el operando
izquierdo. Este proceso también se llama “asignación”. Por lo tanto, el
operando izquierdo no puede ser una constante, ni puede ser ningún
objeto que no se pueda escribir. Estas son algunas expresiones legales
de asignación:

.. code:: berry

   a = 45 b ='string' c = 0

Y la siguiente expresión de asignación es incorrecta:

.. code:: berry

   1 = 5 # Tratando de asignar una constante 1
   a = b = 0 #  Asignación continua

Al asignar tipos ``nil``, enteros, reales y booleanos a variables, el
valor del objeto se pasará al operando izquierdo, pero para otros tipos,
la operación de asignación simplemente pasa la referencia del objeto al
operando izquierdo. Dado que las cadenas, las funciones y los tipos de
clase son de solo lectura, todas las referencias que pasan no tendrán
efectos secundarios, pero debe tener mucho cuidado con los tipos de
instancia.

Operador de asignación compuesto
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Los operadores de asignación compuestos son operadores que combinan
operadores binarios y operadores de asignación. Son extensiones
prácticas de operadores de asignación simples. Los operadores de
asignación compuestos pueden simplificar la escritura de algunas
expresiones. La Tabla 7 enumera todos los operadores de asignación
compuestos

.. container::
   :name: tab::compound_assign

   ============ ===========================================
   **Operador** **Descripción**
   ============ ===========================================
   ``+=``       Asignación de adición
   ``-=``       Asignación de resta
   ``*=``       Asignación de multiplicación
   ``/=``       Asignación de división
   ``%=``       Asignación de resto
   ``&=``       Asignación AND bit a bit
   ``\|=``      Asignación OR bit a bit
   ``^=``       Asignación XOR bit a bit
   ``<<=``      Asignación de desplazamiento a la izquierda
   ``>>=``      Asignación de desplazamiento a la derecha
   ============ ===========================================

   *Tabla 7: Operador de bits*

La expresión de asignación compuesta realiza la operación binaria
correspondiente al operador de asignación compuesta en el operando
izquierdo y el operando derecho, y luego asigna el resultado al operando
izquierdo. Tomando ``+=`` como ejemplo, la expresión ``a += b`` es
equivalente a ``a = a + b``. El operador de asignación compuesto también
es un operador de asignación, por lo que tiene una prioridad más baja.
El operador binario correspondiente al operador de asignación compuesto
siempre se evalúa después del operando derecho, por lo que una expresión
como ``a *= 1 + 2`` debería ser equivalente a ``a = a * (1 + 2)``.

A diferencia del operador de asignación simple, el operando izquierdo
del operador de asignación compuesto debe participar en la evaluación,
por lo que la expresión de asignación compuesta no tiene la función de
definir variables. El operador de asignación en sí no se puede
sobrecargar en la clase. Los usuarios solo pueden sobrecargar el
operador binario correspondiente al operador de asignación compuesto.
Esto también asegura que el operador de asignación compuesto siempre se
ajustará a las características básicas de las operaciones de asignación.

Operador de dominio y operador de subíndice

El operador de dominio ``.`` se utiliza para acceder a un atributo o
miembro de un objeto. Puede utilizar operadores de dominio para ambos
tipos de módulos e instancias:

.. code:: berry

   l = list[]
   l.push('item 0')
   s = l.item(0) # 'item 0'

El operador de subíndice ``[]`` se utiliza para acceder a los elementos
de un objeto, por ejemplo

.. code:: berry

   l[2] = 10 # Read by index
   n = l[2] # Write by index

Las clases que admiten la lectura de subíndices deben implementar el
método ``item`` y las clases que admiten la escritura de subíndices
deben implementar el método ``setitem``. El mapa y la lista en el
contenedor estándar implementan estos dos métodos, por lo que admiten la
lectura y escritura mediante el operador de subíndice. La cadena admite
la lectura de subíndices, pero no admite la escritura de subíndices (las
cadenas son valores de solo lectura):

.. code:: berry

   'string'[2] #'r'
   'string'[2] ='a' # error:  valor 'string' no admite asignación de índice

Actualmente, las cadenas admiten subíndices enteros y el rango de
subíndices no puede exceder la longitud de la cadena.

Operador condicional
~~~~~~~~~~~~~~~~~~~~

El operador condicional (``? :``) es similar a la declaración **if
else**, pero la primera puede usarse en expresiones. La forma de uso del
operador condicional es:

.. code:: 

   cond ? expr1 : expr2

**cond** es la expresión utilizada para juzgar la condición. El proceso
de evaluación del operador condicional es: primero encuentra el valor de
**cond**, si la condición es verdadera, evalúa **expr1** y devuelve el
valor, de lo contrario, el valor de **expr2** ] Evalúa y devuelve el
valor. **expr1** y **expr2** pueden tener diferentes tipos, por lo que
lo siguiente es correcto:

.. code:: berry

   resultado = alcance < 6 ? 'malo' : alcance

Esta expresión primero determina si ``alcance`` es menor que ``6``, si
lo es, devuelve ``malo``, de lo contrario, devuelve el valor de
``alcance``. Independientemente de la condición de la expresión
condicional, solo se ejecutará uno de **expr1** o **expr2**, similar a
la característica de cortocircuito de las operaciones lógicas AND y
lógicas OR.

Operadores de condiciones anidadas
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Un operador condicional se puede anidar en otro operador condicional, es
decir, la expresión condicional se puede usar como **cond** o **expr**
de otra expresión condicional. Por ejemplo, use expresiones
condicionales para dividir puntajes en tres niveles: excelente, bueno y
malo:

.. code:: berry

   resultado = alcance >= 9 ? 'excelente' : alcance >= 6 ? 'bueno' : 'malo'

La primera condición comprueba si la puntuación no es inferior a ``9``
puntos. Si es así, ejecute la rama después de ``?`` y devuelva
``'excelente'``; de lo contrario, ejecute la rama después de ``:``, que
también es una expresión condicional. La condición comprueba si la
puntuación no es inferior a ``6``, si lo es, devuelve ``'bueno'``, de lo
contrario, devuelve ``'malo'``.

El operador condicional satisface la asociatividad correcta, por lo que
el valor de la expresión de bifurcación debe evaluarse primero para
obtener el valor de la expresión condicional. Por lo tanto, en una
expresión condicional anidada, la expresión condicional anidada se
evalúa primero y luego se evalúa la expresión condicional externa.

Prioridad de los operadores condicionales
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Dado que la precedencia de las expresiones condicionales es muy baja
(sólo superada por los operadores de asignación), a menudo es necesario
agregar paréntesis fuera de las expresiones condicionales. Por ejemplo,
cuando se usa una expresión condicional como operando de una expresión
aritmética, los paréntesis tendrán diferentes efectos en el resultado:

.. code:: berry

   resultado = 10 * (signo < 0 ? -1 : 1) # el resultado es -10 ó 10
   resultado = 10 * signo < 0 ? -1 : 1 # el resultado es -1 ó 1

El resultado de la primera expresión es correcto, y la segunda expresión
toma ``10 * signo < 0`` como condición a juzgar, lo que no cumple con la
expectativa de la expresión condicional como el operando derecho de la
multiplicación.

Operador de concatenación
~~~~~~~~~~~~~~~~~~~~~~~~~

.. _operador-1:

Operador ``+``
^^^^^^^^^^^^^^

Cuando los operandos izquierdo y derecho son cadenas, el operador ``+``
se usa para conectar las dos cadenas, y la nueva cadena obtenida es el
valor de la expresión. Por lo tanto, este operador se usa a menudo para
la concatenación de cadenas:

.. code:: berry

   resultado ='abc' + '123' # el resultado es 'abc123'

Los operadores ``+`` también se pueden usar para conectar dos instancias
de lista:

.. code:: berry

   resultado = [1, 2] + [3, 4] # el resultado es [1, 2, 3, 4]

A diferencia del método ``list.push``, el operador ``+`` fusiona dos
listas en un objeto de lista más grande, con los elementos del operando
izquierdo al principio de la lista de resultados y los elementos del
operando derecho al final de la lista de resultados

Operador ``..``
^^^^^^^^^^^^^^^

``..`` es un operador especial. Si el operando izquierdo es una cadena,
el comportamiento de la expresión es concatenar los operandos izquierdo
y derecho en una nueva cadena (conversión automática si el operando
derecho no es una cadena):

.. code:: berry

   resultado ='abc' .. 123 # el resultado es 'abc123'

El operador ``..`` se usa a menudo cuando se concatena una cadena y un
valor que no es una cadena. Si el operando izquierdo es una instancia de
lista, el operador ``..`` agregará el operando derecho al final de la
lista y luego usará esta lista como el valor de la expresión:

.. code:: berry

   resultado = [1, 2] .. 3 # el resultado es [1, 2, 3]

Este proceso modificará directamente el operando izquierdo, que es muy
similar al método ``push`` de ``list`` (excepto las cadenas que son
objetos inmutables). La operación de unión de la lista se puede ejecutar
en cadena:

.. code:: berry

   resultado = [1, 2] .. 3 .. 4 # el resultado es [1, 2, 3, 4]

Todos los valores en este proceso se agregarán al objeto de lista más a
la izquierda.

Si los operandos izquierdo y derecho son enteros, utilice el operador
``..`` para obtener un objeto de rango de enteros:

.. code:: berry

   resultado = 1 .. 10 # el resultado es (1..10)

Este objeto se utiliza para representar un intervalo cerrado de enteros,
donde el operando izquierdo es el límite inferior y el operando derecho
es el límite superior. Dichos objetos de rango de enteros se utilizan a
menudo para la iteración.
