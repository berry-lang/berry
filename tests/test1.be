class Base
    var base
    def init(a)
        print(a)
        self.base = a * a
    end
end

class ClassA : Base
    var test
    def init(a)
        super(self).init(a)
        self.test = a
    end
    def method(a, b)
        return self.test * a + b
    end
    def print()
        print("test:", self.test)
        print("base:", self.base)
        print("method:", self.method)
    end
end

ins = ClassA(100)
# print and class test
print(ins.test, ins.method(5, 2))
ins.print()
