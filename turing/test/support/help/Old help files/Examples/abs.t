% The "abs" program.
put abs (-6.5)
put abs (7)

% Here is an example of using abs to detect whether one
% number is within a given distance of another.
var x, y : real
x := Rand.Real
y := Rand.Real
if abs (x - y) < 0.2 then
    put x, " and ", y, " are within 0.2 of each other"
else
    put x, " and ", y, " differ by more than 0.2"
end if
