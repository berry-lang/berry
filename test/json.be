json = json_load('{"strs":123, "test": {"hello": [false, null]}}')
obj =
    [
        0,
        1,
        {
            'str': "test hahah balabala"
        },
        {
            'map': { "json": 58 },
            'list': [ nil, true, false ]
        }
    ]

print("json parser:")
print(json)
print("json dumps (compress):")
print(json_dumps(obj))
print("json dumps (format):")
print(json_dumps(obj, "format"))
