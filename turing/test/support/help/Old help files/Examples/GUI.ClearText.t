% The "GUI.ClearText" program.
import GUI
var boxID, buttonID, start : int
start := 1

procedure PrintTwentyFive
    GUI.ClearText (boxID)
    for i : start .. start + 24
	GUI.AddLine (boxID, intstr (i))
    end for
    start += 25
end PrintTwentyFive

boxID := GUI.CreateTextBox (50, 50, 200, 200)
buttonID := GUI.CreateButton (50, 5, 0, "Next 25", PrintTwentyFive)
PrintTwentyFive
loop
    exit when GUI.ProcessEvent
end loop
