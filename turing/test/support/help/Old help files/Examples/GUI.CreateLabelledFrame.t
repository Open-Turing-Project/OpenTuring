% The "GUI.CreateLabelledFrame" program.
import GUI
View.Set ("graphics:250;90,nobuttonbar")
GUI.SetBackgroundColor (gray)
var lineFrame, indentFrame, exdentFrame : int
lineFrame := GUI.CreateLabelledFrame (10, 10, 80, 70, 0, "Line")
indentFrame := GUI.CreateLabelledFrame (90, 10, 160, 70, GUI.INDENT, "Indent")
exdentFrame := GUI.CreateLabelledFrame (170, 10, 240, 70,
    GUI.EXDENT, "Exdent")
