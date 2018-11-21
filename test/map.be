m = {"k1": 1, "k2": 2}
m.insert("test", "value")
j = {"a": m}
m.insert("map", {"a": 6})
print(m, j, m["k1"], m[8])
