% The "GUI.CreateFrame" program.
import GUI
View.Set ("graphics:250;90,nobuttonbar")
GUI.SetBackgroundColor (gray)
var lineFrame, indentFrame, exdentFrame : int
var lineLabel, indentLabel, exdentLabel : int
lineFrame := GUI.CreateFrame (10, 10, 80, 70, 0)
indentFrame := GUI.CreateFrame (90, 10, 160, 70, GUI.INDENT)
exdentFrame := GUI.CreateFrame (170, 10, 240, 70, GUI.EXDENT)
% Label the frames.
lineLabel := GUI.CreateLabelFull (10, 10, "Line", 70, 60,
    GUI.CENTER + GUI.MIDDLE, 0)
indentLabel := GUI.CreateLabelFull (90, 10, "Indent", 70, 60,
    GUI.CENTER + GUI.MIDDLE, 0)
exdentLabel := GUI.CreateLabelFull (170, 10, "Exdent", 70, 60,
    GUI.CENTER + GUI.MIDDLE, 0)
