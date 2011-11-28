% The "GUI.CreateVerticalScrollBar" program.
import GUI

View.Set ("graphics:125;200,nobuttonbar")
var scrollBar : int

procedure ScrollBarMoved (value : int)
    Text.Locate (6, 7)
    put "Scroll"
    Text.Locate (7, 8)
    put "Bar"
    Text.Locate (8, 8)
    put value : 3
end ScrollBarMoved

scrollBar := GUI.CreateVerticalScrollBar (10, 10, 180,
    50, 150, 50, ScrollBarMoved)
loop
    exit when GUI.ProcessEvent
end loop
