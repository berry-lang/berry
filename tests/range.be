# test ranges
var l

# simple ranges
l = []
for i:1..10 l..i end
assert(l == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

l = []
for i:range(1, 10) l..i end
assert(l == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

# range in higher steps
l = []
for i:range(1, 10, 2) l..i end
assert(l == [1, 3, 5, 7, 9])

# negative ranges
l = []
for i:range(1, 10, -2) l..i end
assert(l == [])

l = []
for i:range(30, 0, -3) l..i end
assert(l == [30, 27, 24, 21, 18, 15, 12, 9, 6, 3, 0])
