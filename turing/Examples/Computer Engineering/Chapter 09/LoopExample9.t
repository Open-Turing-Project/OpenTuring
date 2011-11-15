% The "LoopExample9" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
var textX, textY, x1, y1, x2, y2, xmouse, ymouse, button : int
var text : string (40)

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
put "Enter text to be placed in button : " ..
get text : *                    % Finds the x location of the button.
textX := x1 + 10                % The y position of the button.
textY := y1 + (y2 - y1) div 2   % Draws the button.
drawbox (x1, y1, x2, y2, black)
% locatexy causes the put statement to place the text near (textX, textY).
locatexy (textX, textY)
% The '..' in the put statement stops Turing from erasing to end of line.
put text ..
loop
    % Gets the mouse cursor position.
    mousewhere (xmouse, ymouse, button)
    % Exits when the button is pressed.
    exit when button = 1 and x1 <= xmouse and xmouse <= x2 and
	y1 <= ymouse and ymouse <= y2
end loop
put ""
put "You have pressed the button."
