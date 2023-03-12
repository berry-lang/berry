<p align="center"><img src="docs/berry-logo.png" alt="Berry language logo" width=272 height=128></p>
<p align="center">The Berry Scripting Language</p>

## Introduction

Berry is a ultra-lightweight dynamically typed embedded scripting language. It is designed for lower-performance embedded devices. The Berry interpreter-core's code size is less than 40KiB and can run on less than 4KiB heap (as tested on ARM Cortex M4 CPU, Thumb ISA and ARMCC compiler).

The interpreter of Berry includes a one-pass compiler and register-based VM. All of it's code is written in ANSI C99 and integrates well with both C and C++. In order to make the VM more performant Berry's primitives consist of both simple types (`int`, `real`, `boolean`, `string`) and class ones (`list`, `map`, `range`).

Berry is:

* **Lightweight** - it has a well-optimized interpreter using very little resources. It's ideal for use in microprocessors
* **Fast** - it has optimized one-pass bytecode compiler and a register-based virtual machine.
* **Powerful** - supports both imperative programming, object-oriented programming and functional programming paradigms reasonably well.
* **Flexible** - Berry is a dynamically typed, scripting language, and it's intended for embedding in various applications.
* **Simple for end user** - Berry's simple and natural syntax and support garbage collection make it very easy to learn for your users.
* **Easy to integrate** - it's Foreign Function Interface and APIs are exposing well known virtual machine concepts in a straightforward way.
* **RAM saving** - with it's compile-time object construction, most of the constant objects are stored in read-only code data segments, so the RAM usage of the virtual machine stays as low as possible.

## Documentation

[Berry Language user manual](https://berry.readthedocs.io/language_manual/)

[Virtual Machine user manual](https://berry.readthedocs.io/vm_manual/)

[Virtual Machine and language developer manual](https://berry.readthedocs.io/developer_manual/)

## Editor plugins

[Visual Studio Code](https://code.visualstudio.com/) plugin is in [./tools/plugins/vscode](./tools/plugins/vscode).

Berry's EBNF grammar definition: [tools/grammar/berry.ebnf](./tools/grammar/berry.ebnf)

## Features

* Base Types
  * Nil: `nil`
  * Boolean: `true` and `false`
  * Numerical: Integer (`int`) and Real (`real`)
  * String: Single quotation-mark string and double quotation-mark string
  * Class: Instance template, read only
  * Instance: Object constructed by class
  * Module: Read-write key-value pair table
  * List: Ordered container, like `[1, 2, 3]`
  * Map: Hash Map container, like `{ 'a': 1, 2: 3, 'map': {} }`
  * Range: include a lower and a upper integer value, like `0..5`
* Operators and expressions
  * Assign operator: `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=`
  * Relational operator: `<`, `<=`, `==`, `!=`, `>`, `>=`
  * Logic operator: `&&`, `||`, `!`
  * Arithmetic operator: `+`, `-`, `*`, `/`, `%`
  * Bitwise operator: `&`, `|`, `~`, `^`, `<<`, `>>`
  * Field operator: `.`
  * Subscript operator: `[]`
  * Connect string operator: `+`
  * Conditional operator: `? :`
  * Brackets: `()`
* Control structures
  * Conditional statement: `if-else`
  * Iteration statement: `while` and `for`
  * Jump statement: `break` and `continue`
* Functions
  * Local variable and block scope
  * Return statement
  * Nested functions definition
  * Closure based on Upvalue
  * Anonymous function
  * Lambda expression
* Classes
  * Inheritance (only public single inheritance)
  * Method and Operator Overload
  * Constructor method
  * Destructive method
* Module management
  * Built-in module that takes almost no RAM
  * Extension module support: script module, bytecode file module and shared library (like `*.so`, `*.dll`) module
* Garbage collecttor
  * Mark-Sweep GC
* Exception handling
  * Throw any exception value using the `raise` statement
  * Multiple catch mode
* Bytecode file support
  * Export function to bytecode file
  * Load the bytecode file and execute

## License

Berry is free software distributed under the [MIT license](./LICENSE).

The Berry interpreter partly referred to [Lua](http://www.lua.org/)'s design.
View Lua's license here: [http://www.lua.org/license.html](http://www.lua.org/license.html).
