.. raw:: html

   <!-- Spanish Translation: Emiliano Gonzalez (egonzalez . hiperion @ gmail . com) -->

Hoja de ruta
============

POR HACER
---------

-  ☒ Soporte multilínea REPL.
-  ☒ Información de depuración en tiempo de ejecución.
-  ☒ Protección de pila API.
-  ☒ Soporte de operación de archivos.
-  ☒ Tabla hash fija (basada en ROM).
-  ☒ Soporte de destructor.
-  ☒ Compatibilidad con módulos nativos (use ``import xxx`` para
   importar un módulo).
-  ☒ Soporte de expresiones condicionales.
-  ☒ Función anónima.
-  ☒ Operación bit a bit.
-  ☒ Sentencia de asignación compuesta.
-  ☒ Alcance incorporado.
-  ☒ Función de argumentos variables.
-  ☒ Función nativa: ``classof(obj)``.
-  ☐ [STRIKEOUT:Función nativa: ``copy(obj)``].
-  ☐ Pila de llamadas de liberación automática.
-  ☐ Expresión regular.
-  ☒ GC optimizado para el uso de la pila (sin recursividad).
-  ☒ Simplifica los mensajes de error de desbordamiento de pila.
-  ☒ Expresión lambda.
-  ☒ Manejo excepcional.
-  ☒ Compatibilidad con archivos de bytecode.
-  ☒ Iterador optimizado y sentencia ``for``.
-  ☒ Operador de conexión (redefine el operador de rango ``..``).

   -  ☒ Conexión de cadena, por ejemplo, ``'cadena' .. expr``.
   -  ☒ Lista de conexiones, por ejemplo, ``[] .. expr``.
   -  ☒ Método de serialización de lista (para conexión de cadena de
      alto rendimiento).

-  ☒ Soporte de módulo completo.

   -  ☒ Exportación/importación del módulo de archivo de script.
   -  ☒ Compilación/importación del módulo de archivo de bytecode.
   -  ☒ Carga del módulo nativo compartido (como *.so,*.dll).

-  ☒ API de verificación de igualdad universal (como el operador ``==``
   pero se usa en C).

   -  ☒ Coincidencia de clave de mapa completa.
   -  ☒ Coincidencia de valor de excepción completa.

-  ☐ Compatibilidad con el depurador.
-  ☐ Mecanismo completo de clases y objetos.
-  ☐ Mensaje de error de soporte para la herramienta *map_build*.
-  ☒ Llamada sobrecargada al operador ``()``.

Versión de lanzamiento
----------------------

V0.2.0
~~~~~~

-  ☐ Documentación china completa.
-  ☒ Manejo excepcional.
-  ☒ Soporte de módulo completo.

V0.1.0
~~~~~~

-  ☒ Interfaz de sistema de archivos portátil.
-  ☒ Compatibilidad con objetos constantes precompilados.
-  ☒ Módulo constante completo precompilado.
-  ☐ [STRIKEOUT:Documentación más completa (chino)].
-  ☐ [STRIKEOUT:Guía de portabilidad].
