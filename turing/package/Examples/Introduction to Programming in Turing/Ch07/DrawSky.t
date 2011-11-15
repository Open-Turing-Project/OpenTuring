% The "DrawSky" program
% Color the whole window blue
colorback (1)
for row : 1 .. 25
    for column : 1 .. 80
	locate (row, column)
	% Output a blank
	put " " ..
    end for
end for
