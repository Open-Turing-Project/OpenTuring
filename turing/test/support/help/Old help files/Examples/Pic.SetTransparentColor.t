% The "Pic.SetTransparentColor" program.
var pic1 : int := Pic.FileNew ("%oot/support/help/Examples/Data Files/airplane.bmp")
var pic2 : int := Pic.FileNew ("%oot/support/help/Examples/Data Files/airplane.bmp")
Pic.SetTransparentColor (pic2, brightred)
setscreen ("offscreenonly")
for x : 100 .. maxx - 100
    cls
    put "The lower image has the transparent color set to bright red"
    Pic.Draw (pic1, x, 150, picMerge)
    Pic.Draw (pic2, x, 50, picMerge)
    View.Update
    delay (5)
end for
