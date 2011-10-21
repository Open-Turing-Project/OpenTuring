% The "GUI.CreateVerticalScrollBar Example" program.

import GUI

View.Set ("graphics:100;230,nobuttonbar")

var scrollBar : int

procedure ScrollBarMoved (value : int)
    Text.Locate (9, 7)
    put "Scroll"
    Text.Locate (10, 8)
    put "Bar"
    Text.Locate (11, 8)
    put value : 3
end ScrollBarMoved

scrollBar := GUI.CreateVerticalScrollBar (10, 10, 180, 50, 150, 50,
    ScrollBarMoved)
loop
    exit when GUI.ProcessEvent
end loop
