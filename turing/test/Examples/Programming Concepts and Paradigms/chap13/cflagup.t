% File "cflagup.t" program

include "canflag.ti"
include "ohcanada.ti"

% Draw a Canadian flag and store the picture of it in flagPic.
const width : int := 200
const height : int := 100
Draw.FillBox (0, 0, maxx, maxy, cyan)
DrawCanFlag (0, 0, width)
var flagPic := Pic.New (0, 0, width, height)

% Turn the screen to a background color.
Draw.FillBox (0, 0, maxx, maxy, cyan)
% Store a flag sized piece of the background.
var backgroundPic := Pic.New (0, 0, width, height)

% Draw a flagpole.
Draw.FillBox (210, 10, 214, maxy - 10, darkgray) % Draw pole.
Draw.FillOval (212, maxy - 6, 4, 4, darkgray) % Draw ball on pole.

% Draw initial flag.
Pic.Draw (flagPic, 215, 10, picCopy)

% Start playing Oh Canada.
fork PlayOhCanada

% Raise it up the flag pole.
for y : 10 .. maxy - (height + 14) by 4
    Time.Delay (60)
    Pic.Draw (backgroundPic, 215, y, picCopy) % Erase previous picture.
    Pic.Draw (flagPic, 215, y + 4, picCopy) % Draw the next picture.
end for

