% The "DrawVerticalLines" program
% Draw a series of vertical lines, one at a time
cls
var column : int
% Draw line of minus signs between the two subwindows
locate (21, 1)
for count : 1 .. 80
    put "-" ..
end for
% Could instead use put repeat ("-", 80)
loop
    % Move cursor into prompt subwindow
    locate (23, 1)
    put " "      % Clear the line
    locate (23, 1)
    put "Choose a column for the line between 1 and 80, end with -1: " ..
    get column
    exit when column = - 1
    for row : 1 .. 20
	locate (row, column)
	put "*" ..
    end for
end loop
