% The "Pic.Free" program.
var picID, bgID : int
var x, y, c, direction : int

% Create the picture being moved
Draw.FillBox (50, 50, 150, 150, brightred)
Draw.FillStar (50, 50, 150, 150, brightgreen)
Draw.FillOval (100, 100, 30, 30, brightblue)
picID := Pic.New (50, 50, 150, 150)

% Create a background
for i : 1 .. 1000
    x := Rand.Int (0, maxx)
    y := Rand.Int (0, maxy)
    c := Rand.Int (9, 15)
    Draw.FillBox (x, y, x + 30, y + 30, c)
end for

x := 1
y := 100
direction := 1
% Main loop
loop
    % Take a picture of the background 
    bgID := Pic.New (x, y, x + 100, y + 100)
    Pic.Draw (picID, x, y, picCopy)     % Draw the picture
    delay (20)                          % Delay 20 milliseconds
    Pic.Draw (bgID, x, y, picCopy)      % Draw the background over the picture
    Pic.Free (bgID)                     % Free the background
    if x <= 0 or (x + 100) >= maxx then
	direction := -direction
    end if
    x += direction
end loop
