% The "arccos" program.

% Show arccos is the inverse function of cos, except the result from
% arccos only goes from 0 to PI
var angle := Math.PI / 8
var c : real := cos (angle)
put "angle = ", angle
put "cos of angle = ", c
put "arccos of ", c, " = ", arccos (c), skip

% Show different values for arccos
for i : -4 .. 4
    const arg := i / 4
    put "Arc cosine of ", arg, " is ", arccos (arg), " radians"
end for
