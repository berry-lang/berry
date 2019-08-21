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
it = list_iter(l)
v = it()
while (v != nil)
    print(v)
    v = it()
end

def range_iter(lower, upper)
    i = lower
    def it()
        if (i <= upper)
            v = i
            i = i + 1
            return v
        end
        return nil
    end
    return it
end

l = []
it = range_iter(0, 10)
v = it()
while (v != nil)
    l.append(v)
    v = it()
end
print(l)

class range
    var __lower, __upper
    def init(lower, upper)
        self.__lower = lower
        self.__upper = upper
    end
    def lower()
        return self.__lower
    end
    def upper()
        return self.__upper
    end
    def iter()
        i = self.lower()
        def it()
            if (i <= self.upper())
                v = i
                i = i + 1
                return v
            end
            return nil
        end
        return it
    end
    def tostring()
        return 'range(' + str(self.__lower) + ', ' + str(self.__upper) + ')'
    end
end

it = range(-5, 5).iter()
print(range(-5, 5))
l = []
v = it()
while (v != nil)
    l.append(v)
    v = it()
end
print(l)
