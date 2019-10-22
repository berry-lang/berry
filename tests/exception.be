codedump(def ()
    try
        throw 'error'
    catch (e)
        print(e)
    end
end)
