var start := Time.Elapsed
fcn test (hi : int) : int
    if (hi mod 2) = 0 then
	result hi div 2
    else
	result hi * 3 + 1
    end if
end test


fcn collatz (n : int) : int
    var prev := n
    loop
	prev := test (prev)
	exit when prev < 2
    end loop
    result prev
end collatz

var sum := 0
for i : 1..100
    sum += collatz(90)
end for
put "its:",sums
put "time:",Time.Elapsed - start
delay(2000)

