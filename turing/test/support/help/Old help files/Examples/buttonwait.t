% The "buttonwait" program.
var x, y, buttonnumber, buttonupdown, buttons : int
var nx, ny : int
loop
    buttonwait ("down", x, y, buttonnumber, buttonupdown)
    nx := x
    ny := y
    loop
	drawline (x, y, nx, ny, 0)          % Erase previous line
	exit when buttonmoved ("up")
	mousewhere (nx, ny, buttons)
	drawline (x, y, nx, ny, brightred)  % Draw line to position
    end loop
    buttonwait ("up", nx, ny, buttonnumber, buttonupdown)
    drawline (x, y, nx, ny, brightgreen)    % Draw line to final position
end loop
