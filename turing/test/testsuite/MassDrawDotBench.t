setscreen("offscreenonly")
var mode : boolean := true
loop
    for x : 1 .. maxx
	for y : 1 .. maxy
	    var val : int
	    if mode then
		val := (x & y) mod 255
	    else
		val := (x xor y) mod 255
	    end if
	    Draw.Dot (x, y, val)
	end for
    end for
    mode := not mode
    View.Update
end loop
