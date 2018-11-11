def func(a, b)
    def retf()
        return a + b
    end
    return retf
end
f = func(100, 20)
print(f())

l = []
i = 0
c = clock()
while (i < 100000000) # need 64bit
    l.append("hello world")
    l.append(i)
    i = i + 1
end
print("loop time usage:", clock() - c, "s")
print(l[100])

f = nil
l = nil

c = clock()
l = []
print("gc time usage:", clock() - c, "s")
