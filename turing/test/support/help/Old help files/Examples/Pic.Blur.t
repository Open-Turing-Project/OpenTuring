% The "Pic.Blur" program.
View.Set ("graphics:260;120,nobuttonbar")
var f := Font.New ("serif:60:bold,italic,noantialias")
Font.Draw ("Turing", 10, 30, f, red)
Draw.FillStar (70, 80, 90, 100, brightgreen)
Draw.FillBox (240, 5, 270, 35, brightblue)
var oldPic := Pic.New (0, 0, maxx, maxy)
loop
    var newPic := Pic.Blur (oldPic, 10)
    Pic.Free (oldPic)
    Pic.Draw (newPic, 0, 0, picCopy)
    delay (300)
    oldPic := newPic
end loop

