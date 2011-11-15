% File "ballrow.t". Draw a row of balls.
include "ballproc.ti"
const size := maxx div 10  % Radius of the ball.
const height := maxy div 2  % Distance of center of balls from bottom.
for count : 1 .. 10 by 2
    ball (count * size, height, size, green)
end for
