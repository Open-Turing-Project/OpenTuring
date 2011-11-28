% The "GUI.CreateCheckBoxFull" program.
import GUI

procedure DoNothing (status : boolean)
end DoNothing

View.Set ("graphics:300;100,nobuttonbar")
var cb1 : int := GUI.CreateCheckBox (10, 10, "Check Box 1",
    DoNothing)
var cb2 : int := GUI.CreateCheckBoxFull (200, 10, "Check Box 2",
    DoNothing, GUI.RIGHT, '2')
GUI.SetCheckBox (cb2, true)
var quitBtn : int := GUI.CreateButton (230, 10, 0, "Quit", GUI.Quit)
loop
    exit when GUI.ProcessEvent
end loop
var cb1Status : boolean := GUI.GetCheckBox (cb1)
var cb2Status : boolean := GUI.GetCheckBox (cb2)
if cb1Status then
    put "Check box 1: filled"
else
    put "Check box 1: empty"
end if
if cb2Status then
    put "Check box 2: filled"
else
    put "Check box 2: empty"
end if
