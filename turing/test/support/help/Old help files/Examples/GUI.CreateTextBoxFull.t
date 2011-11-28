% The "GUI.CreateTextBoxFull" program.
import GUI
View.Set ("graphics:300;300")

const fileNameToBeViewed : string := "%oot/support/help/Examples/GUI.CreateTextBoxFull.t"
var textBox : int           % The Text Field ID.
var title : int             % The label for the title.
var f : int                 % The stream number of the file.
var line : string           % Lines to be read from the file.

% Open the file.
open : f, fileNameToBeViewed, get
if f = 0 then
    put "Unable to open " + fileNameToBeViewed + " : ", Error.LastMsg
    return
end if

% Set background color to gray for indented text box.
GUI.SetBackgroundColor (gray)

% Create the title label and text box.
title := GUI.CreateLabelFull (20, 280, fileNameToBeViewed, 250, 0,
    GUI.CENTER, 0)
textBox := GUI.CreateTextBoxFull (10, 10, 280, 265,
    GUI.INDENT, 0)

% Read the file and place it in the text box.
GUI.SetScrollOnAdd (textBox, false)
loop
    exit when eof (f)
    get : f, line : *
    GUI.AddLine (textBox, line)
end loop

close : f           % Close the file.

loop
    exit when GUI.ProcessEvent
end loop
