% The "arccosd" program.

% Show arccosd is the inverse function of cosd, except the result from
% arccosd only goes from 0 to 180
var angle := 60
var c : real := cosd (angle)
put "angle = ", angle
put "cosd of angle = ", c
put "arccosd of ", c, " = ", arccosd (c), skip

% Show different values for arccos
for i : -4 .. 4
    const arg := i / 4
    put "Arc cosine of ", arg, " is ", arccosd (arg), " degrees"
end for
