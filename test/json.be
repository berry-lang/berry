json = json_load('{"strs":123, "test": {"hello": [false, null]}}')
obj =
    [
        0,
        1,
        {
            'str': "test hahah balabala",
            'json': json
        },
        {
            'map': { "json": 58 },
            'list': [ nil, true, false ]
        }
    ]

print("json parser:")
print(json)
print("json dump (compress):")
print(json_dump(obj))
print("json dump (format):")
print(json_dump(obj, "format"))
