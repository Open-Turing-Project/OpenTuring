% The "DrawDesign2" program
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

const width := 20
const height := 10
% Draw flag in bottom left-hand corner
drawflag (0, 0, width, height, magenta, green)
var snapshot : array 1 .. sizepic (0, 0, width, height) of int
takepic (0, 0, width, height, snapshot) % record the image
drawpic (0, 0, snapshot, 1) % erase the original

for xcount : 0 .. 9
    for ycount : 0 .. 9
	const x := xcount * (width + 2)
	const y := ycount * (height + 2)
	drawpic (x, y, snapshot, 0)
    end for
end for
var reply : string (1)
getch (reply)
