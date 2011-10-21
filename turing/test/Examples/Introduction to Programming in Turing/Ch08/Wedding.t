% The "Wedding" program
% Gives randomly colored dots with greater variety of
% color and higher resolution than CGA graphics
setscreen ("graphics:200;200")
var x, y, c : int
loop
    randint (x, 0, maxx)
    randint (y, 0, maxy)
    randint (c, 0, maxcolor)
    drawdot (x, y, c)
end loop
