print("for iterator test:")
for (i : 0 .. 5)
    print(i)
end

print("\nequivalent while iterator:")
do
    it = __iterator__(0..5)
    while (__hasnext__(it))
        i = __next__(it)
        print(i)
    end
end

# list iterator
print("\nlist iterator:")
for (i : [0, -10..10, 5, [5, {'t':'f'}]])
    print(i)
end

# map iterator
print("\nmap iterator:")
for (i : {'a': 15, 
          'b': {'list': [1, 2, 3],
                'map': {'str': 'this is a str', 'number': 0x100}
               }
         }
    )
    print(i)
end
