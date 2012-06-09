class _Color
    export ~.*Color, var r, var g, var b, use
    type Color : ^_Color
    var r, g, b : real := 0
    proc use
	RGB.SetColor (255, r, g, b)
    end use
end _Color
fcn newColor (r, g, b : real) : Color
    var n : Color
    new n
    n -> r := r
    n -> g := g
    n -> b := b
    result n
end newColor 
fcn GetColor(p : int) : Color
    var w, r, g, b : real
    b := bits (p, 24 .. 31) / 256
    r := bits (p, 16 .. 23) / 256
    g := bits (p, 8 .. 15) / 256
    w := bits (p, 0 .. 7) / 256
    result newColor (r, g, b)
end GetColor
fcn GetColorNum(r,g,b : nat) : int
    var p : nat
    bits (p, 24 .. 31) := b
    bits (p, 16 .. 23) := r
    bits (p, 8 .. 15) := g
    bits (p, 0 .. 7) := 0
    result p
end GetColorNum
fcn tonyswhatdotcolor (x, y : int) : Color
    var bleh : array 1 .. sizepic (1, 1, 2, 2) of int
    for i : 1 .. upper (bleh)
	bleh (i) := 9999
    end for
    takepic (x, y, x + 1, y + 1, bleh)
    var p, q: int
    var w, r, g, b : real
    if (bleh (19) ~= 9999) then
	p := bleh (19)
    else
	p := bleh (21)
	assert false
    end if
    b := bits (p, 24 .. 31) / 256
    r := bits (p, 16 .. 23) / 256
    g := bits (p, 8 .. 15) / 256
    w := bits (p, 0 .. 7) / 256
    result newColor (r, g, b)
end tonyswhatdotcolor 

const size := 500
setscreen ("graphics:500;500")
Draw.FillBox(0,0,maxx,maxy,1)
Draw.Line(50,50,100,120,red)
Draw.Dot(maxx,maxy,brightblue)
Draw.Dot(0,0,brightred)
Draw.Dot(maxx,0,yellow)
Draw.Dot(0,maxy,magenta)
% Create buffer big enough to hold happy pic
var pic : array 1 .. sizepic (0, 0, maxx, maxy) of int
% Copy picture into the buffer, which is the pic array
takepic (0, 0, maxx, maxy, pic)
cls
% inherently nonportable
for i : 19..30
    put i, ": ",pic(i)
end for
quit
put upper(pic), " - ", size**2
var start := pic(7) + 7
for i : (start-3)..(start+3)
    put i, ": ",pic(i)
end for
View.Update
%quit
delay(5000)
for i : 19..upper(pic)
    var c := GetColorNum(0,255,0)
    pic(i) := c
end for
drawpic (0, 0, pic, 0)
