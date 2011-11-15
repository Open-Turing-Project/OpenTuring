% The "MouseTrails" program.
var x, y, oldx, oldy, b : int := - 1
var c : int := 0
loop
    % Get the current location of the mouse
    mousewhere (x, y, b)
    % if the mouse has moved and the button is pressed.
    if (x not= oldx or y not= oldy) and b = 1 then
	% Draw a circle around the mouse location
	drawfilloval (x, y, 20, 20, c)
	% Change the color
	c := (c + 1) mod 16
	oldx := x
	oldy := y
    end if
end loop
