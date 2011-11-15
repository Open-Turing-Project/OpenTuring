% The "ProcedureExample1" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
procedure gui
    % Writes the text for button 1.
    locatexy (110, 325)
    put "ON"
    % Writes the text for button 2.
    locatexy (110, 200)
    put "OFF"
    % Writes the text for button 3.
    locatexy (110, 75)
    put "EXIT"
    % Draws the three boxes.
    drawbox (50, 275, 175, 375, red)
    drawbox (50, 150, 175, 250, red)
    drawbox (50, 25, 175, 125, red)
    % Draws the circle representing the LED.
    drawoval (450, 250, 25, 25, black)
end gui

% The mainline program.
var x, y, button : int
gui     % Calls the procedure gui.
loop
    mousewhere (x, y, button)
    if button = 1 and 50 <= x and x <= 175 and 275 <= y and y <= 375 then
	% If button 1 is clicked, turns the LED on.
	drawfill (450, 250, red, black)
    elsif button = 1 and 50 <= x and x <= 175 and 150 <= y and y <= 250 then
	% If button 2 is clicked, turns the LED off.
	drawfill (450, 250, white, black)
    end if
    % If button 3 is clicked, exits the program.
    exit when button = 1 and 50 <= x and x <= 175 and 25 <= y and y <= 125
end loop
