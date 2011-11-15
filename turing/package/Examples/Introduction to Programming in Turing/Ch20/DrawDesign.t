% The "DrawDesign" program
% Repeats a pattern in window
setscreen ("graphics")

procedure drawflag (x, y, w, h, color1, color2 : int)
    const halfw := round (w / 2)
    const halfh := round (h / 2)
    drawbox (x, y, x + w, y + h, color2)
    drawfill (x + 1, y + 1, color1, color2)
    drawline (x + halfw, y, x + halfw, y + h, color2)
    drawline (x, y + halfh, x + w, y + halfh, color2)
end drawflag

% Draw a magenta and green pattern
const width := 20
const height := 10
for xcount : 0 .. 9
    for ycount : 0 .. 9
	const x := xcount * (width + 2)
	const y := ycount * (height + 2)
	drawflag (x, y, width, height, green, magenta)
    end for
end for
var reply : string (1)
getch (reply)
