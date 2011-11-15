% The "LoopExample7" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
% Declares the button position coordinates as constants.
const BUTTON_X1 := 10
const BUTTON_Y1 := 10
const BUTTON_X2 := 20
const BUTTON_Y2 := 20
% The mouse variables.
var xmouse, ymouse, button : int
% Draws a box on the screen.
drawbox (BUTTON_X1, BUTTON_Y1, BUTTON_X2, BUTTON_Y2, black)
loop
    % Gets the mouse cursor status.
    mousewhere (xmouse, ymouse, button)
    % Keeps looping until the coordinates of the mouse are within the
    % button's coordinates and the mouse button is pressed.
    exit when button = 1 and BUTTON_X1 <= xmouse and 
	xmouse <= BUTTON_X2 and
	BUTTON_Y1 <= ymouse and ymouse <= BUTTON_Y2
end loop
put "You have pressed the button."
