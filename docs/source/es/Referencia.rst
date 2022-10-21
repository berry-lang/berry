Manual de referencia del lenguaje Berry Script
##############################################

Prefacio
^^^^^^^^

Hace algunos años, traté de portar el lenguaje de script Lua al microcontrolador STM32F4 y luego experimenté un firmware basado en Lua en ESP8266: NodeMCU. Estas experiencias me hicieron experimentar la conveniencia del desarrollo de guiones. Más tarde, entré en contacto con algunos lenguajes de script, como Python, JavaScript, Basic y MATLAB. En la actualidad, solo unos pocos lenguajes son adecuados para portar a la plataforma de microcontroladores. Solía prestar más atención a Lua porque es un lenguaje de script integrado muy compacto y su objetivo de diseño es estar integrado en el programa host. Sin embargo, para el microcontrolador, el intérprete de Lua puede no ser lo suficientemente pequeño y no puede ser ejecutado en un microcontrolador de 32 bits con una memoria relativamente pequeña. Con este fin, comencé a leer el código Lua y desarrollé mi propio lenguaje de script, Berry, sobre esta base.

Este es un lenguaje de script integrado ultraligero, también es un lenguaje dinámico multiparadigma. Admite estilos orientados a objetos, orientados a procesos y funcionales (menos soportado). Muchos aspectos de este lenguaje se refieren a Lua, pero su sintaxis también se inspira en el diseño de otros lenguajes. Si el lector ya conoce un lenguaje de alto nivel, la gramática de Berry debería ser muy fácil de entender: el lenguaje tiene solo algunas reglas simples y un diseño de alcance muy natural.

El principal escenario de aplicación que considero son los sistemas integrados con bajo rendimiento. La memoria de estos sistemas puede ser muy pequeña, por lo que es muy difícil ejecutar un lenguaje de script con todas las funciones. Esto significa que es posible que tengamos que hacer una elección. Es posible que Berry solo proporcione las funciones principales básicas y más utilizadas, mientras que otras características innecesarias solo se usan como módulos opcionales. Esto conducirá inevitablemente a que la biblioteca estándar del idioma sea demasiado pequeña, incluso el lenguaje. También habrá diseños inciertos (como la implementación de números de punto flotante y enteros, etc.). Los beneficios de estas compensaciones son más espacio para la optimización, mientras que la desventaja es obviamente la inconsistencia de los estándares del idioma.
El intérprete de Berry se refiere a la implementación del intérprete de Lua, que se divide principalmente en dos partes: compilador y máquina virtual. El compilador de Berry es un compilador de un solo paso para generar bytecode. Esta solución no genera un árbol de sintaxis abstracto, por lo que ahorra memoria. La máquina virtual es de tipo registro. En términos generales, la máquina virtual de tipo registro es más eficiente que la máquina virtual de tipo pila. Además de implementar funciones de lenguaje, también esperamos optimizar el uso de la memoria y la eficiencia operativa del intérprete. En la actualidad, los indicadores de rendimiento del intérprete de Berry no son comparables con los de los lenguajes principales, pero la huella de memoria es muy pequeña.

No fue hasta más tarde que me enteré del proyecto MicroPython: un intérprete de Python simplificado diseñado para microcontroladores. Hoy en día, Python está muy de moda, y este intérprete de Python diseñado para microcontroladores también es muy popular. En comparación con la tecnología madura actual, Berry es un lenguaje nuevo sin una base de usuarios suficiente. Su ventaja es que es fácil de dominar la gramática y puede tener ventajas en términos de consumo de recursos y rendimiento.

Si necesita migrar el intérprete de Berry, debe asegurarse de que el compilador que usa brinde soporte para el estándar C99 (anteriormente cumplia completamente con C89, y algunos trabajos de optimización más tarde me hicieron renunciar a esta decisión). En la actualidad, la mayoría de los compiladores brindan soporte para C99, y los compiladores comunes como ARMCC (KEIL MDK), ICC (IAR) y GCC en el desarrollo de procesadores ARM también admiten C99.
Este documento presenta las reglas gramaticales de Berry, la biblioteca estándar y otras instalaciones, y finalmente guía a los lectores para trasplantar y ampliar Berry. Este documento no explica el mecanismo de implementación del intérprete, y puede ser explicado en otros documentos, si tenemos tiempo.

El nivel del autor es limitado y la escritura apresurada. Si hay omisiones o errores en el artículo, espero que los lectores no duden en corregirlos.

Guan Wenliang

abril 2019

.. toctree::
   :hidden:
   
   Capítulo 1 - Información básica <Capitulo-1>
   Capítulo 2 - Tipos y variables <Capitulo-2>
   Capítulo 3 - Expresión <Capitulo-3>
   Capítulo 4 - Declaración <Capitulo-4>
   Capítulo 5 - Función <Capitulo-5>
   Capítulo 6 - Función orientada a objetos <Capitulo-6>
   Capítulo 7 - Bibliotecas y módulos <Capitulo-7>
   Capítulo 8 - Funciones avanzadas <Capitulo-8>
   Capítulo 9 - Interfaz interactiva del lenguaje <Capitulo-9>
   Apéndice A - Definición de gramática <Apendice-A>
   Apéndice B - Intérprete del compilador <Apendice-B>
   Apéndice C - Guía de Portabilidad <Apendice-C>