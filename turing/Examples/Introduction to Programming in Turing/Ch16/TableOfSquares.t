% The "TableOfSquares" program
% Outputs a table of squares

function square (number : real) : real
    result number ** 2
end square

for i : 1 .. 10
    put i : 2, square (i) : 4
end for
