% The "Pic.Blend" program.
View.Set ("graphics:340;140,nobuttonbar")
var pic1, pic2, picBlended : int
Draw.FillBox (0, 0, 100, 100, brightred)
Draw.FillOval (50, 50, 50, 50, brightblue)
pic1 := Pic.New (0, 0, 100, 100)
cls
Draw.FillStar (10, 10, 90, 90, brightgreen)
Draw.FillMapleLeaf (10, 10, 90, 90, brightred)
pic2 := Pic.New (0, 0, 100, 100)
cls
% Create new picture by blending 30% from pic1, 70% from pic2
picBlended := Pic.Blend (pic1, pic2, 30)
locatexy (45, 130)
put "Pic 1" ..
locatexy (155, 130)
put "Pic 2" ..
locatexy (265, 130)
put "Blend" ..
Pic.Draw (pic1, 10, 10, picCopy)
Pic.Draw (pic2, 120, 10, picCopy)
Pic.Draw (picBlended, 230, 10, picCopy)
