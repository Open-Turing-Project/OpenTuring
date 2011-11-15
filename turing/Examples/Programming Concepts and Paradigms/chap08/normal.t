% File "normal.t". Normalize each element in list
% by the largest value in the list.
const size := 10
var list : array 1 .. size of int

% Generate random values for the list.
for i : 1 .. size
    list (i) := Rand.Int (1, 1000)
end for

% Find the largest value.
var largest := list (1)
for i : 2 .. size
    largest := max (largest, list (i))
end for

% Print the values with their normalized values.
for i : 1 .. size
    put i : 3, list (i) : 10, list (i) / largest : 5 : 2
end for
