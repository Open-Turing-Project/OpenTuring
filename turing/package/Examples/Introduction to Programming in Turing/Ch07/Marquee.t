% The "Marquee" program.
% Draws a box outlined by randomly colored asterisks.
var depth, width : int
put "Enter the width of the box (less than 60)"
get width
put "Enter the depth of the box (less than 20)"
get depth
cls

% Arrange to center box in window.
const topRow := (25 - depth) div 2
const leftColumn := (80 - width) div 2
var colorNo : int

% Draw top of box
locate (topRow, leftColumn)
for count : 1 .. width
    % Choose a random color
    randint (colorNo, 0, 15)
    % Set the color.
    color (colorNo)
    put "*" ..
end for

% Draw bottom of box
locate (topRow + depth - 1, leftColumn)
for count : 1 .. width
    randint (colorNo, 0, 15)
    color (colorNo)
    put "*" ..
end for

% Draw left side of box
const sideTop := topRow + 1
const sideBottom := topRow + depth - 2
for row : sideTop .. sideBottom
    randint (colorNo, 0, 15)
    color (colorNo)
    locate (row, leftColumn)
    put "*" ..
end for

% Draw right side of box
const rightColumn := leftColumn + width - 1
for row : sideTop .. sideBottom
    randint (colorNo, 0, 15)
    color (colorNo)
    locate (row, rightColumn)
    put "*" ..
end for

