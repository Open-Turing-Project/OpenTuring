%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                          MISSILE.T                            %
%                                                               %
%     Developed by Tom West, Stephen Perelgut and Ric Holt      %
%                 for Holt Software Associates                  %
%                                                               %
%   This is a simple video game featuring an ever increasing    %
% number of missiles tha must be avoided.  The seeking          %
% algorithm of the missiles is rather simplistic and is open    %
% to modification and refinement by anyone.  Suggested changes  %
% are having the missiles trail disappear, different launching  %
% times for each missile and having missiles last different     %
% amounts of time.                                              %
%                                                               %
%   All of the constants defined at the top of the program can  %
% be changed in order to experiment with different environments %
% and factors.  Feel free to tinker, doctor and change this     %
% program to your hearts content.  That's what it's here for!   %
%                                                               %
%                                                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                       Constants                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
const groundColour := 1
const missileColour1 := purple
const missileColour2 := brightblue
const explosionColour := 2
const playerColour := brightred
const playerSpeed := 4
const turnDelay := 100
const outOfPlay := -1
const maxMissiles := 100

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                  Global Variables                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
var numMissiles : int := 1 % the number of missiles in play
var evadedMissiles : int := 0 % total number of missiles evaded
var tx : int := maxx div 2 % the x position of the player in pixels
var ty : int := maxy div 2 % the y position of the player in pixels
var turns, timer, dist : int

/**************************************************************
 *   This procedure gives the instructions for the game.      *
 **************************************************************/
procedure Instructions
    setscreen ("noecho,nocursor")
    locate (1, 30)
    put "MISSILE"
    put skip
    color (2)
    put "You are a pilot over Zornastria.  Unfortunately, the missile"
    put "defence system has been activated and is now turned against"
    put "you!  Even worse, the missiles are heat seeking missiles and"
    put "head straight for you.  Luckily if you manuever carefully,"
    put "you can dodge them until they run out of fuel."
    put skip
    put "To pilot your airplane, use the mouse. When the mouse button"
    put "is pressed, your plane moves toward your mouse.  THe missiles start"
    put "out coloured " ..
    colour (purple)
    put "purple" ..
    colour (2)
    put " and turn " ..
    colour (brightblue)
    put "blue" ..
    colour (2)
    put " when they run out of fuel.  You"
    put "must attempt to survive as long as you can.  After surviving each"
    put "wave, the next wave will be launched."
    put skip
    put "Good Luck!  You'll need it."
    put skip
    put "Hit any key to start the game..."

    Input.Pause
    cls
end Instructions

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   This procedure draws the player.                           %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
procedure DrawPlayer (clr : int)
    Draw.FillStar (round (tx - 3), round (ty - 3), round (tx + 3), round (ty + 3), clr)
end DrawPlayer

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   This procedure handles any key strokes by reading any char  %
% from the keyboard and moving the player in response to it.    %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
procedure ControlPlayer
    var x, y, b : int
    Mouse.Where (x, y, b)
    if b = 1 and (x not= tx or y not= ty) then
	DrawPlayer (0)

	var d : real := sqrt ((x - tx) ** 2 + (y - ty) ** 2)
	var dx : int := round ((x - tx) * playerSpeed / d)
	var dy : int := round ((y - ty) * playerSpeed / d)
	if abs (dx) > abs (x - tx) then
	    tx := x
	else
	    tx := tx + dx
	end if
	if abs (dy) > abs (y - ty) then
	    ty := y
	else
	    ty := ty + dy
	end if

	% Make certain the player doesn't go off the screen.
	if tx < 10 then
	    tx := 10
	elsif tx > maxx - 10 then
	    tx := maxx - 10
	end if
	if ty < 10 then
	    ty := 10
	elsif ty > maxy - 10 then
	    ty := maxy - 10
	end if

	% Draw the player in its new position
	DrawPlayer (playerColour)
    end if
end ControlPlayer


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                         Main Body                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
var px, py : array 1 .. maxMissiles of int
% The position array for the missiles
var vx, vy : array 1 .. maxMissiles of int
% The velocity array for the missiles

randomize
setscreen ("graphics,noecho")

Instructions

loop
    % Initialize the missile array.
    for i : 1 .. numMissiles
	px (i) := Rand.Int (0, maxx)
	py (i) := 0
	vx (i) := 0
	vy (i) := Rand.Int (1, 5)
    end for

    % Draw the screen
    cls
    drawline (0, 0, maxx, 0, 1)
    DrawPlayer (playerColour)

    % Set the clock and number of elapsed turns
    turns := 0

    % The main loop
    loop
	turns += 1

	% For each missile: determine the new x velocity
	%                   determine the new y velocity
	%                   set the new missile position
	%                   draw a travel line
	%                   check to see if it hit the ground
	%                   check to see if it hit the player
	for i : 1 .. numMissiles
	    const ox : int := px (i)
	    const oy : int := py (i)

	    if ox not= outOfPlay then
		% Determine the x velocity
		dist := abs (vx (i)) * (abs (vx (i)) + 1) div 2
		if vx (i) < 0 and ox - dist < 0 then
		    vx (i) += 2
		elsif vx (i) > 0 and ox + dist > maxx then
		    vx (i) -= 2
		elsif turns > 100 then
		    if turns mod 20 = 0 then
			vx (i) -= sign (vx (i))
		    end if
		elsif ox < tx then
		    vx (i) += 1
		elsif ox > tx then
		    vx (i) -= 1
		end if

		% Determine the y velocity
		dist := abs (vy (i)) * (abs (vy (i)) + 1) div 2
		if vy (i) > 0 and oy + dist > maxy then
		    vy (i) -= 2
		elsif turns > 100 then
		    if turns mod 8 = 0 then
			vy (i) -= 1
		    end if
		elsif vy (i) < 0 and oy - dist < -turns div 15 then
		    vy (i) += 2
		elsif oy < ty then
		    vy (i) += 1
		elsif oy > ty then
		    vy (i) -= 1
		end if

		% Set the new missile position
		px (i) += vx (i)
		py (i) += vy (i)

		% Draw a travel line
		if turns > 100 then
		    Draw.ThickLine (ox, oy, px (i), py (i), 1, missileColour2)
		else
		    Draw.ThickLine (ox, oy, px (i), py (i), 2, missileColour1)
		end if

		% Check to see if it hit the ground
		if py (i) <= 0 then
		    drawline (px (i), 0, px (i) - 4, 4, explosionColour)
		    drawline (px (i), 0, px (i), 6, explosionColour)
		    drawline (px (i), 0, px (i) + 4, 4, explosionColour)
		    px (i) := outOfPlay
		    evadedMissiles += 1
		end if

		% Check to see if it hit the player
		if Math.DistancePointLine (tx, ty, ox, oy, px (i), py (i)) < 3 then
		    drawfillstar (tx - 7, ty - 7, tx + 7, ty + 7, brightblue)
		    locate (10, 5)
		    put "You evaded ", evadedMissiles, " missiles! " ..
		    locate (1, 1)
		    delay (500)
		    return
		end if
	    end if
	end for

	% Check if the player is being controlled
	ControlPlayer

	% This is a timer delay loop to make sure that each turn takes
	% the same length of time to execute, regardless of the number
	% of missiles on the screen
	Time.DelaySinceLast (30)

	% This will leave the loop when all the missiles have crashed.
	var endNow : boolean := true
	for i : 1 .. numMissiles
	    if px (i) not= outOfPlay then
		endNow := false
	    end if
	end for
	exit when endNow
    end loop

    % After each time all the missiles have crashed, add 1 to the number
    % of missiles with a maximum of maxMissiles.
    numMissiles += 1
    if numMissiles > maxMissiles then
	numMissiles := maxMissiles
    end if
end loop
