% The "View.WhatDotColor" program.
View.Set ("graphics")
var x : int := 20
var y : int := 0
var dx, dy : int := 1
loop
    if View.WhatDotColor (x, y) not= 0 and
	    View.WhatDotColor (x, y) not= brightred then
	Draw.FillOval (x, y, 10, 10, brightred)
	Music.Sound (440, 30)
    end if
    Draw.Dot (x, y, 1)
    x := x + dx
    y := y + dy
    if x = 0 or x = maxx then
	dx := -dx
	Music.Sound (440, 30)
    end if
    if y = 0 or y = maxy then
	dy := -dy
	Music.Sound (440, 30)
    end if
    delay (10)
end loop
