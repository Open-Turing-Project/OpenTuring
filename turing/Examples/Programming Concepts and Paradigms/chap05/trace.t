% File "trace.t". Determine which integers are prime numbers.

% Check whether n is a prime number, return truth value in isPrime.
procedure checkPrime (n : int, var isPrime : boolean)
    pre n >= 2
    const sqrtr := sqrt(n)
    var factor := 2
    loop
        exit when factor > sqrtr or n mod factor = 0
        factor += 1
    end loop
    isPrime := factor > sqrtr  % No value of factor divided n.
end checkPrime

% Check whether v is a prime number and print v with the result.
procedure checkAndPrint (v : int)
    var r : boolean
    checkPrime (v, r)
    put v, ", ", r
end checkAndPrint

for i : 2 .. 10
    checkAndPrint (i)
end for
