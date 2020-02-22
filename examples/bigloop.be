import time

c = time.clock()
do
    i = 0
    while (i < 100000000)
        i = i + 1
    end
end
print('while iteration 10000000 times', time.clock() - c, 's')

c = time.clock()
for (i : 1 .. 10000000)
end
print('for   iteration 1000000  times', time.clock() - c, 's')
# while iteration 10000000 times 0.942453 s
# for   iteration 1000000  times 0.929293 s
