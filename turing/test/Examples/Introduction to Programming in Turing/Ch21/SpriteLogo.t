% The "SpriteLogo" program
% This program animates a graphic around the window using
% sprites.

var logoPicID, spriteID : int
% The picture IDs of the logo and the background
var logoWidth, logoHeight : int
% The width and height of the logo picture
var x, y : int := 0     % The location of the logo
var dx, dy : int := 3   % The direction of movement of the logo

% Load the logo from file and get its height and width
logoPicID := Pic.FileNew ("logo.bmp")
logoWidth := Pic.Width (logoPic)
logoHeight := Pic.Height (logoPic)

% Create the sprite
spriteID := Sprite.New (logoPicID)
Sprite.SetPriority (spriteID, 3)
Sprite.Show (spriteID)

% Load the forest picture from the file and draw it to the window
Pic.ScreenLoad ("forest.bmp", 0, 0, picCopy)

loop
    Sprite.SetPosition (spriteID, x, y)
    delay (50)
    if x + dx < 0 or x + dx + logoWidth > maxx then
	dx := - dx
    end if
    if y + dy < 0 or y + dy + logoHeight > maxy then
	dy := - dy
    end if
    x := x + dx
    y := y + dy
end loop
