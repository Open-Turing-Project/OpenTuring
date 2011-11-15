% The "StopLight" program.

% redPin, yellowPin, and greenPin are the values that are output on
% the parallel port to activate the lights.
const redPin := 1
const yellowPin := 2
const greenPin := 4

% The following constants are the centres of the lights.
const redX := 300
const redY := 275
const yellowX := 300
const yellowY := 225
const greenX := 300
const greenY := 175

% This is the size of the lights.
const size := 20

% These are the locations of the boxes. (x1, y1) is the lower-left corner
% of the box and (x2, y2) is the upper-right corner.
const slowerBoxX1 := 50
const slowerBoxY1 := 0
const slowerBoxX2 := 100
const slowerBoxY2 := 50
const fasterBoxX1 := 125
const fasterBoxY1 := 0
const fasterBoxX2 := 175
const fasterBoxY2 := 50

% When the increase or decrease delay button is clicked, the speed
% of the lights increases or decreases.
procedure ChangeDelay (var delayTime : int)
    var x, y, button : int
    mousewhere (x, y, button)
    if slowerBoxX1 <= x and x <= slowerBoxX2 and
	    slowerBoxY1 <= y and y <= slowerBoxY2 and button = 1 then
	% The mouse was clicked inside the ">" box.  Increase the delay time.
	delayTime += 100
	locatexy (200, 25)
	put "Delay = ", delayTime
    elsif fasterBoxX1 <= x and x <= fasterBoxX2 and
	    fasterBoxY1 <= y and y <= fasterBoxY2 and button = 1 then
	% The mouse was clicked inside the "<" box.  Decrease the delay time.
	delayTime -= 100
	locatexy (200, 25)
	put "Delay = ", delayTime
    end if
end ChangeDelay

% Main program

% delayTime sets the time for which each light should be lit in milliseconds.
var delayTime : int := 1500

% Sets the screen into graphics mode.
setscreen ("graphics")
% Draws the window and controls.
colour (brightblue)
locate (3, 28)
put "Traffic Control System"

% Draws the > box.
drawbox (slowerBoxX1, slowerBoxY1, slowerBoxX2, slowerBoxY2, darkgrey)
locatexy (slowerBoxX1 + 25, slowerBoxY1 + 25)
put ">" ..

% Draws the < box.
drawbox (fasterBoxX1, fasterBoxY1, fasterBoxX2, fasterBoxY2, darkgrey)
locatexy (fasterBoxX1 + 25, fasterBoxY1 + 25)
put "<" ..

% Draws the empty stop lights.
drawoval (redX, redY, size, size, brightred)
drawoval (yellowX, yellowY, size, size, yellow)
drawoval (greenX, greenY, size, size, brightgreen)

locatexy (200, 25)
put "Delay = ", delayTime
locatexy (200, 15)
put "Press any key to exit." ..

% Cycles through each of the colours of the light.  Exit the loop (and the
% program) when a key is pressed.
loop
    exit when hasch

    % Turns off the red light and activates the green light.
    parallelput (greenPin)
    drawfilloval (redX, redY, size, size, white)
    drawoval (redX, redY, size, size, brightred)
    drawfilloval (greenX, greenY, size, size, brightgreen)
    ChangeDelay (delayTime)
    delay (delayTime)

    % Turns off the green light and activates the yellow light.
    parallelput (yellowPin)
    drawfilloval (greenX, greenY, size, size, white)
    drawoval (greenX, greenY, size, size, brightgreen)
    drawfilloval (yellowX, yellowY, size, size, yellow)
    ChangeDelay (delayTime)
    delay (delayTime)

    % Turns off the yellow light and activates the red light.
    parallelput (redPin)
    drawfilloval (yellowX, yellowY, size, size, white)
    drawoval (yellowX, yellowY, size, size, yellow)
    drawfilloval (redX, redY, size, size, brightred)
    ChangeDelay (delayTime)
    delay (delayTime)
end loop
