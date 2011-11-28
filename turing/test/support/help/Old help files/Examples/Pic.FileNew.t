% The "Pic.FileNew" program.
var picID : int
var x, y : int

picID := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/lighthouse.jpg")
for i : 1 .. 10
    x := Rand.Int (0, maxx)             % Random x
    y := Rand.Int (0, maxy)             % Random y
    Pic.Draw (picID, x, y, picCopy)
end for
Pic.Free (picID)
