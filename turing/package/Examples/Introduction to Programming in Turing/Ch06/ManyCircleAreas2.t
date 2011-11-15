% The "ManyCircleAreas2" program
% Compute circle areas until you enter a negative radius
var radius, area : real
const pi := 3.14159
put "Enter a negative radius to stop execution"
loop
    put "Enter radius " ..
    get radius
    exit when radius < 0
    area := pi * radius ** 2
    put "Area is ", area
end loop
put "That's all folks"
