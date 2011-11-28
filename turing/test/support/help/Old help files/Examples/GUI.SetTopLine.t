% The "GUI.SetTopLine" program.
import GUI
View.Set ("graphics:200;300,nobuttonbar")

var textBox : int           % The Text Field ID.

% Set background color to gray for indented text box.
GUI.SetBackgroundColor (gray)

% Create the text box.
textBox := GUI.CreateTextBoxFull (10, 10, 180, 275,
    GUI.INDENT, 0)

% Read the file and place it in the text box.
for i : 1 .. 100
    GUI.AddLine (textBox, intstr (i))
end for
GUI.SetTopLine (textBox, 50)

loop
    exit when GUI.ProcessEvent
end loop
