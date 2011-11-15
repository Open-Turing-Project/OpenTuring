% The "KeyDown" program.
var a : array char of boolean
setscreen ("noecho")
loop
    Input.KeyDown (a)

    locate (1, 1)
    for r : 0 .. 15
	for c : 0 .. 15
	    var z : char := chr (r * 16 + c)
	    if a (z) then
		put 1 : 2 ..
	    else
		put 0 : 2 ..
	    end if
	end for
	put ""
    end for
    put skip
    for c : 0 .. 255
	var z : char := chr (c)
	if a (z) then
	    put c : 4 ..
	end if
    end for
    put ""
end loop
