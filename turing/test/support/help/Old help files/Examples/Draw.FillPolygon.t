% The "Draw.FillPolygon" program.
View.Set ("graphics")
var x : array 1 .. 8 of int := init (100, 100, 135, 185,
    220, 220, 185, 135)
var y : array 1 .. 8 of int := init (100, 150, 185, 185,
    150, 100, 65, 65)
Draw.FillPolygon (x, y, 8, brightcyan)
Draw.Polygon (x, y, 8, black)
