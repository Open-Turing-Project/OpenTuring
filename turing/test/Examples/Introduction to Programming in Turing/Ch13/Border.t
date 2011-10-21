% The "Border" program
% Outputs 5 triangles
% Each triangle has 6 lines
const row := "******"
for triangle : 1 .. 5
    for line : 1 .. 6
	put row (1 .. line)
    end for
end for
