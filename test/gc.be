def func(a, b)
    c = "0hes"
    def retf()
        return a + b
    end
    return retf
end
f = func(100, 20)
print(f())
f = nil

testAppend = false
l = []
i = 0
c = clock()
if (testAppend)
    while (i < 100000000) # need 64bit
        l.append("hello world")
        l.append(i)
        i = i + 1
    end
else
    l.resize(4000, 100)
end
print("list resize/append", l.size(),
    "values, time of use:", clock() - c, "s")

def mcnt(a)
    m = a - memcount()
    if (m >= 1024 * 1024)
        return [m / (1024 * 1024), "MB"]
    elif (m >= 1024)
        return [m / 1024, "kB"]
    end
    return [m, "bytes" ]
end
l = nil
c = clock()
mc = memcount()
l = []
mfree = mcnt(mc)
print("gc freed", mfree[0], mfree[1],
    "time of use:", clock() - c, "s")
print("now memory count:", -mcnt(0)[0], mcnt(0)[1]);
