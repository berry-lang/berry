class Test1
    var a
end

o = Test1()
o.a = 100
assert(o.a == 100)
o.a += 10
assert(o.a == 110)
