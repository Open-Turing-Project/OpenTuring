% The "ProcedureExample2" program.
var x, remainder, quotient, button, xmouse, ymouse : int

% Defines all the constants.
% The text for each of the boxes.
const counterText := "1            Binary Counter"
const exitText := "2            EXIT"
const continueText := "           To continue click here... "

% Gives the x and y coordinates for each of the two titles.
const exitTextX := 60
const exitTextY := 60
const counterTextX := 60
const counterTextY := 110

% Gives the locations of each of the boxes.
const exitX1 := 40
const exitY1 := 35
const exitX2 := 80
const exitY2 := 75

const counterX1 := 40
const counterY1 := 85
const counterX2 := 80
const counterY2 := 125

const continueX1 := 480
const continueY1 := 80
const continueX2 := 510
const continueY2 := 110

% This creates the title and boxes.
procedure title
    mousewhere (xmouse, ymouse, button)
    % Sets the screen to graphics mode.
    setscreen ("graphics")
    put " " : 25, "BINARY COUNTER"

    % Draws the boxes, and places the text in the boxes.
    locatexy (exitTextX, exitTextY)
    put exitText ..
    drawbox (exitX1, exitY1, exitX2, exitY2, black)

    locatexy (counterTextX, counterTextY)
    put counterText ..
    drawbox (counterX1, counterY1, counterX2, counterY2, black)
end title

% Draws the box and the circumference of the circles for the eight lights.
procedure drawlights
    % 89 is the center of the left-most LED on the screen.
    x := 89
    drawbox (23, 210, 617, 270, black)
    % Loop to display eight LEDs.
    for counter : 1 .. 8
	drawoval (x, 240, 15, 15, black)
	% Center of circles sixty-six pixels apart.
	x := x + 66
    end for
end drawlights

% Counts in binary on the screen.
% The base 10 number called counter must be converted to binary
% each time the outer counted loop is executed.
procedure counting
    put " "
    put " "
    put "                         Counting from 0 .. 255 in binary"

    % Starts the counting from 0 to 255.
    for counter : 0 .. 255
	% Center of right-most LED.
	x := 551
	% Starts converting base 10 numbers to binary.
	% Saves base 10 number, counter as quotient.
	quotient := counter
	% The lights represent the number of each LED.
	% The LEDs are lit from right to left.
	% Converts the variable quotient to an eight-bit binary number.
	for light : 1 .. 8
	    remainder := quotient mod 2
	    % Draws the LED in the appropriate colour.
	    % While we could use drawfill, it is very slow.  A drawfilloval
	    % followed by the drawoval for the outline colour is much faster.
	    if remainder = 1 then
		drawfilloval (x, 240, 15, 15, red)
	    else
		drawfilloval (x, 240, 15, 15, white)
	    end if
	    drawoval (x, 240, 15, 15, black)
	    % Changes number being divided.
	    quotient := quotient div 2
	    % Moves one LED to the left.
	    x := x - 66
	end for
	delay (250)
    end for
    locate (20, 20)
    put continueText
    % Draws clickable box in bottom-right corner.
    drawbox (continueX1, continueY1, continueX2, continueY2, black)
    loop
	% Gets the mouse cursor position.
	mousewhere (xmouse, ymouse, button)
	% Exits when the button is pressed.
	exit when button = 1 and continueX1 <= xmouse and
	    xmouse <= continueX2 and continueY1 <= ymouse and
	    ymouse <= continueY2
    end loop
    cls
end counting

% This is the mainline.
title
loop
    % Gets the cursor position.
    mousewhere (xmouse, ymouse, button)
    % Checks if first box is clicked.
    if button = 1 and counterX1 <= xmouse and xmouse <= counterX2 and
	    counterY1 <= ymouse and ymouse <= counterY2 then
	% Clears the screen before continuing.
	cls
	drawlights
	counting
	title
    end if
    % Checks if the second box was clicked.
    exit when button = 1 and exitX1 <= xmouse and xmouse <= exitX2 and
	exitY1 <= ymouse and ymouse <= exitY2
end loop
