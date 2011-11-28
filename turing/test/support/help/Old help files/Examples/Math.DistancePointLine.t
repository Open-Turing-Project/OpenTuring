% The "Math.DistancePointLine" program.
var xp, yp, x1, y1, x2, y2 : int
loop
    const RADIUS : int := 100
    xp := Rand.Int (RADIUS, maxx - RADIUS)
    yp := Rand.Int (RADIUS, maxy - RADIUS)
    x1 := Rand.Int (0, maxx)
    y1 := Rand.Int (0, maxy)
    x2 := Rand.Int (0, maxx)
    y2 := Rand.Int (0, maxy)
    cls
    Draw.FillOval (xp, yp, RADIUS, RADIUS, brightred)
    Draw.Line (x1, y1, x2, y2, blue)
    if Math.DistancePointLine (xp, yp, x1, y1, x2, y2) < RADIUS then
	put "The circle touches the line"
    else
	put "The circle does not touch the line"
    end if
    delay (2000)
    exit when hasch
end loop
