import time

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

testPush = false
l = []
i = 0
c = time.clock()
if (testPush)
    while (i < 100000000) # need 64bit
        l.push("hello world")
        l.push(i)
        i = i + 1
    end
else
    l.resize(4000, 100)
end
print("list resize/push", l.size(),
    "values, time of use:", time.clock() - c, "s")

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
c = time.clock()
mc = memcount()
l = []
mfree = mcnt(mc)
print("gc freed", mfree[0], mfree[1],
    "time of use:", time.clock() - c, "s")
print("now memory count:", -mcnt(0)[0], mcnt(0)[1]);
