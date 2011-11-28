% The "GIF" program.
var pic : int
pic := Pic.FileNew ("%oot/Support/Help/Examples/Data Files/globe.gif")
Pic.Draw (pic, 20, 50, picCopy)
Pic.Draw (pic, 150, 50, picMerge)
