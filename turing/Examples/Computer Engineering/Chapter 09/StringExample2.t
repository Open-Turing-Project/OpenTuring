% The "StringExample2" program.
var x, remainder, quotient, button, xmouse, ymouse : int

% Defines all the constants.
% The titles for the boxes.
const binaryText := "1            Binary Counter"
const asciiText := "2            ASCII Converter"
const exitText := "3            EXIT"
const continueText := "           To continue click here... "

% Gives the x and y coordinates for the text.
const binaryTextX := 60
const binaryTextY := 150
const asciiTextX := 60
const asciiTextY := 105
const exitTextX := 60
const exitTextY := 60

% Binary counter button coordinates.
const binaryX1 := 40
const binaryY1 := 135
const binaryX2 := 80
const binaryY2 := 175

% ASCII converter button coordinates.
const asciiX1 := 40
const asciiY1 := 85
const asciiX2 := 80
const asciiY2 := 125

% Exit button coordinates.
const exitX1 := 40
const exitY1 := 35
const exitX2 := 80
const exitY2 := 75

% Continue button coordinates.
const continueX1 := 480
const continueY1 := 80
const continueX2 := 510
const continueY2 := 110

% This creates the title and boxes.
% Draws the boxes, places them accordingly, and places the phrases.
procedure title
    % Sets the screen to graphics mode.
    setscreen ("graphics")
    put " " : 25, "WRITING CODE FOR THE REAL WORLD"

    % Binary counter box and text is displayed.
    locatexy (binaryTextX, binaryTextY)
    put binaryText
    drawbox (binaryX1, binaryY1, binaryX2, binaryY2, black)

    % ASCII converter box and text is displayed.
    locatexy (asciiTextX, asciiTextY)
    put asciiText
    drawbox (asciiX1, asciiY1, asciiX2, asciiY2, black)

    % Exit box and text is displayed.
    locatexy (exitTextX, exitTextY)
    put exitText
    drawbox (exitX1, exitY1, exitX2, exitY2, black)
end title

% Draws the box and the circumference of the circles for the eight lights.
procedure drawlights
    % 89 is the centre of the left-most LED on the screen.
    x := 89
    drawbox (23, 210, 617, 270, black)
    % Loop to display eight LEDs.
    for counter : 1 .. 8
	drawoval (x, 240, 15, 15, black)
	% Centre of the circles are sixty-six pixels apart.
	x := x + 66
    end for
end drawlights

% Draws a box that must be clicked on to continue.
procedure continue
    locate (20, 20)
    put continueText
    drawbox (continueX1, continueY1, continueX2, continueY2, black)
    loop
	% Gets the mouse cursor position.
	mousewhere (xmouse, ymouse, button)
	exit when button = 1 and continueX1 <= xmouse and
	    xmouse <= continueX2 and continueY1 <= ymouse and
	    ymouse <= continueY2
    end loop
    cls
end continue

% Counts in binary on the LEDs as well as the screen.
procedure counting
    put " "
    put " "
    put "                         Counting from 0 .. 255 in binary"

    % Calculates the remainder (converting from base 10 to binary).
    for counter : 0 .. 255
	% 551 is the x value of the center for the last circle.
	x := 551
	quotient := counter
	for light : 1 .. 8
	    % Calculates the first remainder.
	    remainder := quotient mod 2
	    % If the remainder of the first division is 1, then
	    % fills in the right-most circle.
	    % Draws the LED in the appropriate colour.
	    if remainder = 1 then
		drawfilloval (x, 240, 15, 15, red)
	    else
		drawfilloval (x, 240, 15, 15, white)
	    end if
	    drawoval (x, 240, 15, 15, black)
	    quotient := quotient div 2
	    x := x - 66
	end for
	delay (500)
    end for
    continue
end counting

% This spells in binary.
procedure drawascii
    var answer, name : string
    var size, number1, xcircle : int
    put " "
    put " "
    put "      Enter the word or name you would like spelled out in binary."
    get name : *
    size := length (name)
    for counter : 1 .. size
	% Converts the letters into numbers.
	locate (15, 40)
	put name (counter) ..
	number1 := ord (name (counter))
	xcircle := 551
	for light : 1 .. 8
	    remainder := number1 mod 2
	    if remainder = 1 then
		drawfilloval (xcircle, 240, 15, 15, red)
	    else
		drawfilloval (xcircle, 240, 15, 15, white)
	    end if
	    drawoval (xcircle, 240, 15, 15, black)
	    number1 := number1 div 2
	    xcircle := xcircle - 66
	end for
	% Delays the amount of time the lights will come on.
	delay (500)
    end for
    % Text will be placed approximately at the graphics position.
    continue
end drawascii

% This is the mainline.
title
loop
    % Gets the cursor position.
    mousewhere (xmouse, ymouse, button)
    if button = 1 and binaryX1 <= xmouse and xmouse <= binaryX2 and
	    binaryY1 <= ymouse and ymouse <= binaryY2 then
	% Clears the screen before continuing.
	cls
	drawlights
	counting
	title
    elsif button = 1 and asciiX1 <= xmouse and xmouse <= asciiX2 and
	    asciiY1 <= ymouse and ymouse <= asciiY2 then
	cls
	drawlights
	drawascii
	title
    end if
    exit when button = 1 and exitX1 <= xmouse and xmouse <= exitX2 and
	exitY1 <= ymouse and ymouse <= exitY2
end loop
