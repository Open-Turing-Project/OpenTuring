% The "GUICircles" program

import GUI in "%oot/lib/GUI"

procedure DrawCircle
    var x, y, r, clr : int
    x := Rand.Int (0, maxx)
    y := Rand.Int (70, maxy)
    r := Rand.Int (20, 50)
    clr := Rand.Int (1, maxcolor)
    Draw.FillOval (x, y, r, r, clr)
end DrawCircle

var button : int := GUI.CreateButton (maxx div 2 - 30, 3, 60, "Draw Circle",
    DrawCircle)
loop
    exit when GUI.ProcessEvent
end loop
