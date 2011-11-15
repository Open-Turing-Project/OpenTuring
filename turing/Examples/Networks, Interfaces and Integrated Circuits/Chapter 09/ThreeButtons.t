% The "ThreeButtons" Program
procedure drawButtonsAndLED
    % Draws the three boxes.
    drawbox (50, 275, 175, 375, red)
    drawbox (50, 150, 175, 250, green)
    drawbox (50, 25, 175, 125, blue)
    % Writes the text for button 1.
    locatexy (110, 325)
    put "ON" ..
    % Writes the text for button 2.
    locatexy (110, 200)
    put "OFF" ..
    % Writes the text for button 3.
    locatexy (110, 75)
    put "EXIT" ..
    % Draws the circle representing the LED.
    drawoval (450, 250, 25, 25, black)
end drawButtonsAndLED

% Main program
var x, y, button : int
drawButtonsAndLED             % Calls the procedure 'drawButtonsAndLED'.

loop
    mousewhere (x, y, button)
    % If button 1 is clicked, turns the LED and the graphical representation
    % of the LED on.
    if 50 <= x and x <= 175 and 275 <= y and y <= 375 and button = 1 then
	drawfill (450, 250, red, black)
	parallelput (1)
	% If button 2 is clicked, turns the LED and the graphical
	% representation of the LED off.
    elsif 50 <= x and x <= 175 and 150 <= y and y <= 250 and button = 1 then
	drawfill (450, 250, white, black)
	parallelput (0)
    end if
    % If button 3 is clicked, exits the program.
    exit when 50 <= x and x <= 175 and 25 <= y and y <= 125 and button = 1
end loop
