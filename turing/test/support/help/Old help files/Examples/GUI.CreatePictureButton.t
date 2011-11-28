% The "GUI.CreatePictureButton" program.
import GUI
View.Set ("graphics:130;70,nobuttonbar")

const size : int := 25           % The buttons size.
const border : int := 3

var starButton, mapleButton, starPic, mapleLeafPic : int

procedure StarPressed
    Text.Locate (1, 1)
    put "Star Pressed    "
end StarPressed

procedure MaplePressed
    Text.Locate (1, 1)
    put "Maple Pressed "
end MaplePressed

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

% Create the picture buttons.
Draw.Cls
starButton := GUI.CreatePictureButton (10, 10, starPic, StarPressed)
mapleButton := GUI.CreatePictureButton (55, 10, mapleLeafPic,
    MaplePressed)

loop
    exit when GUI.ProcessEvent
end loop
