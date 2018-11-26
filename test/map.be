m = {"k1": 1, "k2": 2}
m.insert("test", "value")
j = {"a": m}
m.insert("map", {"a": 6})
print('m =', m)
print('j =', j)
print('m["k1"] =', m["k1"])
print('m[8] =', m[8])
