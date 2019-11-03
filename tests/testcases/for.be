var global

global = 0
for (i : 0 .. 10)
    global += i
end
assert(global == 55)

global = 0
for (i : 0 .. 20)
    if (i > 10)
        break
    end
    global += i
end
assert(global == 55)

global = 0
for (i : 0 .. 20)
    if (i > 10)
        continue
    end
    global += i
end
assert(global == 55)

assert(def ()
        for (i : 0 .. 20)
            if (i > 10)
                return i
            end
        end
    end() == 11)
