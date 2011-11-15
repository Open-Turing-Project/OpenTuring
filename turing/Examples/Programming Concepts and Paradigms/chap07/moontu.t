% File "moontu.t".
% Draws a moon with center at center of window
% of radius 30 pixels and color green.
import Turtle in "turtle.tu"
Turtle.SetColor (green)
for square : 1 .. 35   % Set to draw 35 squares.
    for side : 1 .. 4     % Set to draw 4 sides.
        Turtle.Move (60)
        Turtle.TurnLeft (90)
    end for
    Turtle.TurnLeft (10)   % Turn left 10 degrees.
end for
