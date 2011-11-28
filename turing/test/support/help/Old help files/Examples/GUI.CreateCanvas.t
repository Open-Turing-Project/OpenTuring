% The "GUI.CreateCanvas" program.
import GUI
var canvas : int := GUI.CreateCanvas (10, 10, maxx - 20, maxy - 20)
for i : 1 .. 10
    var x : int := Rand.Int (0, maxx - 20)
    var y : int := Rand.Int (0, maxy - 20)
    var c : int := Rand.Int (0, maxcolor)
    GUI.DrawFillOval (canvas, x, y, 20, 20, c)
end for
