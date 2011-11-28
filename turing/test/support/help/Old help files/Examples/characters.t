% The "characters" program.
setscreen ("graphics:502;519")
%setscreen ("msdos")
for i : 0 .. 31
    for j : 0 .. 224 by 32
	var c : int := i + j
	if c = 0 or c = 8 or c = 9 or c = 10 then
	    c := 1
	end if
	put i + j : 3, " " ..
	put chr (c) ..
	if j not= 224 then
	    put "   " ..
	end if
    end for
    if i not= 31 then
	put ""
    end if
end for

% Draw Lines
for i : 0 .. 7
    var x := i * 64
    if i > 3 then
	x -= 3
    end if
    drawbox (x, 4, i * 64 + 48, maxy, black)
    drawline (i * 64 + 26, 4, i * 64 + 26, maxy, black)
end for
