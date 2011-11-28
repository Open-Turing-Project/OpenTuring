% The "GUI.CreateButtonFull" program.
import GUI

procedure DrawRandomCircle
    var r : int := Rand.Int (20, 50)
    var x : int := Rand.Int (r, maxx - r)
    var y : int := Rand.Int (r, maxy - r)
    var c : int := Rand.Int (0, maxcolor)
    Draw.FillOval (x, y, r, r, c)
    % In case we drew over the buttons, redraw them.
    GUI.Refresh
end DrawRandomCircle

View.Set ("graphics:300;200,nobuttonbar")
var draw : int := GUI.CreateButtonFull (50, 10, 0, "Draw Circle",
    DrawRandomCircle, 0, '^D', true)
var quitBtn : int := GUI.CreateButton (200, 10, 0, "Quit", GUI.Quit)
loop
    exit when GUI.ProcessEvent
end loop
