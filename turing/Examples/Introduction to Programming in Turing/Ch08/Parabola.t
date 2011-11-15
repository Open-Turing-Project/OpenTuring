% The "Parabola" program
% Draws the graph of the function y = x ** 2
% for x going from 0 to 14 in steps of .05
% Draw axes
drawline (0, 0, maxx, 0, blue)
drawline (0, 0, 0, maxy, blue)
for pixel : 0 .. 280
    const x := .05 * pixel
    drawdot (pixel, round (x ** 2), cyan)
end for
