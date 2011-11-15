% The "LoopExample8" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
% The button and mouse variables.
var x1, y1, x2, y2, xmouse, ymouse, button : int

% Gets all of the button data.
put "Enter the button data for..."
put "x1: " ..
get x1
put "y1: " ..
get y1
put "x2: " ..
get x2
put "y2: " ..
get y2
drawbox (x1, y1, x2, y2, black) % Draws the button.
loop
    mousewhere (xmouse, ymouse, button)   % Gets the mouse position.
    % Exits when the button is pressed.
    exit when button = 1 and x1 <= xmouse and xmouse <= x2 and
	y1 <= ymouse and ymouse <= y2
end loop
put "You have pressed the button."
