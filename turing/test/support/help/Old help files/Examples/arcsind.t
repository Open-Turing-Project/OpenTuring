% The "arcsind" program.

% Show arcsind is the inverse function of sind, except the result from
% arcsind only goes from -90 to 90.
var angle := 30
var c : real := sind (angle)
put "angle = ", angle
put "sind of angle = ", c
put "arcsind of ", c, " = ", arcsind (c), skip

% Show different values for arcsin
for i : -4 .. 4
    const arg := i / 4
    put "Arc sine of ", arg, " is ", arcsind (arg), " degrees"
end for
