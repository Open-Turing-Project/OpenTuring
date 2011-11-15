% The "LoopExample6" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
% Declares the mouse parameters: xmouse for the x position of the mouse,
% ymouse for the y position of the mouse, and button for the status
% of the button. (1=PRESSED/0=NOT PRESSED)
var xmouse, ymouse, button : int

loop
    % Gets the mouse cursor status and returns with the x, y, and button
    % conditions for the mouse cursor on the screen.
    mousewhere (xmouse, ymouse, button)
    % Displays the mouse position.
    put xmouse, " ", ymouse
    % Exits when the button is pressed.
    exit when button = 1
end loop
