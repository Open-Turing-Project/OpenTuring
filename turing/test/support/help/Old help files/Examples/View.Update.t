% The "View.Update" program.
% Place some circles around the screen
const RADIUS : int := 30
const NUM_BALLS : int := 20
var x, y, dx, dy, clr : array 1 .. NUM_BALLS of int
for i : 1 .. NUM_BALLS
    x (i) := Rand.Int (RADIUS, maxx - RADIUS)
    y (i) := Rand.Int (RADIUS, maxy - RADIUS)
    dx (i) := Rand.Int (-3, 3)
    dy (i) := Rand.Int (-3, 3)
    clr (i) := Rand.Int (1, 15)
end for

% Now, any drawing to the screen won't appear until a
% View.Update is given.
View.Set ("offscreenonly")
loop
    cls         % Clear the offscreen window
    for i : 1 .. NUM_BALLS
	if x (i) + dx (i) < RADIUS or
		x (i) + dx (i) > maxx - RADIUS then
	    dx (i) := -dx (i)
	end if
	if y (i) + dy (i) < RADIUS or
		y (i) + dy (i) > maxy - RADIUS then
	    dy (i) := -dy (i)
	end if
	x (i) := x (i) + dx (i)
	y (i) := y (i) + dy (i)
	Draw.FillOval (x (i), y (i), RADIUS, RADIUS, clr (i))
    end for
    % All the circles have been drawn.  Now update the screen.
    View.Update
    Time.Delay (5)
end loop
