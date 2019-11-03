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
lout.resize(10)
print(lout, lout.size())
print(memcount(), type(1.25), type([]), type(list), super([]))
