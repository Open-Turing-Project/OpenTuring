% The "GUI.CreateRadioButton" program.
import GUI
View.Set ("graphics:350;80,nobuttonbar")

var radio : array 1 .. 6 of int         % The radio button IDs.

procedure RadioPressed
    Text.Locate (1, 1)
    put "Radio Button " ..
    for i : 1 .. 6
	if radio (i) = GUI.GetEventWidgetID then
	    put i ..
	end if
    end for
    put " Selected"
end RadioPressed

radio (1) := GUI.CreateRadioButton (15, maxy - 35,
    "Radio Button 1", 0, RadioPressed)
radio (2) := GUI.CreateRadioButton (-1, -1, "Radio Button 2",
    radio (1), RadioPressed)
radio (3) := GUI.CreateRadioButton (-1, -1, "Radio Button 3",
    radio (2), RadioPressed)
radio (4) := GUI.CreateRadioButtonFull (maxx - 15, maxy - 35,
    "Radio Button A (Shortcut: 'a')", 0, RadioPressed,
    GUI.RIGHT, 'a')
radio (5) := GUI.CreateRadioButtonFull (-1, -1,
    "Radio Button B (Shortcut: 'b')", radio (4), RadioPressed,
    GUI.RIGHT, 'b')
radio (6) := GUI.CreateRadioButtonFull (-1, -1,
    "Radio Button C (Shortcut: 'c')", radio (5), RadioPressed,
    GUI.RIGHT, 'c')

loop
    exit when GUI.ProcessEvent
end loop
