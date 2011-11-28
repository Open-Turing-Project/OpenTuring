% The "Pic.Mirror" program.
View.Set ("graphics:210;125,nobuttonbar")

% Get the original picture
var pic, newPic : int
Draw.FillOval (50, 50, 50, 50, brightred)
Draw.FillOval (70, 50, 50, 50, colorbg)
Draw.Text ("BLUE", 40, 60, defFontID, brightblue)
Draw.Text ("MOON", 40, 40, defFontID, brightblue)
pic := Pic.New (0, 0, 100, 100)
newPic := Pic.Mirror (pic)
cls

% Draw the two images: original and flipped
Pic.Draw (pic, 10, 20, picCopy)
Draw.Text ("Original", 10, 5, defFontID, black)
Pic.Draw (newPic, 110, 20, picCopy)
Draw.Text ("Flipped", 140, 5, defFontID, black)
