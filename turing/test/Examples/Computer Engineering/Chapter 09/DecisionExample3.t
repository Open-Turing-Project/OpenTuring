% The "DecisionExample3" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
% The button and mouse variables.
var button1X1, button1Y1, button1X2, button1Y2, button2X1, button2Y1, button2X2, button2Y2, xmouse, ymouse, button : int
% Gets all of the button data.
put "Enter the button data for button #1 ..."
put "x1: " ..
get button1X1
put "y1: " ..
get button1Y1
put "x2: " ..
get button1X2
put "y2: " ..
get button1Y2
put "Enter the button data for button #2..."
put "x1: " ..
get button2X1
put "y1: " ..
get button2Y1
put "x2: " ..
get button2X2
put "y2: " ..
get button2Y2
% Draws the first button.
drawbox (button1X1, button1Y1, button1X2, button1Y2, 15)
% Draws the second button.
drawbox (button2X1, button2Y1, button2X2, button2Y2, 15)
loop
    locatexy (300, 200)
    % Finds the mouse cursor position.
    mousewhere (xmouse, ymouse, button)
    if button = 1 and button1X1 <= xmouse and xmouse <= button1X2 and
	    button1Y1 <= ymouse and ymouse <= button1Y2 then
	put "You have pressed the first button."
	delay (500)
	locatexy (300, 200)
	put "Press the other button to exit."
    end if
    % Exits when a button is pressed.
    exit when button = 1 and button2X1 <= xmouse and xmouse <= button2X2 and
	button2Y1 <= ymouse and ymouse <= button2Y2
end loop
put "You have pressed the exit button."
