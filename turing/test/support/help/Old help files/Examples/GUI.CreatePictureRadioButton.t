% The "GUI.CreatePictureRadioButton" program.
import GUI
View.Set ("graphics:150;200,nobuttonbar")

const size : int := 25           % The buttons size.
const border : int := 3

var starButton, mapleButton, circleButton, squareButton : int
var starPic, mapleLeafPic, circlePic, squarePic : int

procedure StarPressed
    Text.Locate (1, 1)
    put "Star Pressed    "
end StarPressed
procedure MaplePressed
    Text.Locate (1, 1)
    put "Maple Pressed "
end MaplePressed

procedure CirclePressed
    Text.Locate (1, 1)
    put "Circle Pressed"
end CirclePressed

procedure SquarePressed
    Text.Locate (1, 1)
    put "Square Pressed"
end SquarePressed

% Create the pictures.
% The star.
Draw.Star (border, border, border + size, border + size, black)
Draw.Star (border + 1, border + 1, border + size - 1,
    border + size - 1, black)
Draw.FillStar (border + 2, border + 2, border + size - 2,
    border + size - 2, brightred)
starPic := Pic.New (0, 0, 2 * border + size, 2 * border + size)

% The mapleleaf.
Draw.FillBox (border, border, border + size, border + size, white)
Draw.MapleLeaf (border, border, border + size, border + size, black)
Draw.MapleLeaf (border + 1, border + 1, border + size - 1,
    border + size - 1, black)
Draw.FillMapleLeaf (border + 2, border + 2, border + size - 2,
    border + size - 2, brightred)
mapleLeafPic := Pic.New (0, 0, 2 * border + size, 2 * border + size)

% The circle.
const radius : int := size div 2
Draw.FillBox (border, border, border + size, border + size, white)
Draw.Oval (border + radius, border + radius, radius, radius, black)
Draw.Oval (border + radius, border + radius, radius - 1, radius - 1,
    black)
Draw.FillOval (border + radius, border + radius, radius - 2,
    radius - 2, brightred)
circlePic := Pic.New (0, 0, 2 * border + size, 2 * border + size)

% The square.
Draw.FillBox (border, border, border + size, border + size, white)
Draw.Box (border, border, border + size, border + size, black)
Draw.Box (border + 1, border + 1, border + size - 1,
    border + size - 1, black)
Draw.FillBox (border + 2, border + 2, border + size - 2,
    border + size - 2, brightred)
squarePic := Pic.New (0, 0, 2 * border + size, 2 * border + size)

% Create the picture buttons.
Draw.Cls
starButton := GUI.CreatePictureRadioButton (10, maxy - 80,
    starPic, 0, StarPressed)
mapleButton := GUI.CreatePictureRadioButton (-1, -1,
    mapleLeafPic, starButton, MaplePressed)
circleButton := GUI.CreatePictureRadioButton (-1, -1,
    circlePic, mapleButton, CirclePressed)
squareButton := GUI.CreatePictureRadioButton (-1, -1,
    squarePic, circleButton, SquarePressed)

loop
    exit when GUI.ProcessEvent
end loop
