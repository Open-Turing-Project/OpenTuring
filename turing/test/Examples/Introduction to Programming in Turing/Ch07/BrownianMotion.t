% The "BrownianMotion" program
% Moves an asterisk around on
% the window like a smoke particle
% This is known as Brownian motion
% Color window green
colorback (green)
for row : 1 .. 25
    put repeat (" ", 80) ..
end for

% Set character color to magenta
color (magenta)
setscreen ("nocursor") % Hide cursor
loop
    % Start at center
    var row := 13
    var column := 40
    loop
	% Start over when asterisk goes off the window
	exit when column < 1 or column > 80
	    or row < 1 or row > 25
	locate (row, column)
	put "*" ..
	delay (100)
	% Erase asterisk in old location by
	% outputting a blank there
	locate (row, column)
	put " " ..
	% Compute new value for row
	randint (row, row - 1, row + 1)
	% Compute new value for column
	randint (column, column - 1, column + 1)
    end loop
    play (">C<")     % Sound a note
    % This is explained in the Music chapter
end loop
