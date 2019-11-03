import os

# os.path.join test
assert(os.path.join('') == '')
assert(os.path.join('abc', 'de') == 'abc/de')
assert(os.path.join('abc', '/de') == '/de')
assert(os.path.join('a', 'de') == 'a/de')
assert(os.path.join('abc/', 'de') == 'abc/de')
assert(os.path.join('abc', 'de', '') == 'abc/de/')
assert(os.path.join('abc', '', '', 'de') == 'abc/de')
assert(os.path.join('abc', '/de', 'fghij') == '/de/fghij')
assert(os.path.join('abc', 'xyz', '/de', 'fghij') == '/de/fghij')
