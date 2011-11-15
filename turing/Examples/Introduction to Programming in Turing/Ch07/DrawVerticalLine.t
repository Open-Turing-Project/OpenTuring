% The "DrawVerticalLine" program
% Draws a vertical line of asterisks
var column : int
put "Choose a column for line ",
    "between 1 and 80 inclusive"
get column
cls
for row : 1 .. 25
    locate (row, column)
    put "*" ..
end for
