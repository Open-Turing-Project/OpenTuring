%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                           PILOT.T                             %
%                                                               %
%     Developed by Tom West, Stephen Perelgut and Ric Holt      %
%                 for Holt Software Associates                  %
%                                                               %
%   This program is a simple two gravitational body simulation. %
% It graphically shows the gravitational pull from the two      %
% "planets" and from the wall.  Most of the gravity calculation %
% is done in hundredth pixels in order to get a fine enough     %
% resolution.  (Originally, all variables starting with 'r'     %
% where real variables, but the program executed far too slowly %
% for use on 8088 PCs.)                                         %
%                                                               %
%   All of the constants defined at the top of the program can  %
% be changed in order to experiment with different environments %
% and factors.  Feel free to tinker, doctor and change this     %
% program to your hearts content.  That's what it's here for!   %
%                                                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
setscreen ("graphics,noecho")
randomize

/***************************************************************
 *                      Colours of Objects                     *
 ***************************************************************/
const wallColour := 1 % the colour of the walls
const obstacleColour := 1 % the colour of planet 1
const targetColour := 2 % the colour of planet 2
const edgeColour := 3
const probeColour := 2

/***************************************************************
 *                      Constants                              *
 ***************************************************************/
% the gravitational push of the walls
const wallGravityConstant := 50
% the gravitational pull of the planets
const planetGravity := 85
% the maximum initial speed is maxSpeed / maxDivider
const maxSpeed := 20
const maxDivider := 10
% the closest that objects will be placed to the edge of the screen
const border := 35

/***************************************************************
 *                      Variables                              *
 ***************************************************************/
var obstacleX, obstacleY : real % the integer position of planet 1
var targetX, targetY : real % the integer position of planet 2
var obstacleSize : int := 10 % The size of the obstacle
var targetSize : int := 30 % The size of the target
var probeX, probeY : real % the probe's position (integer numbers)
var probeVelocityX, probeVelocityY : real % the probe's velocity in
%                                             hundredths of pixels

/**************************************************************
 *   This procedure gives the instructions for the game.      *
 **************************************************************/
procedure Instructions
    setscreen ("noecho,nocursor")
    locate (1, 30)
    put "PILOT"
    put skip
    color (2)
    put "In Xeron IV the universe is a box bounded by negative gravity,"
    put "so that you can never go near the edge."
    put skip
    put "You are a master pilot of the space probe Galaticon, and wish to"
    put "pilot your craft back to the home planet of " ..
    colour (13)
    put "Purplonia" ..
    colour (2)
    put ".  Unfortunately"
    put "the planet of " ..
    colour (11)
    put "Bluezix" ..
    colour (2)
    put " is in the way.  In order to do so, you "
    put "must specifiy the starting velocity of the probe with the arrow"
    put "keys and then use the space bar to launch it.  Each time you are"
    put "successful, you will be put in another situation, each a little"
    put "more difficult.  If you survive 13 times, you are awarded"
    put "Master Pilots status with Nova Clusters."
    put skip
    put "You will need to use the gravity of the two planets to achieve"
    put "this goal.  Good luck."
    put skip
    put "Hit any key to start the game..."

    Input.Pause

    cls
end Instructions

/****************************************************************
 *   This procedure sets up the probe and planets so that the   *
 * obstacle planet is always between the probe and the target   *
 * planet.                                                      *
 ****************************************************************/
procedure InitPosition
    var targetDistance, obstacleDistance : real
    var dist1, dist2, rnd : real
    loop
	targetX := Rand.Int (5 + targetSize, maxx - 5 - targetSize)
	targetY := Rand.Int (5 + targetSize, maxy - 5 - targetSize)
	probeX := Rand.Int (5, maxx - 5)
	probeY := Rand.Int (5, maxy - 5)
	targetDistance := Math.Distance (targetX, targetY, probeX, probeY)
	exit when targetDistance > 30 + targetSize + (2 * obstacleSize)
    end loop
    dist1 := (targetDistance - 30 - targetSize - (2 * obstacleSize)) /
	targetDistance
    rand (rnd)
    dist1 := rnd * dist1
    dist2 := (20 + obstacleSize) / targetDistance
    obstacleX := round (probeX + (dist1 + dist2) * (targetX - probeX))
    obstacleY := round (probeY + (dist1 + dist2) * (targetY - probeY))
    obstacleDistance := Math.Distance (obstacleX, obstacleY, probeX, probeY)
end InitPosition

/***************************************************************
 *   This procedure determines the probe's initial velocity.   *
 ***************************************************************/
procedure InitProbeVelocity
    var endNow : boolean := false
    probeVelocityX := 0
    probeVelocityY := 0
    var c : string (1)
    loop
	c := getchar
	Draw.ThickLine (round (probeX), round (probeY), round (probeX + probeVelocityX),
	    round (probeY + probeVelocityY), 3, white)
	case c of
		% The 7 key (on the keypad)
	    label KEY_HOME :
		probeVelocityX -= 2
		probeVelocityY += 2
		% The 8 key (on the keypad)
	    label KEY_UP_ARROW :
		probeVelocityY += 2
		% The 9 key (on the keypad)
	    label KEY_PGUP :
		probeVelocityX += 2
		probeVelocityY += 2
		% The 4 key (on the keypad)
	    label KEY_LEFT_ARROW :
		probeVelocityX -= 2
		% The 6 key (on the keypad)
	    label KEY_RIGHT_ARROW :
		probeVelocityX += 2
		% The 1 key (on the keypad)
	    label KEY_END :
		probeVelocityX -= 2
		probeVelocityY -= 2
		% The 2 key (on the keypad)
	    label KEY_DOWN_ARROW :
		probeVelocityY -= 2
		% The 3 key (on the keypad)
	    label KEY_PGDN :
		probeVelocityX += 2
		probeVelocityY -= 2
	    label " ", KEY_ENTER :
		endNow := true
	    label :
	end case
	if probeVelocityX < -maxSpeed then
	    probeVelocityX := -maxSpeed
	elsif probeVelocityX > maxSpeed then
	    probeVelocityX := maxSpeed
	end if
	if probeVelocityY < -maxSpeed then
	    probeVelocityY := -maxSpeed
	elsif probeVelocityY > maxSpeed then
	    probeVelocityY := maxSpeed
	end if
	Draw.ThickLine (round (probeX), round (probeY),
	    round (probeX + probeVelocityX), round (probeY + probeVelocityY),
	    3, edgeColour)
	Input.Flush
	exit when endNow
    end loop

    probeVelocityX /= maxDivider
    probeVelocityY /= maxDivider
end InitProbeVelocity


/*********************************************************************
 *   This procedure draws the planets, probe and edge of the screen. *
 *********************************************************************/
procedure DrawPlayingField
    cls
    drawbox (0, 0, maxx, maxy, wallColour)
    drawfilloval (round (probeX), round (probeY), 3, 3, probeColour)
    drawfilloval (round (obstacleX), round (obstacleY), obstacleSize, obstacleSize, brightblue)
    drawfilloval (round (targetX), round (targetY), targetSize, targetSize, brightred)
end DrawPlayingField

/****************************************************************
 *   This procedure draws the probe.                            *
 ****************************************************************/
procedure DrawTarget (px, py, clr : int)
    drawline (px - 1, py - 1, px + 1, py - 1, clr)
    drawline (px - 1, py, px + 1, py, clr)
    drawline (px - 1, py + 1, px + 1, py + 1, clr)
end DrawTarget

/************************************************************
 *                      Main Procedure                      *
 ************************************************************/
var lost : boolean := false % Set if you hit the obstacle or hit a key
var successes : int := 0 % Number of successful interceptions

Instructions

loop
    % Set up the initial positions and velocity
    InitPosition
    DrawPlayingField
    InitProbeVelocity
    DrawPlayingField

    % The movement loop
    loop
	% Calculate the gravitational pull of the walls
	%
	% Gravitation Pull / Push = G / (r**2)
	%
	const wallGravityX := wallGravityConstant / (probeX ** 2) -
	    wallGravityConstant / ((maxx - probeX) ** 2)
	const wallGravityY := wallGravityConstant / (probeY ** 2) -
	    wallGravityConstant / ((maxy - probeY) ** 2)
	const wallGravity := sqrt (wallGravityX ** 2 + wallGravityY ** 2)

	% Calculate the gravitational pull of planet 1
	const d1x := obstacleX - probeX
	const d1y := obstacleY - probeY
	const obstacleDistance := sqrt (d1x ** 2 + d1y ** 2)
	if obstacleDistance <= obstacleSize or hasch then
	    % Hit Obstacle ?
	    locate (2, maxcol div 2 - 5)
	    put "You Lose!" ..
	    var c : string (1)
	    getch (c)
	    lost := true
	    exit
	end if
	const obstacleGravity := obstacleSize * planetGravity /
	    (obstacleDistance ** 2)
	const divisor1 := abs (d1x) + abs (d1y)
	const obstacleGravityX := obstacleGravity * d1x / divisor1
	const obstacleGravityY := obstacleGravity * d1y / divisor1

	% Calculate the gravitational pull of planet 2
	const d2x := targetX - probeX
	const d2y := targetY - probeY
	const targetDistance := sqrt (d2x ** 2 + d2y ** 2)
	if targetDistance <= targetSize then
	    % Hit Target ?
	    locate (2, maxcol div 2 - 4)
	    put "You Win!" ..
	    var c : string (1)
	    getch (c)
	    exit
	end if
	const targetGravity := targetSize * planetGravity /
	    (targetDistance ** 2)
	const divisor2 := abs (d2x) + abs (d2y)
	const targetGravityX := targetGravity * d2x / divisor2
	const targetGravityY := targetGravity * d2y / divisor2

	% Calculate the acceleration of the probe (x, y and total)
	const probeAccelerationX := wallGravityX + obstacleGravityX +
	    targetGravityX
	const probeAccelerationY := wallGravityY + obstacleGravityY +
	    targetGravityY

	% Add the probe's acceleration to its velocity
	probeVelocityX += probeAccelerationX
	probeVelocityY += probeAccelerationY

	% Add the probe's velocity to its position
	probeX += probeVelocityX
	probeY += probeVelocityY

	drawfilloval (round (probeX), round (probeY), 3, 3, probeColour)
	delay (34)
    end loop

    % end the game if you lost
    exit when lost

    % otherwise make the next round a little more difficult
    targetSize -= 2
    obstacleSize += 2

    % end the game if you have a winner!
    % (Note: The programmer of this game has yet to see anyone win.)
    exit when targetSize < 5

    successes += 1
end loop

% Print the ending message
cls
locate (10, 2)
if targetSize < 5 then
    put "You have successfully completed the"
    locate (11, 13)
    put "missions!"
    locate (12, 9)
    put "Congratulations!!!"
    locate (14, 3)
    put "You are awarded your Master Pilot Wings"
    locate (15, 3)
    put "         With Nova Clusters"
elsif successes < 1 then
    put "You failed to make any interceptions!"
elsif successes < 2 then
    put "You made one successful interception."
else
    put "You made ", successes, " successful interceptions."
end if
put skip, skip
