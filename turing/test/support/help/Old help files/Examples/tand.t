% The "tand" program.

% Note that this program quits attempting to calculate the tangent of
% 90 degrees, causing an overflow.
for i : 0 .. 12
    const angle := i * 30
    put "Tan of ", angle, " is ", tand (angle)
end for
