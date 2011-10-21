% The "LeafFall" program
% Color parts of the window randomly
cls
% Set to color background green
colorback (green)
var row, column : int
loop
    randint (row, 1, 25)
    randint (column, 1, 80)
    locate (row, column)
    put " " ..
end loop
