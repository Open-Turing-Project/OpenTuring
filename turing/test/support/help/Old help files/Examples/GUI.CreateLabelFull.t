% The "GUI.CreateLabelFull" program.
import GUI
View.Set ("graphics:300;100,nobuttonbar")
var lowerLeft : int := GUI.CreateLabel (0, 0, "Lower-Left")
var center : int := GUI.CreateLabelFull (0, 0, "Center", maxx, maxy,
    GUI.MIDDLE + GUI.CENTER, 0)
var upperRight : int := GUI.CreateLabelFull (0, 0, "Upper-Right",
    maxx, maxy, GUI.RIGHT + GUI.TOP, 0)
