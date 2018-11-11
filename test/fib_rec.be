def fib(x)
    if (x <= 2)
        return 1
    end
    return fib(x - 1) + fib(x - 2)
end

c = clock()
print("fib:", fib(38))
print("time:", clock() - c, 's')
