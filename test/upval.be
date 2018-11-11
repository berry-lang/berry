def fbase(u1 , u2)
    do
        def retf1()
            u1 = u1 + 1;
            return u1 * u2
        end
        def retf2()
            u2 = u2 * 2
            return u2
        end
        return [retf1, retf2]
    end
end

l = fbase(2.5, 3)

i = 0
while (i < 4)
    i = i + 1
    print(i, "call retf1():", l[0](), "\tcall retf2():", l[1]())
end
