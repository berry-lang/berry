def fraise()
    raise 'error', 'this is a error message'
end

def test_func1()
    try
        fraise()
        return
    except 'exception' as e, v:
        print('exception 1')
    except 'error' as e, v:
        print('exception 2, msg: ' + str(v))
    except '2':
        print('exception 3')
    end
end

codedump(test_func1)
codedump(fraise)
test_func1()

def test_func2()
    try
        compile('def +() end')()
    except .. as e, v:
        print('catch execption:', str(e) + ' >>>\n    ' + str(v))
    end
end

test_func2()
