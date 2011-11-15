% The "FlipHouse" program.

% This program demonstrates the Pic.FlipHorizontal and Pic.FlipVertical

% Draw a house
drawbox (30, 30, 100, 100, black)
drawbox (50, 30, 80, 60, brightred)
drawbox (35, 70, 55, 90, green)
drawline (45, 70, 45, 90, green)
drawline (35, 80, 55, 80, green)
drawbox (75, 70, 95, 90, green)
drawline (85, 70, 85, 90, green)
drawline (75, 80, 95, 80, green)
drawline (30, 100, 65, 135, brightblue)
drawline (65, 135, 100, 100, brightblue)
drawline (90, 110, 90, 150, black)
drawline (75, 125, 75, 150, black)
drawline (75, 150, 90, 150, black)
locate (maxrow, 2)
put "Home sweet home" ..

% Create the three pictures.
var pic := Pic.New (0, 0, 130, 160)
var picHor := Pic.Mirror (pic)
var picVer := Pic.Flip (pic)

% Draw the three pictures (with boxes around them)
Pic.Draw (pic, 0, 225, picCopy)
drawbox (0, 225, 130, 385, cyan)
locatexy (50, 210)
put "Normal" ..

Pic.Draw (picHor, 180, 225, picCopy)
drawbox (180, 225, 310, 385, cyan)
locatexy (180, 210)
put "    Mirrored" ..
locatexy (180, 200)
put "(with Pic.Mirror)" ..

Pic.Draw (picVer, 360, 225, picCopy)
drawbox (360, 225, 490, 385, cyan)
locatexy (370, 210)
put "    Flipped" ..
locatexy (370, 200)
put "(with Pic.Flip)" ..
