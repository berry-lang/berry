.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

Hacer una función nativa
========================

La FFI en C (interfaz de función externa) de Berry opera en una pila
virtual para interactuar con la máquina virtual. Si necesitamos hacer
una función ``add`` para sumar dos números y usarla en Berry de esta
manera:

.. code:: ruby

   result = add(1, 2)

Necesitamos saber cómo el código C obtiene los argumentos de la llamada
a la función Berry y cómo devolver el valor.

Los argumentos de la función se almacenan en una pila, y desde el primer
argumento hasta el último argumento de la función se almacenan desde la
parte inferior de la pila hasta la parte superior de la pila. Si desea
utilizar C para obtener elementos de la pila, utilice el siguiente
conjunto de FFI:

.. code:: c

   int be_toint(bvm *vm, int index);
   breal be_toreal(bvm *vm, int index);
   int be_tobool(bvm *vm, int index);
   const char* be_tostring(bvm *vm, int index);
   void* be_tocomptr(bvm *vm, int index);

Si desea probar si un valor en la pila es de un tipo específico, use el
siguiente conjunto de FFI:

.. code:: c

   int be_isnil(bvm *vm, int index);
   int be_isbool(bvm *vm, int index);
   int be_isint(bvm *vm, int index);
   int be_isreal(bvm *vm, int index);
   int be_isnumber(bvm *vm, int index);
   int be_isstring(bvm *vm, int index);
   int be_isclosure(bvm *vm, int index);
   int be_isntvclos(bvm *vm, int index);
   int be_isfunction(bvm *vm, int index);
   int be_isproto(bvm *vm, int index);
   int be_isclass(bvm *vm, int index);
   int be_isinstance(bvm *vm, int index);
   int be_islist(bvm *vm, int index);
   int be_ismap(bvm *vm, int index);
   int be_iscomptr(bvm *vm, int index);

Si necesita enviar valores a la pila, use estos FFI:

.. code:: c

   void be_pushnil(bvm *vm);
   void be_pushbool(bvm *vm, int b);
   void be_pushint(bvm *vm, bint i);
   void be_pushreal(bvm *vm, breal r);
   void be_pushstring(bvm *vm, const char *str);
   void be_pushnstring(bvm *vm, const char *str, size_t n);
   const char* be_pushfstring(bvm *vm, const char *format, ...);
   void be_pushvalue(bvm *vm, int index);
   void be_pushntvclosure(bvm *vm, bntvfunc f, int nupvals);
   void be_pushntvfunction(bvm *vm, bntvfunc f);
   void be_pushclass(bvm *vm, const char *name, const bnfuncinfo *lib);
   void be_pushcomptr(bvm *vm, void *ptr);

``index`` es la posición del elemento en la pila, un valor positivo es
el desplazamiento desde la parte inferior de la pila hasta la parte
superior de la pila, y un valor negativo es el desplazamiento desde la
parte superior de la pila hasta la parte inferior de la pila.

El valor de retorno utiliza dos FFI:

.. code:: c

   be_return(vm)
   be_return_nil(vm)

Estos FFI son en realidad macros. ``be_return`` devuelve el objeto en la
parte superior de la pila, y ``be_return_nil`` devuelve ``nil``.

Estas FFI se definen en berry.h.

Ahora implementemos la función ``add``:

.. code:: c

   int my_add_func(bvm *vm)
   {
       /* comprobar que los argumentos son todos enteros */
       if (be_isint(vm, 1) && be_isint(vm, 2)) {
           bint a = be_toint(vm, 1); /* obtener el primer argumento */
           bint b = be_toint(vm, 2); /* obtener el segundo argumento */
           be_pushint(vm, a + b); /* empuja el resultado a la pila */
       } else if (be_isnumber(vm, 1) && be_isnumber(vm, 2)) { /* comprobar que los argumentos son todos números */
           breal a = be_toreal(vm, 1); /* obtener el primer argumento */
           breal b = be_toreal(vm, 1); /* empuja el resultado a la pila */
           be_pushreal(vm, a + b); /* empuja el resultado a la pila */
       } else { /* parámetros inaceptables */
           be_pushnil(vm); /* empuja nil a la pila */
       }
       be_return(vm); /* devuelve el resultado del cálculo */
   }

Luego regístrelo en el lugar apropiado:

.. code:: c

   be_regcfunc(vm, "add", my_add_func);

Crear una instancia de un objeto ``list`` en una función nativa
===============================================================

La generación de clases nativas instanciadas en C puede ser engorrosa en
comparación con los tipos simples. Esta sección guiará al lector a
instanciar la clase ``list``.

La clase ``list`` es un contenedor alrededor de la estructura de la
lista, que tiene una propiedad ``.data`` para la estructura de la lista.
Por lo tanto, primero necesitamos construir una estructura de lista:

.. code:: c

   be_newlist(vm);

La función ``be_newlist`` construye un valor de tipo ``BE_LIST``.
Entonces podemos operar sobre los datos:

.. code:: c

   be_pushint(vm, 100);
   be_data_append(vm, -2);
   be_pop(vm, 1); /* extraer el entero 100 */

Las dos primeras líneas de código se utilizan para añadir el entero
``100`` a la lista, y la tercera línea del entero ``100`` se extrae para
facilitar las operaciones posteriores.

Dado que el tipo ``BE_LIST`` no se puede usar directamente en Berry,
pero lo usa la clase ``list``, tenemos que construir la clase ``list``
para él:

.. code:: c

   be_getglobal(vm, "list");
   be_pushvalue(vm, -2); /* empuja los datos de la lista al principio */
   be_call(vm, 1); /* llama al constructor */

El constructor de la clase ``list`` permite el uso del parámetro de tipo
``BE_LIST``, que toma el argumento como datos de lista.

El código completo es el siguiente:

.. code:: c

   int m_listtest(bvm *vm)
   {
       be_getglobal(vm, "list");
       be_newlist(vm);
       be_pushint(vm, 100);
       be_data_append(vm, -2);
       be_pop(vm, 1);
       be_call(vm, 1);
       be_pop(vm, 1); /* pop the arguments */
       be_return(vm);
   }

Registre la función nativa en el lugar apropiado:

.. code:: c

   be_regcfunc(vm, "listtest", m_listtest);
