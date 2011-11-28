% The "View.WhatDotColor2" program.
const side : int := 30
for x : 0 .. maxx by side
    for y : 0 .. maxy by side
	var c : int := Rand.Int (0, maxcolour)
	Draw.FillBox (x, y, x + side, y + side, c)
	Font.Draw (intstr (c), x + 3, y + side div 2, defFontID, black)
    end for
end for

var x, y, b : int
loop
    Mouse.Where (x, y, b)
    locate (1, 1)
    put View.WhatDotColor (x, y) : 3, " " ..
end loop
