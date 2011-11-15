% The "LightSwitch" program.

% This is the centre and radius of the light.
const lightX := 320
const lightY := 200
const size := 20

% These are the locations of the boxes. (x1, y1) is the lower-left corner
% of the box and (x2, y2) is the upper-right corner.
const exitBoxX1 := 20
const exitBoxY1 := 20
const exitBoxX2 := 70
const exitBoxY2 := 60
const offBoxX1 := 20
const offBoxY1 := 70
const offBoxX2 := 70
const offBoxY2 := 110
const onBoxX1 := 20
const onBoxY1 := 120
const onBoxX2 := 70
const onBoxY2 := 150

setscreen ("graphics")

% x,y is the position of the mouse.
% click is the variable determining if the mouse was clicked.
var x, y, button : int

% Title
locatexy (230, 380)
put ("Computer Light Controller")

% Draws the circle to represent the light.
drawfilloval (lightX, lightY, size, size, gray)

% Draws Exit box.
drawbox (exitBoxX1, exitBoxY1, exitBoxX2, exitBoxY2, red)
locatexy (32, 40)
put "Exit" ..

% Draws Off box.
drawbox (offBoxX1, offBoxY1, offBoxX2, offBoxY2, red)
locatexy (37, 90)
put "Off" ..

% Draws On box.
drawbox (onBoxX1, onBoxY1, onBoxX2, onBoxY2, red)
locatexy (40, 130)
put "On" ..

% Continuously loops until the exit box is clicked.
loop
    % Locates the (x,y) mouse position.
    mousewhere (x, y, button)
    if onBoxX1 <= x and x <= onBoxX2 and
	    onBoxY1 <= y and y <= onBoxY2 and button = 1 then
	% If the "On" box is clicked, the real-world light turns on and the
	% virtual light changes to red.
	% D0 (pin 1) controls the relay.
	drawfilloval (lightX, lightY, size, size, red)
	parallelput (1)
    elsif offBoxX1 <= x and x <= offBoxX2 and
	    offBoxY1 <= y and y <= offBoxY2 and button = 1 then
	% If the "Off" box is clicked, the real-world light turns off and the
	% virtual light changes to gray.
	drawfilloval (lightX, lightY, size, size, gray)
	parallelput (0)
    elsif exitBoxX1 <= x and x <= exitBoxX2 and
	    exitBoxY1 <= y and y <= exitBoxY2 and button = 1 then
	% If the "Exit" box is clicked the real-world light turns off, the
	% virtual light changes to gray, and the program exits.
	drawfilloval (lightX, lightY, size, size, gray)
	parallelput (0)
	exit
    end if
end loop
