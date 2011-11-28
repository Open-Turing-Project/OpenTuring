% The "Pic.Rotate2" program.
View.Set ("graphics:300;250,nobuttonbar")
var pic : array 0 .. 35 of int
var f : int := Font.New ("Serif:36")
const CTR : int := 57
Font.Draw ("Hello", 5, 45, f, black)
Draw.FillOval (CTR, CTR, 3, 3, brightred)
pic (0) := Pic.New (0, 0, 115, 115)
cls
for angle : 1 .. 35
    pic (angle) := Pic.Rotate (pic (0), angle * 10, CTR, CTR)
end for

var x : int := CTR
var y : int := CTR
var dx : int := 1
var dy : int := 1
loop
    for angle : 0 .. 35
	Pic.Draw (pic (angle), x - CTR, y - CTR, picCopy)
	if x + dx < CTR or x + dx > maxx - CTR then
	    dx := dx
	end if
	if y + dy < CTR or y + dy > maxy - CTR then
	    dy := dy
	end if
	x += dx
	y += dy
	delay (50)
    end for
end loop
