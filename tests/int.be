#- toint() converts any instance to int -#
class Test_int
    def toint()
        return 42
    end
end
t=Test_int()
assert(int(t) == 42)

#- int can parse hex strings -#
assert(int("0x00") == 0)
assert(int("0X1") == 1)
assert(int("0x000000F") == 15)
assert(int("0x1000") == 0x1000)

#- test new int(val, min, max) feature -#
def assert_error(f, error_type)
    try
        f()
        assert(false, 'unexpected execution flow')
    except .. as e, m
        assert(e == error_type)
    end
end

assert(int(0) == 0)
assert(int(0,0,0) == 0)
assert(int(4, 2, 10) == 4)
assert(int(4, 2) == 4)

assert(int(4, 5, 10) == 5)
assert(int(4, 0, 2) == 2)
assert(int(-30, -50, -40) == -40)

# nil parameter is ignored
assert(int(4, nil, 10) == 4)
assert(int(4, nil, 2) == 2)
assert(int(4, 2, nil) == 4)
assert(int(4, 5, nil) == 5)
assert(int(4, nil, nil) == 4)

# test invalid parameters
assert_error(def () return int(4, 5, 3) end, 'range_error')
assert_error(def () return int(4, 5.5, 8) end, 'type_error')
assert_error(def () return int(4, 5, "8") end, 'type_error')
