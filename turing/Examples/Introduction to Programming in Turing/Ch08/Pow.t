% The "Pow" program
% Plots magenta circles of ever increasing radius centered
% in window until the edge of the window is reached
setscreen ("graphics")
const centerx := maxx div 2
const centery := maxy div 2
for radius : 1 .. centery
    drawoval (centerx, centery, radius, radius, magenta)
end for
