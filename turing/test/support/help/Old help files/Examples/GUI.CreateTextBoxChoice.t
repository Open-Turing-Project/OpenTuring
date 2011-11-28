% The "GUI.CreateTextBoxChoice" program.
import GUI

setscreen ("graphics:300;200,nobuttonbar")
procedure ChoseLine (line : int)
    locate (1, 1)
    put "The user chose ", line
end ChoseLine

var x : int := GUI.CreateTextBoxChoice (20, 20, 200, 100, 0, 0, ChoseLine)
GUI.AddLine (x, "Choose A")
GUI.AddLine (x, "Choose B")
GUI.AddLine (x, "Choose C")
GUI.AddLine (x, "Choose D")
GUI.AddLine (x, "Choose E")
GUI.AddLine (x, "Choose F")
GUI.AddLine (x, "Choose G")
GUI.AddLine (x, "Choose H")
GUI.AddLine (x, "Choose I")
GUI.AddLine (x, "Choose J")
loop
    exit when GUI.ProcessEvent
end loop
