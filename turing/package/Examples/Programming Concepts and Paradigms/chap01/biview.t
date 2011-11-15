% File "biview.t". Draws two windows
% using the Win class, one with a ball, the other with a block.
import Win in "win.tu", Shape in "shape.tu"
var wind1, wind2 : ^Win  % Declare two pointers to Win class.
% Allocate the first window.
new wind1
% Allocate the second window.
new wind2
% Determine the screen dimensions.
const screenWidth := Config.Display (cdScreenWidth)
const screenHeight := Config.Display (cdScreenHeight)
% Set position, size, and title of window1.
wind1 -> SetSize (screenWidth div 2, screenHeight div 2 - 30)
wind1 -> SetPosition (0, screenHeight div 2)
wind1 -> SetTitle ("Upper")
% Set position, size, and title of window2.
wind2 -> SetSize (screenWidth div 2, screenHeight div 2)
wind2 -> SetPosition (0, 0)
wind2 -> SetTitle ("Lower")
% Draw a blue ball in window 1.
wind1 -> SetActive  % Make window 1 active.
Shape.ball (maxx div 2, maxy div 2, maxy div 4, blue)
wind1  -> Show  % Make window 1 visible.
% Draw a green block in window 2.
wind2 -> SetActive  % Make window 2 active.
Shape.block (maxx div 4, maxy div 4, maxy div 2, green)
wind2  -> Show  % Make window 2 visible.
Shape.hold
% Move the upper window to top  right of screen.
wind1 -> SetPosition (screenWidth div 2, screenHeight div 2)
wind2 -> SetActive  % Make window 2 active.
Shape.hold
% Move the lower window to lower right.
wind2 -> SetPosition (screenWidth div 2, 0)
Shape.hold

