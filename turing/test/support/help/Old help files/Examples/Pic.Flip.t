% The "Pic.Flip" program.
% Get the original picture
var pic, newPic, width, height : int
pic := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/lighthouse.jpg")
newPic := Pic.Flip (pic)
width := Pic.Width (pic)
height := Pic.Height (pic)

% Draw the two images: original and flipped
View.Set ("graphics:" + intstr (2 * width + 30) + ";" +
    intstr (height + 25) + ",nobuttonbar")
Pic.Draw (pic, 10, 20, picCopy)
Draw.Text ("Original", 50, 5, defFontID, black)
Pic.Draw (newPic, 20 + Pic.Width (pic), 20, picCopy)
Draw.Text ("Flipped", 60 + Pic.Width (pic), 5, defFontID, black)
