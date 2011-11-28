% The "GUI.AddText" program.
import GUI
var boxID : int := GUI.CreateTextBox (50, 50, 200, 200)
for i : 1 .. 26
    GUI.AddText (boxID, intstr (i))
    GUI.AddText (boxID, " ")
    GUI.AddLine (boxID, chr (64 + i))
end for
loop
    exit when GUI.ProcessEvent
end loop
