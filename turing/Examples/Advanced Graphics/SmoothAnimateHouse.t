% The "SmoothAnimateHouse" program.

% This program demonstrates the View.Update and setscreen ("offscreenonly")

% Draw a house
drawbox (30, 30, 100, 100, black)
drawbox (50, 30, 80, 60, brightred)
drawbox (35, 70, 55, 90, green)
drawline (45, 70, 45, 90, green)
drawline (35, 80, 55, 80, green)
drawbox (75, 70, 95, 90, green)
drawline (85, 70, 85, 90, green)
drawline (75, 80, 95, 80, green)
drawline (30, 100, 65, 135, brightblue)
drawline (65, 135, 100, 100, brightblue)
drawline (90, 110, 90, 150, black)
drawline (75, 125, 75, 150, black)
drawline (75, 150, 90, 150, black)
locate (maxrow, 2)
put "Home sweet home" ..

% Create the picture
const PIC_WIDTH := 130
const PIC_HEIGHT := 160
var pic := Pic.New (0, 0, PIC_WIDTH, PIC_HEIGHT)

% Animate 10 of them around the screen
var x, y, dx, dy : array 1 .. 10 of int
cls
for i : 1 .. 10
    x (i) := Rand.Int (10, maxx - 10 - PIC_WIDTH)
    y (i) := Rand.Int (10, maxy - 10 - PIC_HEIGHT)
    dx (i) := Rand.Int (-3, 3)
    dy (i) := Rand.Int (-3, 3)
end for

loop
    cls
    locate (maxrow, 10)
    put "Old Animation Technique.  Press any key to see the new technique" ..
    for i : 1 .. 10
	if x (i) + dx (i) < 0 or x (i) + dx (i) + PIC_WIDTH > maxx then
	    dx (i) := -dx (i)
	end if
	if y (i) + dy (i) < 0 or y (i) + dy (i) + PIC_HEIGHT > maxy then
	    dy (i) := -dy (i)
	end if
	x (i) := x (i) + dx (i)
	y (i) := y (i) + dy (i)
	Pic.Draw (pic, x (i), y (i), picMerge)
    end for
    exit when hasch
end loop

% Read the character from the buffer.
var ch : string (1)
getch (ch)

% Now, any drawing to the screen won't appear until a View.Update is
% given.  Note that you can turn this off with 
setscreen ("offscreenonly")
loop
    cls
    locate (maxrow, 10)
    put "New Animation Technique.  Press any key to quit" ..
    for i : 1 .. 10
	if x (i) + dx (i) < 0 or x (i) + dx (i) + PIC_WIDTH > maxx then
	    dx (i) := -dx (i)
	end if
	if y (i) + dy (i) < 0 or y (i) + dy (i) + PIC_HEIGHT > maxy then
	    dy (i) := -dy (i)
	end if
	x (i) := x (i) + dx (i)
	y (i) := y (i) + dy (i)
	
	% Note this only draws on the offscreen window.  Nothing appears
	% in the visible window.
	Pic.Draw (pic, x (i), y (i), picMerge)
    end for
    % All the houses have been drawn.  Now update the screen.
    View.Update
    exit when hasch
end loop
setscreen ("nooffscreenonly")
