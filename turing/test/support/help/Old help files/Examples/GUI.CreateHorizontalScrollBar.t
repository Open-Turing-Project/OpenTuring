% The "GUI.CreateHorizontalScrollBar" program.
import GUI

View.Set ("graphics:300;60,nobuttonbar")
var scrollBar : int

procedure ScrollBarMoved (value : int)
    Text.Locate (2, 3)
    put "Horizontal Scroll Bar: ", value : 4
end ScrollBarMoved

scrollBar := GUI.CreateHorizontalScrollBar (10, 10, 250,
    50, 150, 50, ScrollBarMoved)
loop
    exit when GUI.ProcessEvent
end loop
