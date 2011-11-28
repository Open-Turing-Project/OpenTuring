% The "arctan" program.

% Show arctan is the inverse function of tan, except the result from
% arctan only goes from -PI/2 to PI/2
var angle := Math.PI / 8
var t : real := tan (angle)
put "angle = ", angle
put "tan of angle = ", t
put "arctan of ", t, " = ", arctan (t), skip

% Show different values for arctan
for i : -6 .. 6
    const arg := i / 2
    put "Arc tangent of ", arg, " is ", arctan (arg), " radians"
end for
