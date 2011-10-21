% The "DrawFillExample" program.
% Sets the screen to graphics mode.
setscreen ("graphics")
drawoval (320, 200, 25, 40, red)
drawfill (320, 200, yellow, red)
% The 320, 200 is a point inside of the oval (the center).
% The drawfill will fill in the oval. The oval will be filled in yellow
% until it reaches the border colour which is red.
