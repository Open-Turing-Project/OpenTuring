% The "DrawBox" program
% Draws a box of width, height,
% position, and color that you specify
var w, h, x, y, c : int
put "Enter width of box in pixels: " ..
get w
put "Enter height of box in pixels: " ..
get h
put "Enter x-coordinate of lower-left corner: " ..
get x
put "Enter y-coordinate of lower-left corner: " ..
get y
put "Enter color number: " ..
get c
setscreen ("graphics")
drawline (x, y, x + w, y, c)            % Base of box
drawline (x + w, y, x + w, y + h, c)    % Right side
drawline (x + w, y + h, x, y + h, c)    % Top
drawline (x, y + h, x, y, c)            % Left side
drawfill (x + 1, y + 1, c, c)           % Fill the box
