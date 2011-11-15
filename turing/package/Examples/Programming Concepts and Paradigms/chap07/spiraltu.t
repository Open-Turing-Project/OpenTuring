% File "spiraltu.t".
% Two turtle objects are created
% from the Turtle class and used to draw in the window.
import TurtleCl in "turtlecl.tu"
% Declare pointers to two objects.
var turtle1, turtle2 : ^TurtleCl
% Create objects.
new turtle1
new turtle2
% Set colors of each turtle object.
turtle1 -> SetColor (brightblue)
% Turtle 1 started pointing to the upper left
turtle1 -> TurnLeft (120)
turtle2 -> SetColor (brightred)
% Turtle 2 started pointing to the upper right
turtle2 -> TurnLeft (60)
% Both automatically start at center of window.
% Draw two spirals.
for decreasing distance : maxx div 4 .. 1
    turtle1 -> Move (distance)
    turtle1 -> TurnLeft (70)
    turtle2 -> Move (distance)
    turtle2 -> TurnRight (70)
end for
