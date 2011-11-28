% The "GUI.SetColor" program.
import GUI

proc DoNothing
end DoNothing

var b1 := GUI.CreateButton (100, 100, 0, "Push Me", DoNothing)
GUI.SetColor (b1, brightred)
var b2 := GUI.CreateButton (100, 140, 0, "Push Me", DoNothing)
GUI.SetColor (b2, brightgreen)
var b3 := GUI.CreateButton (100, 180, 0, "Push Me", DoNothing)
GUI.SetColor (b3, brightblue)
loop
    exit when GUI.ProcessEvent
end loop
