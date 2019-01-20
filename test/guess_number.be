import io

res = random() % 100
max_test = 7
test = -1
idx = 1
print('Guess a number between 0 and 99. You have', max_test, 'chances.')
while (test != res && idx <= max_test)
    print(idx, ': enter the number you guessed.')
    test = number(io.input())
    if (type(test) != 'int')
        print('This is not an integer. Continue!')
        continue
    elif (test > res)
        print('This number is too large.')
    elif (test < res)
        print('This number is too small.')
    end
    idx = idx + 1
end
if (test == res)
    print('You win!')
else
    print('You failed, the correct answer is', res)
end
