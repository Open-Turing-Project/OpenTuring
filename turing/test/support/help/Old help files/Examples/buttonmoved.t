% The "buttonmoved" program.
var circles : boolean := true
loop
    var x, y, radius, clr : int
    if buttonmoved ("down") then
	var buttonnumber, buttonupdown : int
	buttonwait ("down", x, y, buttonnumber, buttonupdown)
	circles := not circles
    end if
    randint (x, 0, maxx)
    randint (y, 0, maxy)
    randint (radius, 0, 100)
    randint (clr, 0, maxcolor)
    if circles then
	drawfilloval (x, y, radius, radius, clr)
    else
	drawfillbox (x, y, x + radius, y + radius, clr)
    end if
    locate (1, 1)
    put "Click a mouse button to make change the shape being drawn "..
    delay (50)
end loop
