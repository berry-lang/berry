def test_func()
    try
        raise 'error'
        print(1+nil)
    except (e)
        print('exception test')
    end
end

#codedump(test_func)
test_func()
