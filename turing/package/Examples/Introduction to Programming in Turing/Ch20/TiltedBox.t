% The "TiltedBox" program
% The square box of length size is tilted so that its bottom
% makes an angle theta degrees with x-axis
% The upper left corner of the box is at the center
var theta : real
put "Enter angle of tilt in degrees   " ..
get theta
var size : int
put "Enter size of square box in pixels   " ..
get size
% Draw box
const xcenter := maxx div 2
const ycenter := maxy div 2
const c := cosd (theta)
const s := sind (theta)
% Draw box in cyan
const x1 := xcenter
const y1 := ycenter
const x2 := x1 + round (size * c)
const y2 := y1 - round (size * s)
const x3 := x2 + round (size * s)
const y3 := y2 + round (size * c)
const x4 := x1 + round (size * s)
const y4 := y1 + round (size * c)
drawline (x1, y1, x2, y2, cyan)
drawline (x2, y2, x3, y3, cyan)
drawline (x3, y3, x4, y4, cyan)
drawline (x4, y4, x1, y1, cyan)
var reply : string (1)
getch (reply)
