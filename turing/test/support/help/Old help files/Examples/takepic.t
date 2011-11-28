% The "takepic" program.
setscreen ("graphics")
% Draw the happy face.
drawoval (50, 50, 50, 50, brightred)
drawarc (50, 50, 40, 40, 180, 360, brightgreen)
drawfilloval (40, 70, 5, 5, brightblue)
drawfilloval (60, 70, 5, 5, brightblue)
% Create buffer big enough to hold happy face
var face : array 1 .. sizepic (0, 0, 100, 100) of int
% Copy picture into the buffer, which is the face array
takepic (0, 0, 100, 100, face)
% Redraw the picture with its left bottom at (200,0)
drawpic (200, 0, face, 0)
% Wait two seconds
delay (2000)
% Erase the face by drawing it in the same place in mode 1.
drawpic (200, 0, face, 1)
