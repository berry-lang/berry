codedump(def ()
    try
        raise 'error'
    except (e)
        print(e)
    end
end)
