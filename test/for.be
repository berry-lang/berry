print("for test:")
for (i : 0 .. 5)
    print(i)
end

print("equivalent while test:")
do
    it = __iterator__(0..5)
    while (__hasnext__(it))
        i = __next__(it)
        print(i)
    end
end
