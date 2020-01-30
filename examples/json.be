import json as jsonlib

json = jsonlib.load('{"strs":123, "test": {"hello": [false, null]}}')
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
print(jsonlib.dump(obj))
print("json dump (format):")
print(jsonlib.dump(obj, "format"))
print(jsonlib.dump("str\r\n/45\v你好\x7f"))
