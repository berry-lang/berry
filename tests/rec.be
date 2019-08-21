def func(x)
    print(x)
    if (x < 50)
        func(x + 1)
    end
end

func(0)
