The Berry Script Language Reference Manual
##########################################

Preface
^^^^^^^

A few years ago, I tried to port the Lua scripting language to STM32F4 microcontroller, and then I experienced a Lua-based firmware on ESP8266: NodeMCU. These experiences made me experience the convenience of script development. Later, I came into contact with some scripting languages, such as Python, JavaScript, Basic, and MATLAB. At present, only a few languages are suitable for transplanting to the microcontroller platform. I used to pay more attention to Lua because it is a very compact embedded scripting language, and its design goal is to be embedded in the host program. However, for the microcontroller, the Lua interpreter may not be small enough, and I cannot run it on a 32-bit microcontroller with a relatively small memory. To this end, I started to read Lua code and developed my own scripting language-Berry on this basis.

This is an ultra-lightweight embedded scripting language, it is also a multi-paradigm dynamic language. Supports object-oriented, process-oriented and functional (less support) styles. Many aspects of this language refer to Lua, but its syntax also borrows from the design of other languages. If the reader already knows a high-level language, Berry’s grammar should be very easy to grasp: the language has only some simple rules and a very natural scope design.

The main application scenario I consider is embedded systems with low performance. The memory of these systems may be very small, so it is very difficult to run a full-featured scripting language. This means that we may have to make a choice. Berry may only provide the most commonly used and basic core functions, while other unnecessary features are only used as optional modules. This will inevitably lead to the language’s standard library being too small, even the language There will also be uncertain designs (such as the implementation of floating-point numbers and integers, etc.). The benefits of these trade-offs are more room for optimization, while the disadvantage is obviously the inconsistency of language standards.
Berry’s interpreter refers to the implementation of Lua interpreter, which is mainly divided into two parts: compiler and virtual machine. Berry’s compiler is a one-pass compiler to generate bytecode. This solution does not generate an abstract syntax tree, so it saves memory. The virtual machine is a register type. Generally speaking, the register type virtual machine is more efficient than the stack type virtual machine. In addition to implementing language features, we also hope to optimize the memory usage and operating efficiency of the interpreter. At present, the performance indicators of Berry interpreter are not comparable to those of mainstream languages, but the memory footprint is very small.

It wasn’t until later that I learned about the MicroPython project: a simplified Python interpreter designed for microcontrollers. Nowadays, Python is very hot, and this Python interpreter designed for microcontrollers is also very popular. Compared with the current mature technology, Berry is a new language without a sufficient user base. Its advantage is that it is easy to master the grammar and may have advantages in terms of resource consumption and performance.

If you need to port the Berry interpreter, you need to ensure that the compiler you use provides support for the C99 standard (I previously fully complied with C89, and some optimization work later made me give up this decision). At present, most compilers provide support for C99, and the common compilers such as ARMCC (KEIL MDK), ICC (IAR) and GCC in the development of ARM processors also support C99.
This document introduces Berry’s grammar rules, standard library and other facilities, and finally guides readers to transplant and extend Berry. This document does not explain the implementation mechanism of the interpreter, and may be explained in other documents if we have time.

The author’s level is limited, and the writing is in a hurry. If there are any omissions or errors in the article, I hope the readers will not hesitate.

Guan Wenliang

April 2019

.. toctree::
   :hidden:
   
   Chapter 1 — Basic Information <Chapter-1>
   Chapter 2 — Types and Variables <Chapter-2>
   Chapter 3 — Expression <Chapter-3>
   Chapter 4 — Statement <Chapter-4>
   Chapter 5 — Function <Chapter-5>
   Chapter 6 — Object Oriented Function <Chapter-6>
   Chapter 7 — Libraries and Modules <Chapter-7>
   Chapter 8 — Advanced features <Chapter-8>
   Chapter 9 — Language Interactive Interface <Chapter-9>
   Appendix A — Grammar Definition <Appendix-A>
   Appendix B — Compiler Interpreter <Appendix-B>
   Appendix C — Porting Guide <Appendix-C>

