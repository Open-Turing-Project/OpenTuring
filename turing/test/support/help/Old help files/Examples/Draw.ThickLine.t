% The "Draw.ThickLine" program.
View.Set ("graphics")
% First draw a line of 5 pixels width from the left bottom to right top
Draw.ThickLine (0, 0, maxx, maxy, 5, brightred)
% Now draw a line  of 5 pixels width from the left top to right bottom
Draw.ThickLine (0, maxy, maxx, 0, 5, brightgreen)
