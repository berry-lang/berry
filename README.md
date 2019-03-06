# Berry

## Introduction

Berry is a ultra-lightweight dynamically typed embedded scripting language. Berry's design goals include running on very small memory devices, and so, Berry must be very streamlined. Its core language only supports a few types, but we still provide class and closure support.

The implementation of Berry include a one pass compiler and register-based VM, all the code is written in ANSI C. In Berry not every type is a class object. Some simple value types, such as int, real, boolean and string are not class object, but list, map and range are class object. This is a consideration about performance. Register-based VM is also based on this consideration.

## Documents

[https://gztss.github.io/berry](https://gztss.github.io/berry)

## Features

* Base Type
  * Numerical value: Integer (`int`) and Real (`real`)
  * Boolean: `true` and `false`
  * String: Single quotation-mark string and double quotation-mark string
  * List: Continuous memory list, like `[1, 2, 3]`
  * Map: Hash Map, like `{ 'a': 1, 2: 3, 'map': {} }`
  * Range: include a lower and a upper integer value, like `0..5`
* Operator and Expression
  * Assign operator: `=`
  * Relational operator: `<`, `<=`, `==`, `!=`, `>`, `>=`
  * Logic operator: `&&`, `||`, `!`
  * Arithmetic operator: `+`, `-`, `*`, `/`, `%`
  * Field operator: `.`
  * Index operator: `[]`
  * Connect string operator: `+`
  * Brackets: `()`
* Control Structure
  * Conditional statement: `if-else`
  * Iteration statement: `while` and `for`
  * Jump statement: `break`
* Function
  * Local variable and block scope
  * Return statement
  * Nested functions definition
  * Closure based on Upvalue
* Class
  * Inheritance (only public single inheritance)
  * Method and Operator Overload
  * Constructor method
* Module Management
  * Statically allocated memory: no RAM used before import.
  * Module nesting.

## Build

1. install the readline library (Linux or MacOS)

``` bash
sudo apt install libreadline-dev # Ubuntu
brew install readline            # MacOS
```

2. Build:

```
make
```

3. Run:

``` bash
./berry # Bash
berry   # Windows CMD
```

## Editor pulgins

[Visual Studio Code](https://code.visualstudio.com/) pulgin are in this directory: [./tools/pulgins/vscode](./tools/pulgins/vscode).

# Examples

After compiling successfully, use the `berry` command with no parameters to enter the REPL environment:
```
Berry 0.0.1 (build in Dec 24 2018, 18:12:49)
[GCC 8.2.0] on Linux (default)
>
```

Now enter this code:

``` lua
print("Hello world!")
```

You will see this output:

```
Hello world!
```

You can copy this code to the REPL:

```ruby
def list_iter(list)
    index = 0
    def it()
        value = list[index]
        index = index + 1
        return value
    end
    return it
end
l = [0, 1, 2, 3, 4, 5]
lout = []
it = list_iter(l)
v = it()
while (v != nil)
    lout.append(v)
    v = it()
end
print(lout)
```

This examples is a simple list iterator. Let's look at the output:

```
[0, 1, 2, 3, 4, 5]
```

You can save the above code to a file (eg test.be) and run:

``` bash
./berry test.be
```

This will also get the correct output.
