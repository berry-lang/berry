m = { 'a':1, 'b':3.5, 'c': "foo", 0:1}

assert(type(m) == 'instance')
assert(classname(m) == 'map')

# accessor
assert(m['a'] == 1)
assert(m['b'] == 3.5)
assert(m['c'] == 'foo')
assert(m[0] == 1)

# find
assert(m.find('a') == 1)
assert(m.find('z') == nil)
assert(m.find('z', 4) == 4)

# has
assert(m.has('a'))
assert(m.has(0))
assert(!m.has('z'))
assert(!m.has())

# set
m['y'] = -1
assert(m['y'] == -1)
