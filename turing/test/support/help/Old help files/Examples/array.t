% The "array" program.
var marks : array 1 .. 10 of int
for i : 1 .. 10                % Add up the elements of marks
    marks (i) := Rand.Int (1, 10)
end for

var sum : int := 0
for i : 1 .. 10                % Add up the elements of marks
    put marks (i)
    sum := sum + marks (i)
end for
put "Sum = ", sum
