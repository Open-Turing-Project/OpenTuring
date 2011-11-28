% The "GUI.AddLine" program.
import GUI
var boxID : int := GUI.CreateTextBox (50, 50, 200, 200)
for i : 1 .. 25
    GUI.AddLine (boxID, intstr (i))
end for
loop
    exit when GUI.ProcessEvent
end loop
