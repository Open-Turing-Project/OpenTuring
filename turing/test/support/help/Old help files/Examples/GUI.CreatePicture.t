% The "GUI.CreatePicture" program.
import GUI
View.Set ("graphics:230;135,nobuttonbar")

% We'll need to create a picture for our Picture widget. Normally
% an external file (and Pic.FileNew) would be used.
Draw.FillOval (50, 50, 50, 50, blue)
Draw.FillBox (17, 17, 83, 83, brightred)
Draw.FillStar (17, 17, 83, 83, brightgreen)
Draw.FillMapleLeaf (37, 37, 63, 63, brightpurple)
var pic : int := Pic.New (0, 0, 100, 100)
var picture1, picture2 : int
var label1, label2 : int

GUI.SetBackgroundColor (gray)

label1 := GUI.CreateLabel (15, 5, "Picture (no merge)")
picture1 := GUI.CreatePicture (10, 25, pic, false)

label2 := GUI.CreateLabel (135, 5, "Picture (merge)")
picture2 := GUI.CreatePicture (120, 25, pic, true)
