% The "AlfTheHawk" Program

% ALF THE HAWK
% The program controls two infrared receivers and one stepper
% motor. The IR receivers react, in this case, to light from a.
% flashlight As light is shone in one receiver the motor turns
% clockwise. When light is shone in the other receiver the
% motor turns counter-clockwise. If no light is present the
% motor continually scans through 180 degrees.
% If light is shone into both receivers the motor stops.
%
% The receivers are set up as "eyes" in a paper-mache hawk
% resembling the TV character Alf. The hawk scans through 180
% degrees looking for light. When light is sensed the hawk
% follows the light. When the exit button on the screen is
% clicked with the mouse the program is terminated and the hawk
% returns to the home position.
%
% The following chart indicates the parallelget values assigned
% by a specific computer for input pins 10 and 11.
%
% pin number      open/closed    parallelget
%   10 & 11         both open         127
%     10            10 closed          63
%     11            11 closed         255
%   10 & 11       both closed         191
%
% The motor has four steps controlled by pins 2, 3, 4, and 5
% using parallelput(2 ** counter) where counter loops through
% the whole numbers 0, 1, 2, and 3.

setscreen ("graphics")

% Outputs the value.
procedure OutputValue (value : int)
    locate (maxrow div 2 + 5, maxcol div 2 - 8)
    colour (cyan)
    put "Value = " ..
    colour (brightblue)
    put value
end OutputValue

% Hides the mouse.
procedure Hide
    mousehide
    locate (maxrow div 2 + 8, maxcol div 2 - 8)
    colour (white)
    put repeat (" ", 20)
end Hide

% Outputs the degrees.
procedure OutputDegree (degrees : int)
    locate (maxrow div 2 + 6, maxcol div 2 - 10)
    colour (cyan)
    put "Degrees = " ..
    colour (brightblue)
    put degrees, " "
end OutputDegree

% Outputs the direction.
procedure OutputDirection (direction : string)
    locate (maxrow div 2 + 7, maxcol div 2 - 12)
    colour (cyan)
    put "Direction = " ..
    colour (brightblue)
    put direction
    if direction = "Clockwise" then
	drawfill (190, 310, blue, cyan)
	drawfill (400, 310, white, cyan)
	drawfilloval (300, 310, 30, 30, green)
    elsif direction = "Counter Clockwise" then
	drawfill (400, 310, blue, cyan)
	drawfill (200, 310, white, cyan)
	drawfilloval (300, 310, 30, 30, green)
    else
	drawfill (400, 310, white, cyan)
	drawfill (200, 310, white, cyan)
	drawfilloval (300, 310, 30, 30, red)
    end if
end OutputDirection

% Draws the arrows.
procedure DrawGraphicsArrows
    % Draws clockwise arrow.
    drawarc (200, 200, 100, 100, 90, 180, cyan)
    drawarc (200, 200, 120, 120, 90, 180, cyan)
    drawline (80, 201, 100, 201, cyan)
    drawline (200, 320, 200, 340, cyan)
    drawline (200, 300, 200, 280, cyan)
    drawline (230, 310, 200, 340, cyan)
    drawline (230, 310, 200, 280, cyan)
    % Draws counter-clockwise arrow.
    drawarc (400, 200, 100, 100, 0, 90, cyan)
    drawarc (400, 200, 120, 120, 0, 90, cyan)
    drawline (500, 200, 520, 200, cyan)
    drawline (400, 320, 400, 340, cyan)
    drawline (400, 300, 400, 280, cyan)
    drawline (370, 310, 400, 340, cyan)
    drawline (370, 310, 400, 280, cyan)
end DrawGraphicsArrows

% Draws the title.
procedure DrawGraphicsTitle
    % Advanced programmers may want to use Turing's Font module.
    % Draws the box and the shadow.
    drawfillbox (145, 345, 475, 395, blue)
    drawbox (145, 345, 475, 395, yellow)
    drawfillbox (150, 340, 480, 390, brightblue)
    drawbox (150, 340, 480, 390, yellow)
    % Draws the "T".
    drawline (155, 385, 185, 385, yellow)
    drawline (170, 385, 170, 345, yellow)
    % Draws the "H".
    drawline (200, 385, 200, 345, yellow)
    drawline (230, 385, 230, 345, yellow)
    drawline (200, 365, 230, 365, yellow)
    % Draws the "E".
    drawline (245, 385, 245, 345, yellow)
    drawline (245, 385, 275, 385, yellow)
    drawline (245, 345, 275, 345, yellow)
    drawline (245, 365, 275, 365, yellow)
    % Draws the "H".
    drawline (310, 385, 310, 345, yellow)
    drawline (310, 365, 340, 365, yellow)
    drawline (340, 385, 340, 345, yellow)
    % Draws the "A".
    drawline (355, 365, 355, 345, yellow)
    drawline (385, 365, 385, 345, yellow)
    drawline (355, 365, 385, 365, yellow)
    drawline (355, 365, 370, 385, yellow)
    drawline (385, 365, 370, 385, yellow)
    % Draws the "W".
    drawline (400, 385, 400, 345, yellow)
    drawline (400, 345, 415, 365, yellow)
    drawline (415, 365, 430, 345, yellow)
    drawline (430, 345, 430, 385, yellow)
    % Draws the "K".
    drawline (445, 345, 445, 385, yellow)
    drawline (445, 365, 475, 385, yellow)
    drawline (445, 365, 475, 345, yellow)
end DrawGraphicsTitle

% Draws the exit sign.
procedure DrawGraphicsExitSign
    % Draws the box and the shadow.
    drawfillbox (635, 5, 570, 40, blue)
    drawbox (635, 5, 570, 40, yellow)
    drawfillbox (639, 0, 575, 35, brightblue)
    drawbox (639, 0, 575, 35, yellow)
    % Draws the "E".
    drawline (580, 5, 580, 30, yellow)
    drawline (580, 5, 590, 5, yellow)
    drawline (580, 30, 590, 30, yellow)
    drawline (580, 18, 590, 18, yellow)
    % Draws the "X".
    drawline (595, 30, 605, 5, yellow)
    drawline (605, 30, 595, 5, yellow)
    % Draws the "I".
    drawline (612, 30, 612, 5, yellow)
    % Draws the "T".
    drawline (625, 30, 625, 5, yellow)
    drawline (618, 30, 632, 30, yellow)
end DrawGraphicsExitSign

% Returns to starting position.
procedure ReturnToHomePosition (var degrees, value : int)
    OutputDirection ("Counter Clockwise")
    value := parallelget
    OutputValue (value)
    if degrees > 0 then
	loop
	    for decreasing count : 3 .. 0
		parallelput (2 ** count)
		delay (50)
		degrees -= 2
		OutputDegree (degrees)
	    end for
	    exit when degrees <= 0
	end loop
    end if
end ReturnToHomePosition

% Detects to see if the 3D button was pushed.  If it was, this
% procedure returns Alf to the home position and sets flag = 1.
procedure CheckForQuit (var degrees, value, flag : int)
    var xmouse, ymouse, button : int
    mousewhere (xmouse, ymouse, button)
    if 575 <= xmouse and xmouse <= 639 and
	    0 <= ymouse and ymouse <= 35 and button = 1 then
	Hide
	drawfillbox (570, 40, maxx, 0, white)
	flag := 1
	color (darkgray)
	locate (maxrow - 1, 1)
	put "Please wait while the hawk returns home."
	ReturnToHomePosition (degrees, value)
    end if
end CheckForQuit

% When light is shone in Alf's right eye, he rotates
% clockwise until light is acquired in both eyes.
procedure RotateClockwise (var degrees, value, flag : int)
    value := parallelget
    OutputValue (value)
    OutputDirection ("Clockwise")
    loop
	CheckForQuit (degrees, value, flag)
	for count : 0 .. 3
	    parallelput (2 ** count)
	    delay (70)
	    value := parallelget
	    OutputValue (value)
	    degrees += 2
	    OutputDegree (degrees)
	end for
	exit when value = 127 or value = 191 or value = 255
    end loop
end RotateClockwise

% When light is shone in Alf's left eye, he rotates counter-
% clockwise until light is acquired in both eyes.
procedure RotateCounterClockwise (var degrees, value,
	flag : int)
    value := parallelget
    OutputValue (value)
    OutputDirection ("Counter-Clockwise")
    loop
	CheckForQuit (degrees, value, flag)
	for decreasing count : 3 .. 0
	    parallelput (2 ** count)
	    delay (70)
	    value := parallelget
	    OutputValue (value)
	    degrees -= 2
	    OutputDegree (degrees)
	end for
	exit when value = 127 or value = 191 or value = 63
    end loop
end RotateCounterClockwise

% When light is shone in both eyes, Alf stops.
procedure StopRotation
    parallelput (0)
    OutputDirection ("Stopped")
end StopRotation

% This procedure scans back and forth through the 180 degree
% arc until a light is acquired in Alf's eyes.
procedure ScanForLight (var degrees, value, flag : int)
    var xmouse, ymouse, button : int
    value := parallelget
    OutputValue (value)
    mousewhere (xmouse, ymouse, button)
    loop
	OutputDirection ("Clockwise")
	loop
	    CheckForQuit (degrees, value, flag)
	    % This for loop runs through the sequence of steps to
	    % turn the stepper motor clockwise.
	    for count : 0 .. 3
		parallelput (2 ** count)
		delay (70)
		value := parallelget
		OutputValue (value)
		degrees += 2
		OutputDegree (degrees)
	    end for
	    exit when degrees >= 180 or value not= 127 or flag = 1
	end loop
	exit when value not= 127 or flag = 1
	loop
	    OutputDirection ("Counter Clockwise")
	    % This for loop runs through the sequence of steps
	    % required to turn the stepper motor counter-clockwise.
	    loop
		CheckForQuit (degrees, value, flag)
		for decreasing count : 3 .. 0
		    parallelput (2 ** count)
		    delay (70)
		    value := parallelget
		    OutputValue (value)
		    degrees -= 2
		    OutputDegree (degrees)
		end for
		exit when degrees <= 4 or value not= 127 or flag = 1
	    end loop
	    if button = 1 and xmouse > 100 and xmouse < 300 and
		    ymouse > 100 and ymouse < 300 then
		CheckForQuit (degrees, value, flag)
	    end if
	    exit when degrees <= 0 or value not= 127 or flag = 1
	end loop
	exit when value not= 127 or flag = 1
    end loop
end ScanForLight

% Main program
var degrees, value, flag : int
var xmouse, ymouse, button : int

cls
degrees := 0
OutputDegree (degrees)
flag := 0
color (cyan)
DrawGraphicsArrows
OutputDirection ("Stopped")
DrawGraphicsExitSign
DrawGraphicsTitle
loop
    mousewhere (xmouse, ymouse, button)
    value := parallelget
    OutputValue (value)
    CheckForQuit (degrees, value, flag)

    if value = 127 then
	ScanForLight (degrees, value, flag)
    elsif value = 63 then
	RotateClockwise (degrees, value, flag)
    elsif value = 255 then
	RotateCounterClockwise (degrees, value, flag)
    elsif value = 191 then
	StopRotation
    end if
    if value = 127 and degrees > 0 then
	ReturnToHomePosition (degrees, value)
    end if
    exit when flag = 1
end loop
Hide
degrees := 0
OutputDegree (degrees)
OutputDirection ("Stopped")
parallelput (0)
