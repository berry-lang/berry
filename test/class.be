class Base
    var b
    def init(a)
        self.b = a * 3
    end
end
class A : Base
    var c
    def init(c)
        self.c = c
        super(self).init(c)
    end
    def func(a, b)
        return 1 + a + b
    end
    def -*(other)
    end
end

obj = A(2)
print(obj.func(5, 6))

assert(obj.func(5, 6) == 12)
