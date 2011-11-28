% The "Pic.Draw2" program.
var triangle : int

% Draw Triangle
Draw.Line (0, 100, 50, 0, black)
Draw.Line (50, 0, 100, 100, black)
Draw.Line (100, 100, 0, 100, black)
Draw.Fill (50, 50, 70, black)
triangle := Pic.New (0, 0, 100, 100)

var font := Font.New ("serif:24:bold")
cls

% picCopy
Draw.FillBox (20, 100, 70, 150, 63)
Draw.Box (20, 100, 70, 150, black)
Draw.FillBox (120, 100, 170, 150, 63)
Draw.Box (120, 100, 170, 150, black)
Draw.FillBox (20, 200, 70, 250, 63)
Draw.Box (20, 200, 70, 250, black)
Draw.FillBox (120, 200, 170, 250, 63)
Draw.Box (120, 200, 170, 250, black)
Pic.Draw (triangle, 45, 125, picCopy)
Font.Draw ("picCopy", 40, 70, font, black)

% picMerge
Draw.FillBox (220, 100, 270, 150, 63)
Draw.Box (220, 100, 270, 150, black)
Draw.FillBox (320, 100, 370, 150, 63)
Draw.Box (320, 100, 370, 150, black)
Draw.FillBox (220, 200, 270, 250, 63)
Draw.Box (220, 200, 270, 250, black)
Draw.FillBox (320, 200, 370, 250, 63)
Draw.Box (320, 200, 370, 250, black)
Pic.Draw (triangle, 245, 125, picMerge)
Font.Draw ("picMerge", 230, 70, font, black)

% picUnderMerge
Draw.FillBox (440, 100, 490, 150, 63)
Draw.Box (440, 100, 490, 150, black)
Draw.FillBox (540, 100, 590, 150, 63)
Draw.Box (540, 100, 590, 150, black)
Draw.FillBox (440, 200, 490, 250, 63)
Draw.Box (440, 200, 490, 250, black)
Draw.FillBox (540, 200, 590, 250, 63)
Draw.Box (540, 200, 590, 250, black)
Pic.Draw (triangle, 465, 125, picUnderMerge)
Font.Draw ("picUnderMerge", 400, 70, font, black)
