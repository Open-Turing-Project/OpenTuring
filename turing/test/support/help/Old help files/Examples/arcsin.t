% The "arcsin" program.

% Show arcsin is the inverse function of sin, except the result from
% arcsin only goes from -PI/2 to PI/2
var angle := Math.PI / 8
var c : real := sin (angle)
put "angle = ", angle
put "sin of angle = ", c
put "arcsin of ", c, " = ", arcsin (c), skip

% Show different values for arcsin
for i : -4 .. 4
    const arg := i / 4
    put "Arc sine of ", arg, " is ", arcsin (arg), " radians"
end for
