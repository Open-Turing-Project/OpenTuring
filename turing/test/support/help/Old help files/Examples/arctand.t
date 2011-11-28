% The "arctand" program.

% Show arctand is the inverse function of tand, except the result from
% arctand only goes from -90 to 90
var angle := 60
var t : real := tand (angle)
put "angle = ", angle
put "tand of angle = ", t
put "arctand of ", t, " = ", arctand (t), skip

% Show different values for arctan
for i : -6 .. 6
    const arg := i / 2
    put "Arc tangent of ", arg, " is ", arctand (arg), " degrees"
end for
