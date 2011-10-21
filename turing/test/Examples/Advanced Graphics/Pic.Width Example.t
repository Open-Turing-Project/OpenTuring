% The "Pic.Width Example" program.
var pic : int := Pic.FileNew ("lighthouse.jpg")
var width : int := Pic.Width (pic)
var height : int := Pic.Height (pic)
var x, y : int := 0
loop
    exit when y > maxy
    loop
	exit when x > maxx
	Pic.Draw (pic, x, y, picCopy)
	x := x + width
    end loop
    x := 0
    y := y + height
end loop
