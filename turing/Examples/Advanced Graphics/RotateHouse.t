% The "RotateHouse" program.

% This program demonstrates the Pic.Rotate

% Some notes about this program
% (1) It takes a long time to rotate a picture.  Do all the rotations
%     before the program begins.
% (2) When rotating around a point, make certain there's enough space
%     around the original picture to fit the rotated picture.  Note how
%     the chimney and the words go off the left and bottom.  It would have
%     been better to take a bigger initial picture with more space to the
%     left.
% (3) If you are not rotating an object in place, then pass -1 for the 
%     x and y axis.  You are then guaranteed the rotated picture will fit,
%     however, it won't be rotated around a given point.
% (4) This program would probably be just as visually appealing if it only
%     used 36 angles instead of 10.

% The rotated pictures.  
var pics : array 0 .. 359 of int

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

% Create the three pictures.
pics (0) := Pic.New (0, 0, 130, 160)
cls
put "Creating the rotated pictures can take a while."
for angle : 1 .. 359
    locate (3, 3)
    put "Creating picture rotated ", angle, " degrees"
    pics (angle) := Pic.Rotate (pics (0), angle, 65, 80)
end for

% Draw the three pictures (with boxes around them)
cls
var angle : int := 0
loop
    Pic.Draw (pics (angle), 100, 100, picCopy)
    angle := (angle + 1) mod 360
    delay (20)
end loop
