# this is berry language test
class Base
	var b
	def init(a)
		self.b = a * 1.5
	end
end
class A : Base
    var test
    def init(a)
        self.test = a
		super(self).init(a)
    end
    def method(a, b)
        return self.test * a + b * self.b
    end
    def print()
        print("test:", self.test)
        print("super.b:", super(self).b)
        print("method:", self.method)
    end
	def +(other)
		return self.test + other.test
	end
	def >(other)
		return self.test > other.test
	end
	def tobool()
		return self.test != 0
	end
	def item(index)
		return index * index - 1000
	end
	def -*()
		return -self.test
	end
end

#-
 @ multi-line comment
-#
obj = A(100)
# print and class test
print(obj.test, obj.method(5, 2))
print('print object:', obj)
print("reload operator test: A(100) + A(20) =", A(100) + A(20))
print("reload operator test: A(100) > A(20) =", A(100) > A(20))
print("reload operator test: A(100) && A(0) =", A(100) && A(0))
print("reload operator test: A(100) || A(0) =", A(100) || A(0))
print("reload operator test: -A(100) =", -A(100))
print("obj[30] =", obj[20])

l2 = list()
print("list test...");
l2.append(24)
l2.append("2test")
l2.append("2end")
l2.append("2end")
l2.setitem(3, nil)

l = [450 * 30, "hello", 666, l2, [4, 5, 6]]

idx = -5
while (idx <= 5)
	l.append(idx * idx * idx)
	idx = idx + 1
end

print(.5)

print("test list print 1:", l)
# 这是一个通过下标读取的测试，实际上是item()方法的语法糖
print("list[3] =", l[3])
# 这是一个通过下标写入的测试，实际上是setitem()方法的语法糖
l[2] = '皮一下很开心'
print("test list print 2:", l)
print([8, 9][0]);
print("\033[0;36mprint test\033[0m")

l2 = nil
l = nil
