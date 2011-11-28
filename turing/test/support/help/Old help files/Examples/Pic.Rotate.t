% The "Pic.Rotate" program.
View.Set ("graphics:200;150,nobuttonbar")
var f : int := Font.New ("Serif:36")
Font.Draw ("Hello", 10, 10, f, black)
var pic : int := Pic.New (5, 5, 120, 45)
var pic45 : int := Pic.Rotate (pic, 45, -1, -1)
var pic90 : int := Pic.Rotate (pic, 90, -1, -1)
Pic.Draw (pic45, 5, 50, picCopy)
Pic.Draw (pic90, 150, 5, picCopy)
