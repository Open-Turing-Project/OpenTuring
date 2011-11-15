% The "Ratmobile" program.

% D0 (pin 2) and D1 (pin 3) control the left side motor.
% D0 (pin 2) moves the left side motor forward.
% D1 (pin 3) moves the left side motor backward.
% D2 (pin 4) and D3 (pin 5) control the right side motor.
% D2 (pin 4) moves the right side motor forward
% D3 (pin 5) moves the right side motor backward.

% These are the locations of the boxes. (x1, y1) is the lower-left corner
% of the box and (x2, y2) is the upper-right corner.
const leftBoxX1 := 20
const leftBoxY1 := 270
const leftBoxX2 := 90
const leftBoxY2 := 310
const rightBoxX1 := 20
const rightBoxY1 := 220
const rightBoxX2 := 90
const rightBoxY2 := 260
const aheadBoxX1 := 20
const aheadBoxY1 := 170
const aheadBoxX2 := 90
const aheadBoxY2 := 210
const backBoxX1 := 20
const backBoxY1 := 120
const backBoxX2 := 90
const backBoxY2 := 160
const stopBoxX1 := 20
const stopBoxY1 := 70
const stopBoxX2 := 90
const stopBoxY2 := 110
const exitBoxX1 := 20
const exitBoxY1 := 20
const exitBoxX2 := 90
const exitBoxY2 := 60
setscreen ("graphics")

% x,y is the position of the mouse.
% button is the variable determining if the mouse was clicked.
var x, y, button : int

% Draws the titles.
colour (red)
locatexy (230, 380)
put "Ratmobile Control Screen"
locatexy (0, 320)
colour (black)
put "Control Panel"
locatexy (200, 200)
colour (red)
put "Click the Control Panel to Activate Ratmobile"

colour (black)

% Draws "Left" button.
drawbox (leftBoxX1, leftBoxY1, leftBoxX2, leftBoxY2, red)
locatexy (leftBoxX1 + 20, leftBoxY1 + 20)
put "Left" ..

% Draws the "Right" button.
drawbox (rightBoxX1, rightBoxY1, rightBoxX2, rightBoxY2, red)
locatexy (rightBoxX1 + 20, rightBoxY1 + 20)
put "Right" ..

% Draws the "Ahead" button.
drawbox (aheadBoxX1, aheadBoxY1, aheadBoxX2, aheadBoxY2, red)
locatexy (aheadBoxX1 + 20, aheadBoxY1 + 20)
put "Ahead" ..

% Draws the  "Back" button.
drawbox (backBoxX1, backBoxY1, backBoxX2, backBoxY2, red)
locatexy (backBoxX1 + 20, backBoxY1 + 20)
put "Back" ..

% Draws  the "Stop" button.
drawbox (stopBoxX1, stopBoxY1, stopBoxX2, stopBoxY2, red)
locatexy (stopBoxX1 + 20, stopBoxY1 + 20)
put "Stop" ..

% Draws the "Exit" button.
drawbox (exitBoxX1, exitBoxY1, exitBoxX2, exitBoxY2, red)
locatexy (exitBoxX1 + 20, exitBoxY1 + 20)
put "Exit" ..

loop
    % Locates the current mouse position.
    mousewhere (x, y, button)

    colour (green)

    if leftBoxX1 <= x and x <= leftBoxX2 and
	    leftBoxY1 <= y and y <= leftBoxY2 and button = 1 then
	% If the "Left" box is clicked, sets D1 and D2 high signaling
	% the left motor to go back and the right motor to go ahead.
	locatexy (200, 200)
	put "The Ratmobile is Turning Left"
	parallelput (2 + 4)
    elsif rightBoxX1 <= x and x <= rightBoxX2 and
	    rightBoxY1 <= y and y <= rightBoxY2 and button = 1 then
	% If the "Right" box is clicked, sets D0 and D3 high signaling
	% the left motor to go forward and the right motor to go back.
	locatexy (200, 200)
	put "The Ratmobile is Turning Right"
	parallelput (1 + 8)
    elsif aheadBoxX1 <= x and x <= aheadBoxX2 and
	    aheadBoxY1 <= y and y <= aheadBoxY2 and button = 1 then
	% If the "Ahead" box is clicked, sets D1 and D3 high signaling
	% both left and right motor to go ahead.
	locatexy (200, 200)
	put "The Ratmobile is Going Forward"
	parallelput (1 + 4)
    elsif backBoxX1 <= x and x <= backBoxX2 and
	    backBoxY1 <= y and y <= backBoxY2 and button = 1 then
	% If the "Back" box is clicked, sets D2 and D4 high signaling
	% both the left and right motors to go in reverse.
	locatexy (200, 200)
	put "The Ratmobile is Backing Up"
	parallelput (2 + 8)
    elsif stopBoxX1 <= x and x <= stopBoxX2 and
	    stopBoxY1 <= y and y <= stopBoxY2 and button = 1 then
	% If the "Stop" box is clicked, sets all pins low signaling both
	% motors to stop.
	locatexy (200, 200)
	put "The Ratmobile has Stopped"
	parallelput (0)
    elsif exitBoxX1 <= x and x <= exitBoxX2 and
	    exitBoxY1 <= y and y <= exitBoxY2 and button = 1 then
	% If the "Quit" box is clicked, sets all pins low signaling both motors
	% to stop, then exits out of the loop and terminates the program.
	locatexy (200, 200)
	put "Robin has Left the Ratmobile"
	parallelput (0)
	exit
    end if
end loop
