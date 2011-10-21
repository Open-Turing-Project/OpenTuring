% The "FancyBorder" program
% Outputs alternating triangles
const row := "******"
for triangle : 1 .. 5
    if triangle mod 2 = 0 then
	for line : 1 .. 6
	    put row (1 .. line)
	end for
    else
	for line : 1 .. 6
	    put row (line .. *)
	end for
    end if
end for
