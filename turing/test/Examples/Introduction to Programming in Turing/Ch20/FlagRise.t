% The "FlagRise" program
% Raises a flag up a pole

procedure drawflag (x, y, w, h, color1, color2 : int)
    const halfw := round (w / 2)
    const halfh := round (h / 2)
    drawbox (x, y, x + w, y + h, color2)
    drawfill (x + 1, y + 1, color1, color2)
    drawline (x + halfw, y, x + halfw, y + h, color2)
    drawline (x, y + halfh, x + w, y + halfh, color2)
end drawflag

setscreen ("graphics")
cls

% Draw brown flagpole
const poleleft := 150
const polelow := 50
const poleright := poleleft + 3
const polehigh := polelow + 100
drawbox (poleleft, polelow, poleright, polehigh, brown)
drawfill (poleleft + 1, polelow + 1, brown, brown)

% Draw flag at bottom of pole
const flagheight := 20
const flagwidth := 40
const flagleft := poleright + 1
const flaglow := polelow
const flagright := flagleft + flagwidth
const flaghigh := flaglow + flagheight
drawflag (flagleft, flaglow, flagwidth, flagheight, green, red)

% Record flag picture in buffer
% First declare buffer named camera
var camera : array 1 .. sizepic (flagleft, flaglow,
    flagright, flaghigh) of int
takepic (flagleft, flaglow, flagright, flaghigh, camera)
for y : polelow + 1 .. polehigh - flagheight
    % Erase former picture
    drawpic (flagleft, y - 1, camera, 1)
    % Draw picture in higher position
    drawpic (flagleft, y, camera, 0)
    sound (y * 25, 1)
end for
var reply : string (1)
getch (reply)

