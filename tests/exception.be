def fraise()
    raise 'error'
end

def test_func()
    try
        fraise()
        return
    except (e)
        print('exception test')
    end
end

codedump(test_func)
test_func()
