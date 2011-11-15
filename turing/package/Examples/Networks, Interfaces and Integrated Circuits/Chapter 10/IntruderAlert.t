% The "IntruderAlert" program.

% D0 (pin 2) sends power to the emitter.
% D1 (pin 3) activates the alarm if the IR beam is broken.
% I0 (pin 10) connects to the IR receiver.  I0 is 1 if the emitter is given
% power and the IR receiver is receiving the beam.

% These are the locations of the boxes. (x1, y1) is the lower-left corner
% of the box and (x2, y2) is the upper-right corner.
const armBoxX1 := 20
const armBoxY1 := 270
const armBoxX2 := 80
const armBoxY2 := 310
const disarmBoxX1 := 20
const disarmBoxY1 := 210
const disarmBoxX2 := 80
const disarmBoxY2 := 250
const quitBoxX1 := 20
const quitBoxY1 := 150
const quitBoxX2 := 80
const quitBoxY2 := 190

% For this computer, 63 means that light is shining on the receiver.
% The receiver is receiving light from the emitter.
% For this computer, 127 means that light is not shining on the receiver.
% The beam is broken.

setscreen ("graphics")

% x,y is the position of the mouse.
% button is the variable determining if the mouse button is pressed.
var x, y, button : int
var value : int
% armed indicates whether or not the alarm is currently armed.
var armed : boolean := false

% Draws the titles.
colour (red)
locatexy (230, 380)
put "Home Security System"
locatexy (0, 320)
colour (black)
put "Control Panel"

% Draws the "Quit" box.
drawbox (quitBoxX1, quitBoxY1, quitBoxX2, quitBoxY2, black)
locatexy (quitBoxX1 + 20, quitBoxY1 + 20)
colour (black)
put "Quit" ..

% Draws the "Disarm" box.
drawbox (disarmBoxX1, disarmBoxY1, disarmBoxX2, disarmBoxY2, green)
locatexy (disarmBoxX1 + 10, disarmBoxY1 + 20)
colour (green)
put "Disarm" ..

% Draws the "Arm" box.
drawbox (armBoxX1, armBoxY1, armBoxX2, armBoxY2, red)
locatexy (armBoxX1 + 20, armBoxY1 + 20)
colour (red)
put "Arm" ..

loop
    % Locates the current mouse position.
    mousewhere (x, y, button)
    if quitBoxX1 <= x and x <= quitBoxX2 and
	    quitBoxY1 <= y and y <= quitBoxY2 and button = 1 then
	% If the Quit box is clicked, sets all pins on parallel port to 0.
	% The system is now shut down.
	locatexy (200, 200)
	colour (black)
	put "The Security System is Shut Down"
	parallelput (0)
	locatexy (200, 180)
	put ""
	exit
    elsif disarmBoxX1 <= x and x <= disarmBoxX2 and
	    disarmBoxY1 <= y and y <= disarmBoxY2 and button = 1 then
	% If the Disarm box is clicked, sets all pins on parallel port to 0
	% to shut off infrared emitter.
	% The program continues to wait for another command.
	locatexy (200, 200)
	colour (green)
	put "The Security System is Disarmed"
	parallelput (0)
	armed := false
	locatexy (200, 180)
	put ""
    elsif armBoxX1 <= x and x <= armBoxX2 and
	    armBoxY1 <= y and y <= armBoxY2 and button = 1 then
	% If the Arm box is clicked, sets D0 pin to 1 to the
	% activating the emitter.
	locatexy (200, 200)
	colour (red)
	put "The Security System is Armed"
	parallelput (1)
	armed := true
    end if

    % If the system is armed, tests the result of the IR receiver by
    % checking the input pin I0 on the parallel port.
    if armed then
	value := parallelget

	if (value div 64) mod 2 = 1 then
	    % If I0 is 1, the IR receiver is getting light.
	    locatexy (200, 180)
	    put " Situation normal. No intruders detected."
	else
	    % If I0 is 0, the beam has been interrupted.
	    locatexy (200, 180)
	    put "INTRUDER ALERT"
	    % Sets D0 and D1.  D0 continues to provide power and D1
	    % starts the buzzer.
	    parallelput (3)
	end if
    end if
end loop
