# berry

## Introduction

Berry is a small dynamically typed scripting language. Berry's design goals include running on very small memory devices, and so, berry must be very streamlined. Its core language only supports a small number of types, but we still provide class and closure support.

The implementation of berry include a one pass compiler and register-based VM. Berry is not everything is object. Some simple value types, such as int, real, boolean and string are not class object, but list, map and range are class object. This is a consideration about performance. Register-based VM is also based on this consideration.

## Features

* Base Type
  * Numerical value：Integer (`int`) and Real (`real`).
  * Boolean: `true` and `false`.
  * String: Single quotation-mark string and double quotation-mark string.
  * List: Continuous memory list, like `[1, 2, 3]`.
  * Map: Hash Map, like `{ 'a' = 1, 2 = 3 }`.
  * Range: include a lower and a upper integer value, like `0..5`.
* Operator and Expression
  * Assign operator: `=`.
  * Relational operator: `<`, `<=`, `==`, `!=`, `>`, `>=`.
  * Logic operator: `&&`, `||`, `!`.
  * Arithmetic operator: `+`, `-`, `*`, `/`, `%`.
  * Field operator: `.`.
  * Index operator: `[]`.
  * Connect string operator: `+`.
  * Brackets: `()`.
* Control Structure
  * Conditional statement: `if-else`
  * Iteration statement: `while` and `for`
  * Jump statement: `break`
* Function
  * Local variable and block scope
  * Return statement`
  * Nested functions definition
  * Closure based on Upvalue
* Class
  * Inheritance (only public single inheritance)
  * Method and Operator Overload
  * Constructor method

## Examples

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

