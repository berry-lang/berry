# Berry

## Introduction

Berry is a small dynamically typed embedded scripting language. Berry's design goals include running on very small memory devices, and so, Berry must be very streamlined. Its core language only supports a few types, but we still provide class and closure support.

The implementation of Berry include a one pass compiler and register-based VM, all the codes are written in ANSI C. Berry is not everything is object. Some simple value types, such as int, real, boolean and string are not class object, but list, map and range are class object. This is a consideration about performance. Register-based VM is also based on this consideration.

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

## Build

1. install "libreadline" (Only linux)

``` bash
sudo apt install libreadline-dev # Ubuntu
```

2. Build:

```
$ make
```

3. Run:

```
$ ./berry test.be

```

## Editor pulgins

[Visual Studio Code](https://code.visualstudio.com/) pulgin are in this directory: [./tools/pulgins/vscode](./tools/pulgins/vscode).

# Examples

Berry does not support REPL (Read Eval Print Loop) yet. You can only run the code in the source file. If no command line parameters are specified, Berry will run "./test.be" by default.

Type:

``` lua
print("Hello world!")
```

Output:

```
Hello world!
```

Type:

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
