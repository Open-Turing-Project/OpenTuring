% File "count.t".  Show beneficial side effects.
var count := 0

function squareRoot (x : int) r : real
    count += 1
    result sqrt (x)
end squareRoot

put "Square roots of first few integers:"
for i : 0 .. 5
    put squareRoot (i) : 8: 2, " " ..
end for
put ""
put "Number of square roots computed: ", count
