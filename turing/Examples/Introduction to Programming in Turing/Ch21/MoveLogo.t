% The "MoveLogo" program
% This program animates a graphic around the window.

var logoPicID, bgPicID : int
% The picture IDs of the logo and the background
var logoWidth, logoHeight : int
% The width and height of the logo picture
var x, y : int := 0
% The lower-left corner of the logo
var dx, dy : int := 3
% The direction of movement of the logo

% Load the logo from file and get its height and width
logoPicID := Pic.FileNew ("logo.bmp")
logoWidth := Pic.Width (logoPicID)
logoHeight := Pic.Height (logoPicID)

% Load the forest picture from the file and draw it in the window
Pic.ScreenLoad ("forest.bmp", 0, 0, picCopy)

loop
    % Take the picture of the background
    bgPicID := Pic.New (x, y, x + logoWidth, y + logoHeight)
    % Draw the logo
    Pic.Draw (logoPicID, x, y, picMerge)
    delay (50)
    % Draw the background over the logo, effectively erasing the logo
    Pic.Draw (bgPicID, x, y, picCopy)
    % Free up the memory used by the background picture
    Pic.Free (bgPicID)
    % Check if the logo bounced off the left or right edges of the window
    if x + dx < 0 or x + dx + logoWidth > maxx then
	dx := - dx
    end if
    % Check if the logo bounced off the top or bottom edges of the window
    if y + dy < 0 or y + dy + logoHeight > maxy then
	dy := - dy
    end if
    % Change the location of the logo
    x := x + dx
    y := y + dy
end loop
