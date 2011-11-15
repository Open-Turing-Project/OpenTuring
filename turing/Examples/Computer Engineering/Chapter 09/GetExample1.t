% The "GetExample1" program.
% Variable declaration to list the variables being used declared as integers.
var x1, y1, x2, y2 : int
% Sets the screen to graphics mode.
setscreen ("graphics")
put "Enter the x value of the bottom-left corner of the box."
get x1
% x1 is the x value of the bottom-left of the box.
put "Enter the y value of the bottom-left corner of the box."
get y1
% y1 is the y value of the bottom-left corner of the box.
put "Enter the x value of the top-right corner of the box."
get x2
put "Enter the y value of the top-right corner of the box."
get y2
% x2, y2 are the coordinates of the top-right corner of the box.
drawbox (x1, y1, x2, y2, yellow)
% The box is now drawn in yellow.
