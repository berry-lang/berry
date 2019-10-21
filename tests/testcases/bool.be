# test cases for boolean expressions

a = true
b = false

assert(!!list == true)
assert(a && b == false)
assert(!(a && b))

def test(a, b)
    while (!(a && b))
        assert(false)
    end
end

test(true, true)
