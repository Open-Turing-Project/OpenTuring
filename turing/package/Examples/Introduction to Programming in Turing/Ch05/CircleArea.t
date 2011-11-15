% The "CircleArea" program
% Computes the area of a circle given its radius
const pi : real := 3.14159
var radius: real
var area: real
% Ask the user for the radius
put "Enter radius " ..
get radius
% Calculate the circle's area
area := pi * radius**2
% Output the result
put "Area is ", area
