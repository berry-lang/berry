****************
Berry examples
****************

.. code::berry

    def fib(x)
        if x <= 1
            return x
        end
        return fib(x - 1) + fib(x - 2)
    end

    fib(10)
