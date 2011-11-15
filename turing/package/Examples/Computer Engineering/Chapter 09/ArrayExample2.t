% The "ArrayExample2" program.
var x, remainder, quotient, button, xmouse, ymouse : int
var number : array 1 .. 8 of int

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

% Creates the title and boxes.
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

% Draws the box and outlines the circles for the lights.
procedure drawlights
    x := 89
    drawbox (23, 210, 617, 270, black)
    for counter : 1 .. 8
	drawoval (x, 240, 15, 15, black)
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

% Counts in binary on the screen.
procedure counting
    put " "
    put " "
    put "                         Counting from 0 .. 255 in binary."

    % Calculates the remainder (converting from base 10 to binary).
    for counter : 0 .. 255
	% 551 is the x value of the center for the last circle.
	x := 551
	quotient := counter
	locate (7, 1)
	put counter, "=" ..
	for light : 1 .. 8
	    % Calculates the first remainder.
	    remainder := quotient mod 2
	    % Saves the binary number under array called number.
	    number (light) := remainder
	    % If the remainder of the first division is 1, then
	    % fills in the right-most circle.

	    if remainder = 1 then
		% If the remainder is 1, then fills the oval with red.
		drawfilloval (x, 240, 15, 15, red)
	    else
		% If the remainder is 0, then fills the oval with white.
		drawfilloval (x, 240, 15, 15, white)
	    end if
	    drawoval (x, 240, 15, 15, black)
	    quotient := quotient div 2
	    x := x - 66
	end for

	locate (7, 5)
	% Places the binary numbers above the circles.
	% The digits are displayed left to right.
	for decreasing c : 8 .. 1
	    put number (c) : 8 ..
	end for
	delay (500)
    end for
    continue
end counting

% Spells in binary.
procedure drawascii
    var xcircle, remainder, quotient, size, number1 : int
    var answer, name : string
    put " "
    put " "
    put "      Enter the word or name you would like spelled out in binary."
    get name : *
    size := length (name)
    for counter : 1 .. size
	% Converts the letters into numbers.
	number1 := ord (name (counter))
	xcircle := 551
	quotient := counter
	% Places the numbers 1/0 above the screen.
	for light : 1 .. 8
	    remainder := number1 mod 2
	    number (light) := remainder

	    if remainder = 1 then
		drawfilloval (xcircle, 240, 15, 15, red)
	    else
		drawfilloval (xcircle, 240, 15, 15, white)
	    end if
	    drawoval (xcircle, 240, 15, 15, black)
	    number1 := number1 div 2
	    xcircle := xcircle - 66
	end for
	locate (7, 2)
	put name (counter), " ="
	locate (7, 5)
	% Displays binary number of each letter.
	for decreasing c : 8 .. 1
	    put number (c) : 8 ..
	end for
	put " "
	% Delays the amount of time the lights on the screen will come on.
	delay (1000)
    end for
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
