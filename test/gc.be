def func(a)
    def retf()
        return a
    end
    return retf
end
f = func(100)

print(f())
print(f())
l = list()
l.append("hello")
print(l)

l = nil
