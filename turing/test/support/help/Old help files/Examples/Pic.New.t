% The "Pic.New" program.
var picID : int
var x, y : int
Draw.FillBox (50, 50, 150, 150, red)
Draw.FillStar (50, 50, 150, 150, green)
Draw.FillOval (100, 100, 30, 30, blue)

picID := Pic.New (50, 50, 150, 150)
for i : 1 .. 50
    x := Rand.Int (0, maxx)             % Random x
    y := Rand.Int (0, maxy)             % Random y
    Pic.Draw (picID, x, y, picCopy)
end for
Pic.Free (picID)
