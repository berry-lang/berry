def fraise()
    raise 'error'
end

def test_func()
    try
        fraise()
        return
    except 'error', 'exception' as e, d:
        print('exception test')
    except 'error':
        print('exception test')
    except '2':
        print('exception test')
    end
end

codedump(test_func)
codedump(fraise)
