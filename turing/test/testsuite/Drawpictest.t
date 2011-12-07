setscreen ("graphics")
drawline(0,0,70,70,black)
% Create buffer big enough to hold happy face
var face : array 1 .. sizepic (0, 0, 100, 100) of int
% Copy picture into the buffer, which is the face array
takepic (0, 0, 100, 100, face)
% Redraw the picture with its left bottom at (200,0)
drawpic (200, 0, face, 0)
