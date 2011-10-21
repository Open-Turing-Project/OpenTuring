% The "StarLight" program
% Draws lines from center to random points
setscreen ("graphics")
var x, y, c : int
const centerx := maxx div 2
const centery := maxy div 2
loop
    randint (x, 0, maxx)
    randint (y, 0, maxy)
    randint (c, 0, maxcolor)
    drawline (centerx, centery, x, y, c)
end loop
