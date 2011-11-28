% The "SpotLight" program.

%
% First create the background image.
%
var newColour : int
setscreen ("graphics:400,256")
% This is so you don't see the background image being created.  If
% you wan't to see it created, then comment this line out.
setscreen ("invisible")

% Create a bunch of shades of red and draw them across the background.
for decreasing redshade : 255 .. 56
    newColour := RGB.AddColour (redshade / 255, 0, 0)
    drawline ((255 - redshade), 0, (255 - redshade), maxy, newColour)
    drawline (maxx - (255 - redshade), 0, maxx - (255 - redshade), maxy, newColour)
end for

% Output Turing in big lettes
var newFont := Font.New ("serif:72")
Font.Draw ("Turing!", 50, 100, newFont, white)

% Take a picture.  Note the picture is exactly the size of the window.
var pic := Pic.New (0, 0, maxx, maxy)

%
% Now produce the spotlight effect
%

% This is the "spotlight"
var x1 : int := 100
var y1 : int := 100
var x2 : int := maxx - 120
var y2 : int := 80
var dx1 := 3
var dy1 := 3
var dx2 := -4
var dy2 := 4
const RADIUS := 50

cls
setscreen ("visible")           % Make the window visible
setscreen ("offscreenonly")     % Use this for smooth animation

% This is the main loop
loop
    % Move the spotlights
    if x1 - RADIUS + dx1 < 0 or x1 + RADIUS + dx1 > maxx then
	dx1 := -dx1
    end if
    if y1 - RADIUS + dy1 < 0 or y1 + RADIUS + dy1 > maxy then
	dy1 := -dy1
    end if
    x1 += dx1
    y1 += dy2
    if x2 - RADIUS + dx2 < 0 or x2 + RADIUS + dx2 > maxx then
	dx2 := -dx2
    end if
    if y2 - RADIUS + dy2 < 0 or y2 + RADIUS + dy2 > maxy then
	dy2 := -dy2
    end if
    x2 += dx2
    y2 += dy2

    cls
    Draw.FillBox (0, 0, maxx, maxy, black)
    color (white)
    colorback (black)
    put "Marquee effect of white Turing on red background"
    % Set the clipping rectangle.
    View.ClipOff
    View.ClipAddOval (x1, y1, RADIUS, RADIUS)
    View.ClipAddOval (x2, y2, RADIUS, RADIUS)
    % Draw the entire picture.  Note that the part of the picture
    % in the clipping rectangle will actually appear.
    Pic.Draw (pic, 0, 0, picCopy)
    % Turn clipping off.
    View.ClipOff
    % Update the entire window.  The entire window redraws at once,
    % eliminating flashing.
    View.Update
    % Delay so the animation doesn't go too quickly.
    delay (20)
end loop
