% The "ManyCircleAreas" program
% Computes the areas of circles
var radius, area : real
const pi := 3.14159
loop
    put "Enter radius " ..
    get radius
    area := pi * radius ** 2
    put "Area is ", area
end loop
