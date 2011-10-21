% The "DrawDiagonalLine" program
% Draw a diagonal line in window
% starting in row 1, column 1
var row, column := 1
cls
loop
    % Stop when diagonal touches bottom of window
    exit when row > 25
    locate (row, column)
    put "*" ..
    % Move down diagonal
    row := row + 1
    column := column + 1
end loop
