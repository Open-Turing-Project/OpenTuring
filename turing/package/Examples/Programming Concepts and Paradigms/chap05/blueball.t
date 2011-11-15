% File "blueball.t".
% Draw a blue ball centered in window.

% Definition of ball procedure.
procedure ball (xc, yc, radius, clr : int)
    Draw.FillOval (xc, yc, radius, radius, clr)
end ball

ball (maxx div 2, maxy div 2, maxy div 2, blue)
