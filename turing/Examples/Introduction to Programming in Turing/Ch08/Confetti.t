% The "Confetti" program
% Color pixels in the window randomly
setscreen ("graphics")
var x, y, c : int
loop
    randint (x, 0, maxx)
    randint (y, 0, maxy)
    randint (c, 1, 15)
    drawdot (x, y, c)
end loop
