% The "GUI.CreateLine" program.
import GUI
View.Set ("graphics:180;100,nobuttonbar")
GUI.SetBackgroundColor (gray)
var line, indentLine, exdentLine : int
var lineLabel, indentLabel, exdentLabel : int

line := GUI.CreateLine (30, 20, 30, 90, 0)
indentLine := GUI.CreateLine (90, 20, 90, 90, GUI.INDENT)
exdentLine := GUI.CreateLine (150, 20, 150, 90, GUI.EXDENT)        
% Label the lines.
lineLabel := GUI.CreateLabelFull (30, 15, "Line", 0, 0,
    GUI.CENTER + GUI.TOP, 0)
indentLabel := GUI.CreateLabelFull (90, 15, "Indent", 0, 0,
    GUI.CENTER + GUI.TOP, 0)
exdentLabel := GUI.CreateLabelFull (150, 15, "Exdent", 0, 0,
    GUI.CENTER + GUI.TOP, 0)
