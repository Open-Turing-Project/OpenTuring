% The "RandomTriangles" program
% Outputs 5 triangles of random size between 3 and 6

procedure triangles (size : int)
    % Outputs a triangle of size asterisks
    for i : 1 .. size
	put repeat ("*", i)
    end for
end triangles

var howBig : int
for i : 1 .. 5
    randint (howBig, 3, 6)
    triangles (howBig)
end for
