% The "Pic.Blur1" program.
var f := Font.New ("serif:60:bold,italic,noantialias")
View.Set ("graphics:510;90,nobuttonbar")
Font.Draw ("Turing", 150, 20, f, black)
var p : array 1 .. 50 of int
p (1) := Pic.New (0, 0, maxx, maxy)
cls
for i : 2 .. 50
    locate (1, 1)
    put i, " of ", 50
    p (i) := Pic.Blur (p (i - 1), 10)
end for
for decreasing i : 50 .. 1
    Pic.Draw (p (i), 0, 0, picCopy)
    delay (40)
end for
