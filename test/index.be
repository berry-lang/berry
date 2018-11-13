class mylist : list
    def init()
        super(self).init()
    end
    def iter()
        i = 0
        def it()
            if (i < self.size())
                v = i
                i = i + 1
                return self[v]
            end
            return nil
        end
        return it
    end
    def item(idx) # int or range index
        if (type(idx) == "int")
            return super(self)[idx]
        elif (type(idx) == "range")
            i = idx.lower()
            upper = idx.upper()
            l = mylist()
            while (i <= upper)
                l.append(self[i])
                i = i + 1
            end
            return l
        end
        return nil
    end
end

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
    def print()
        print('range(', self.__lower, ',', self.__upper, ')')
    end
end

l = mylist()
l.append(1)
l.append(2)
l.append(3)
l.resize(6, 8)
it = l.iter()
print(it(), it(), it())
print(l[range(0, 5)])
