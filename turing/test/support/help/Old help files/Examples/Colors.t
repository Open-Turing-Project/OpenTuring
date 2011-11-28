% The "Colors" program.
% Get the font height
var height, ascent, descent, intLeading : int
Font.Sizes (defFontID, height, ascent, descent, intLeading)
var ratio : real := (maxx + 1) / (maxy + 1)
var side : int := (maxy + 1) div ceil (sqrt (256 / ratio))
var x := 0
var y := maxy
for clr : 0 .. 255
    var r, g, b : real
    var textColor : int
    var clrStr := intstr (clr)
    var w := Font.Width (clrStr, defFontID)
    RGB.GetColor (clr, r, g, b)
    if r + g + b < 0.6 then
	textColor := white
    else
	textColor := black
    end if
    Draw.FillBox (x, y, x + side, y - side, clr)
    Draw.Text (clrStr, x + (side - w) div 2, y - ascent - (side - ascent) div 2,
	defFontID, textColor)
    x += side
    if x + side > maxx then
	x := 0
	y -= side
    end if
end for

