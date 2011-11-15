% The "Jagged" program
% Outputs a number of triangles

procedure triangle
    % Outputs a triangle of asterisks
    for i : 1 .. 5
	put repeat ("*", i)
    end for
end triangle

var howMany : int
put "How many triangles do you want? " ..
get howMany
for i : 1 .. howMany
    triangle
end for
