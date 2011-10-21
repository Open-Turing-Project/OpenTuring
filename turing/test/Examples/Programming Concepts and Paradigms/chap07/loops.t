% File "loops.t".
% Two circles are drawn simultaneously by instantiating
% two turtle objects from the modified class TurtleFatCl.
import TurtleFatCl in "turtfat.tu"
var turtle1, turtle2 : ^TurtleFatCl
% Create two turtle objects.
new turtle1
new turtle2
% Set x of each turtle at its own radius
% from the center of window, pointing up.
const xc := maxx div 2 % Center of window.
const yc := maxy div 2
const radius1 := maxx div 6
const radius2 := maxx div 4
const stepAngle := 10
% Circles will each be made of 36 short straight lines.
const pi := 3.14159
const stepAngleInRadians := stepAngle * 2 * pi / 360
const distance1 := round (radius1 * stepAngleInRadians)
const distance2 := round (radius2 * stepAngleInRadians)
turtle1 -> SetPosition (xc + radius1, yc - distance1 div 2) % Place turtle1.
turtle1 -> SetColor (yellow)
turtle1 -> TurnLeft (90) % Set to point up.
turtle1 -> SetWidth (10)
turtle2 -> SetPosition (xc + radius2, yc - distance2 div 2) % Place turtle2.
turtle2 -> SetColor (green)
turtle2 -> TurnLeft (90)
turtle2 -> SetWidth (20)
for direction : 0 .. 360 by stepAngle
    turtle1 -> Move (distance1)
    turtle1 -> TurnLeft (stepAngle)
    turtle2 -> Move (distance2)
    turtle2 -> TurnLeft (stepAngle)
end for
