% The "CheeseSlice" program
% Draws a slice of green cheese
% that is one sixth of a round cheese
% by drawing circular arcs
setscreen ("graphics")
const maxRadius := 50
const xcenter := maxx div 2
const ycenter := maxy div 2
const theta := 60
for radius : 1 .. maxRadius
    drawarc (xcenter, ycenter, radius, radius, 0, theta, green)
end for
