% The "GUI.CreateTextFieldFull" program
import GUI
View.Set ("graphics:200;100,nobuttonbar")

var nameTextField, addressTextField : int  % The Text Field IDs.

procedure NameEntered (text : string)
    GUI.SetSelection (addressTextField, 0, 0)
    GUI.SetActive (addressTextField)
end NameEntered

procedure AddressEntered (text : string)
    GUI.SetSelection (nameTextField, 0, 0)
    GUI.SetActive (nameTextField)
end AddressEntered

GUI.SetBackgroundColor (gray)
var quitButton := GUI.CreateButton (52, 5, 100, "Quit", GUI.Quit)
nameTextField := GUI.CreateTextFieldFull (50, 70, 100, "",
    NameEntered, GUI.INDENT, 0, 0)
addressTextField := GUI.CreateTextFieldFull (50, 40, 100, "",
    AddressEntered, GUI.INDENT, 0, 0)
var nameLabel := GUI.CreateLabelFull (45, 70, "Name", 0, 0,
    GUI.RIGHT, 0)
var addressLabel := GUI.CreateLabelFull (45, 40, "Address", 0, 0,
    GUI.RIGHT, 0)
loop
    exit when GUI.ProcessEvent
end loop

GUI.Dispose (quitButton)
colorback (gray)
Text.Locate (maxrow - 1, 1)
put "Name = ", GUI.GetText (nameTextField)
put "Address = ", GUI.GetText (addressTextField) ..
