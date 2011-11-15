% The "Bounce" program
% Simulates the action of a bouncing puck
var row, column : int
put "Enter starting row from 2-25 " ..
get row
put "Enter starting column from 2-80 " ..
get column
var rowChange, columnChange := 1
cls
loop
    % The next six lines are a Selection construct
    % See the Selection chapter if you do not
    % understand them
    if row = 25 or row = 1 then
	rowChange := - rowChange
    end if
    if column = 80 or column = 1 then
	columnChange := - columnChange
    end if
    locate (row, column)
    put "*" ..
    row := row + rowChange
    column := column + columnChange
end loop
