% The "Joystick" program.

% This program is a game designed for the joystick. An arrow flashes
% until the user moves the joystick in that position. The game
% times how long it took the user to complete ten cycles.

% Constants representing the direction of the target arrow and the current
% joystick location.
const NEUTRAL := 0
const UP := 1
const UP_RIGHT := 2
const RIGHT := 3
const DOWN_RIGHT := 4
const DOWN := 5
const DOWN_LEFT := 6
const LEFT := 7
const UP_LEFT := 8

% The target variable represents the currently lit up target arrow.
var target : int := NEUTRAL
% The selected variable represents the current location of the joystick.
var selected : int := NEUTRAL
var firing : boolean := false

procedure CentreText (row : int, s : string)
    locate (row, 40 - length (s) div 2)
    put s ..
end CentreText


procedure DrawText (row, col : int, s : string)
    locate (row, col)
    put s ..
end DrawText


procedure Instructions
    var ch : string (1)

    colour (red)
    CentrerText (1, "Joystick!")
    colour (black)
    DrawText (3, 10, "This game measures reaction times.  The object is to")
    DrawText (4, 10, "score as low a score as possible over ten trials.  To")
    DrawText (5, 10, "start a trial, centre the joystick and release the fire")
    DrawText (6, 10, "button.  Once this is done, there is a delay from")
    DrawText (7, 10, "half a second to four seconds.  Then one of the arrows")
    DrawText (8, 10, "on the screen lights.  Move the joystick to highlight")
    DrawText (9, 10, "the arrow and press the fire button.")
    DrawText (11, 10, "If you press the fire button too soon, a penalty is")
    DrawText (12, 10, "added to the reaction time.")

    CentreText (maxrow, "Press any key to start the game")
    getch (ch)
end Instructions

% This returns the current position of the joystick and
% status of the fire button.  It assumes that I0 is attached to
% the upper pipe, I1 is attached to the lower pipe, I2 is attached
% to the left pipe, I3 is attached to the right pipe and I4 is
% attached to the fire button.  The pins are high unless the joystick
% is touching them.
procedure GetJoystickPosition (var position : int, var firing : boolean)
    var upperPipe, lowerPipe, leftPipe, rightPipe : boolean
    var value : int := parallelget

    if (value div 64) mod 2 = 1 then
	upperPipe := false
    else
	upperPipe := true
    end if
    if (value div 128) mod 2 = 1 then
	lowerPipe := false
    else
	lowerPipe := true
    end if
    if (value div 32) mod 2 = 1 then
	leftPipe := false
    else
	leftPipe := true
    end if
    if (value div 16) mod 2 = 1 then
	rightPipe := false
    else
	rightPipe := true
    end if
    if (value div 8) mod 2 = 1 then
	firing := false
    else
	firing := true
    end if

    if upperPipe then
	if leftPipe then
	    position := UP_LEFT
	elsif rightPipe then
	    position := UP_RIGHT
	else
	    position := UP
	end if
    elsif lowerPipe then
	if leftPipe then
	    position := DOWN_LEFT
	elsif rightPipe then
	    position := DOWN_RIGHT
	else
	    position := DOWN
	end if
    else
	if leftPipe then
	    position := LEFT
	elsif rightPipe then
	    position := RIGHT
	else
	    position := NEUTRAL
	end if
    end if
end GetJoystickPosition


% The following eight procedures draw the arrows.
procedure DrawUpArrow
    var clr : int := black
    if selected = UP then
	clr := red
    end if

    drawline (300, 280, 340, 280, clr)
    drawline (300, 280, 300, 350, clr)
    drawline (340, 280, 340, 350, clr)

    drawline (300, 350, 290, 350, clr)
    drawline (340, 350, 350, 350, clr)

    drawline (290, 350, 320, 390, clr)
    drawline (350, 350, 320, 390, clr)
    if target = UP then
	drawfill (322, 371, green, clr)
    end if
end DrawUpArrow

procedure DrawUpRightArrow
    var clr : int := black
    if selected = UP_RIGHT then
	clr := red
    end if

    drawline (355, 295, 385, 265, clr)
    drawline (355, 295, 405, 345, clr)
    drawline (385, 265, 435, 315, clr)

    drawline (405, 345, 395, 355, clr)
    drawline (435, 315, 445, 305, clr)

    drawline (395, 355, 445, 360, clr)
    drawline (445, 305, 445, 360, clr)
    if target = UP_RIGHT then
	drawfill (440, 320, green, clr)
    end if
end DrawUpRightArrow

procedure DrawRightArrow
    var clr : int := black
    if selected = RIGHT then
	clr := red
    end if

    drawline (370, 250, 370, 210, clr)
    drawline (370, 250, 440, 250, clr)
    drawline (370, 210, 440, 210, clr)

    drawline (440, 250, 440, 260, clr)
    drawline (440, 210, 440, 200, clr)

    drawline (440, 260, 480, 230, clr)
    drawline (440, 200, 480, 230, clr)
    if target = RIGHT then
	drawfill (390, 220, green, clr)
    end if
end DrawRightArrow

procedure DrawDownRightArrow
    var clr : int := black
    if selected = DOWN_RIGHT then
	clr := red
    end if

    drawline (385, 195, 355, 165, clr)
    drawline (385, 195, 435, 145, clr)
    drawline (355, 165, 405, 115, clr)

    drawline (435, 145, 445, 155, clr)
    drawline (405, 115, 395, 105, clr)

    drawline (445, 155, 445, 100, clr)
    drawline (395, 105, 445, 100, clr)
    if target = DOWN_RIGHT then
	drawfill (420, 120, green, clr)
    end if
end DrawDownRightArrow

procedure DrawDownArrow
    var clr : int := black
    if selected = DOWN then
	clr := red
    end if

    drawline (340, 180, 300, 180, clr)
    drawline (340, 180, 340, 110, clr)
    drawline (300, 180, 300, 110, clr)

    drawline (340, 110, 350, 110, clr)
    drawline (300, 110, 290, 110, clr)

    drawline (350, 110, 320, 70, clr)
    drawline (290, 110, 320, 70, clr)
    if target = DOWN then
	drawfill (310, 120, green, clr)
    end if
end DrawDownArrow

procedure DrawLeftDownArrow
    var clr : int := black
    if selected = DOWN_LEFT then
	clr := red
    end if

    drawline (285, 165, 255, 195, clr)
    drawline (285, 165, 235, 115, clr)
    drawline (255, 195, 205, 145, clr)
    drawline (235, 115, 245, 105, clr)
    drawline (205, 145, 195, 155, clr)
    drawline (245, 105, 195, 100, clr)
    drawline (195, 155, 195, 100, clr)
    if target = DOWN_LEFT then
	drawfill (200, 110, green, clr)
    end if
end DrawLeftDownArrow

procedure DrawLeftArrow
    var clr : int := black
    if selected = LEFT then
	clr := red
    end if

    drawline (270, 210, 270, 250, clr)
    drawline (270, 210, 200, 210, clr)
    drawline (270, 250, 200, 250, clr)

    drawline (200, 250, 200, 260, clr)
    drawline (200, 210, 200, 200, clr)

    drawline (200, 200, 160, 230, clr)
    drawline (200, 260, 160, 230, clr)
    if target = LEFT then
	drawfill (211, 211, green, clr)
    end if
end DrawLeftArrow

procedure DrawUpLeftArrow
    var clr : int := black
    if selected = UP_LEFT then
	clr := red
    end if

    drawline (255, 265, 285, 295, clr)
    drawline (255, 265, 205, 315, clr)
    drawline (285, 295, 235, 345, clr)

    drawline (235, 345, 245, 355, clr)
    drawline (205, 315, 195, 305, clr)

    drawline (245, 355, 195, 360, clr)
    drawline (195, 305, 195, 360, clr)
    if target = UP_LEFT then
	drawfill (260, 290, green, clr)
    end if
end DrawUpLeftArrow


% The following two procedures draw the fire button and base.
procedure DrawFireButtonBase
    drawfillbox (70, 310, 150, 390, gray)
end DrawFireButtonBase


procedure DrawFireButton
    var clr : int := black
    if firing then
	clr := red
    end if
    drawfilloval (110, 350, 30, 30, clr)
end DrawFireButton

% The following procedure draws all of the arrows.
procedure DrawArrows
    DrawUpArrow
    DrawUpRightArrow
    DrawRightArrow
    DrawDownRightArrow
    DrawDownArrow
    DrawLeftDownArrow
    DrawLeftArrow
    DrawUpLeftArrow
    DrawFireButton
end DrawArrows


% Draws all the arrows with none lit up, then waits until the user has
% centred the joystick.
procedure WaitForNeutral
    cls
    DrawFireButtonBase
    target := NEUTRAL
    loop
	GetJoystickPosition (selected, firing)
	DrawArrows
	exit when selected = NEUTRAL and not firing
    end loop

    % Outputs a message.
    CentrerText (maxrow - 1, "Prepare for the next arrow...")
end WaitForNeutral

% Delays for 1/2 to 4 seconds.
procedure DelayForRandomTime
    var delayTime : int
    randint (delayTime, 500, 4000)
    delay (delayTime)
end DelayForRandomTime

% Draws all the arrows with the target lit up, then waits until the user has
% placed the joystick in the correct location.
procedure WaitForCorrectJoystickPosition (var timeTaken : int)
    var startTime, endTime : int
    var penalty : boolean := false

    cls
    DrawFireButtonBase
    clock (startTime)

    % Each time through the loop, gets the joystick's current location.  Draws
    % the arrows with the target illuminated and the currently selected
    % joystick location indicated.
    loop
	GetJoystickPosition (selected, firing)
	DrawArrows
	% If the fire button is pressed and the joystick is not in the
	% correct location, the user suffers a penalty.
	if firing and selected not= target then
	    CentreText (maxrow - 2, "PENALTY!  You pressed the fire " +
		"button too soon!")
	    penalty := true
	end if

	% If the fire button is pressed and the joystick is in the correct
	% location, exits the loop and calculates the reaction time.
	exit when firing and selected = target
    end loop
    clock (endTime)

    timeTaken := endTime - startTime

    % Outputs a message.
    var message := "Time taken: " + intstr (timeTaken) + " milliseconds"
    if penalty then
	message := message + " + penalty of " + intstr (timeTaken) +
	    " milliseconds"
	timeTaken := timeTaken * 2
    end if
    CentreText (maxrow, message)
    delay (2000)
end WaitForCorrectJoystickPosition


% Main program
setscreen ("graphics,nocursor")

% The running total of the response times.
var totalTime, timeForTrial : int := 0

Instructions

for i : 1 .. 10
    % Waits until user has centred joystick and is not pressing a button.
    WaitForNeutral
    % Delays a random amount of time.
    DelayForRandomTime
    % Draws an arrow in a random direction and waits until the user has
    % moved the joystick to the correct location and pressed the fire button.
    randint (target, 1, 9)
    WaitForCorrectJoystickPosition (timeForTrial)
    totalTime := totalTime + timeForTrial
end for

cls
CentreText (12, "The total response time was: " + intstr (totalTime) +
    " milliseconds")
