% File "powers.t".  Compute a table of powers of integers.
const size := 10
const maxPower := 5
var table : array 1 .. size, 1 .. maxPower of int

% Initialize the table.
for i : 1 .. size
    table (i, 1) := i
    for j : 2 .. maxPower
        table (i, j) := table (i, j - 1) * i
    end for
end for

% Print the table.
for i : 1 .. size
    for j : 1 .. maxPower
        put table (i, j) : 7 ..
    end for
    put ""
end for
