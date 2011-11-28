% The "Draw.Dot" program.
View.Set ("graphics")
var x, y, c : int
loop
    x := Rand.Int (0, maxx)             % Random x
    y := Rand.Int (0, maxy)             % Random y
    c := Rand.Int (0, maxcolor)         % Random color
    Draw.Dot (x, y, c)
end loop
