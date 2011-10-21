% The "SpriteLogos" program
% This program animates a graphic around the window using
% sprites.

var logoPicID, spriteID : int
% The picture IDs of the logo and the background
var logoWidth, logoHeight : int
% The width and height of the logo picture
var spriteID : array 1 .. 6 of int
var x, y : array 1 .. 6 of int
var dx, dy : array 1 .. 6 of int

% Load the logo from file and get its height and width
logoPicID := Pic.FileNew ("logo.bmp")
logoWidth := Pic.Width (logoPic)
logoHeight := Pic.Height (logoPic)

% Create the sprites
for i : 1 .. 6
    spriteID (i) := Sprite.New (logoPicID)
    Sprite.SetPriority (spriteID (i), i)
    Sprite.Show (spriteID (i))
    x (i) := Rand.Int (logoWidth, maxx - logoWidth)
    y (i) := Rand.Int (logoWidth, maxy - logoWidth)
    dx (i) := Rand.Int ( - 3, 3)
    dy (i) := Rand.Int ( - 3, 3)
end for

% Load the forest picture from the file and draw it to the window
Pic.ScreenLoad ("forest.bmp", 0, 0, picCopy)

loop
    for i : 1 .. 6
	Sprite.SetPosition (spriteID (i), x (i), y (i))
	if x (i) + dx (i) < 0 or x (i) + dx (i) + logoWidth > maxx then
	    dx (i) := - dx (i)
	end if
	if y (i) + dy (i) < 0 or y (i) + dy (i) + logoHeight > maxy then
	    dy (i) := - dy (i)
	end if
	x (i) := x (i) + dx (i)
	y (i) := y (i) + dy (i)
    end for
    delay (50)
end loop
