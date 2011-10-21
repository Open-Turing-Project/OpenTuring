% The "MouseWhack" program.
var failedClicks, missedBlocks : int := 0
var exitLoopTime, currentTime : int
var x, y, clr, duration : int
var mx, my, dummy1, dummy2 : int
const SIZE : int := 20
% The outer loop
loop
    % Set a random location, color, and duration for the square
    randint (x, 0, maxx - SIZE)
    randint (y, 0, maxy - SIZE)
    randint (clr, 1, 15)

    % Draw the square
    drawfillbox (x, y, x + SIZE, y + SIZE, clr)

    clock (exitLoopTime)
    randint (duration, 300, 800)
    exitLoopTime := exitLoopTime + duration
    % The inner loop
    loop
	% Has the duration of the square expired
	clock (currentTime)
	exit when currentTime > exitLoopTime
	% If a button press has occurred
	if buttonmoved ("down") then
	    % get the location of the button press
	    buttonwait ("down", mx, my, dummy1, dummy2)
	    % Is it within the square
	    if x <= mx and mx <= x + SIZE and
		    y <= my and my <= y + SIZE then
		% You clicked the square!
		cls
		put "You WON!  You missed ", failedClicks, " clicks and ",
		    " clicks and ", missedBlocks, " blocks."
		return
	    else
		failedClicks += 1
	    end if
	end if
    end loop
    missedBlocks += 1
    % Erase the square by drawing it in the background color
    drawfillbox (x, y, x + SIZE, y + SIZE, colorbg)
end loop
