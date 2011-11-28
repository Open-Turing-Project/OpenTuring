% The "Math.Distance" program.
const RADIUS := 75
loop
    var x1, y1, x2, y2 : int
    x1 := Rand.Int (RADIUS, maxx - RADIUS)
    y1 := Rand.Int (RADIUS, maxy - RADIUS)
    x2 := Rand.Int (RADIUS, maxx - RADIUS)
    y2 := Rand.Int (RADIUS, maxy - RADIUS)
    cls
    drawfilloval (x1, y1, RADIUS, RADIUS, brightred)
    drawfilloval (x2, y2, RADIUS, RADIUS, brightgreen)
    locate (1, 1)
    if Math.Distance (x1, y1, x2, y2) < RADIUS * 2 then
	put "The two circles touch"
    else
	put "The two circles do not touch"
    end if
    delay (2000)
    exit when hasch
end loop
