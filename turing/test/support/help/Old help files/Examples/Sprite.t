% The "Sprite" program.

const maxSprites := 10

var xa : array 1 .. 3 of int := init (0, 20, 40)
var ya : array 1 .. 3 of int := init (0, 40, 0)
var pics : array 1 .. maxSprites, 0 .. 35 of int

Sprite.SetFrameRate (100)

var sprite, angle : array 1 .. maxSprites of int
var x, y : array 1 .. maxSprites of real
for i : 1 .. maxSprites
    % Create all the pictures for each sprite
    Draw.FillPolygon (xa, ya, 3, Rand.Int (8, 14))
    Font.Draw (intstr (i - 3), 15, 15, defFontID, black)
    Draw.Polygon (xa, ya, 3, black)
    pics (i, 0) := Pic.New (0, 0, 40, 40)
    for a : 1 .. 35
	pics (i, a) := Pic.Rotate (pics (i, 0), -a * 10, -1, -1)
    end for
    angle (i) := Rand.Int (0, 35)
    sprite (i) := Sprite.New (pics (i, angle (i)))
    x (i) := Rand.Int (30, maxx - 30)
    y (i) := Rand.Int (30, maxy - 30)
    Sprite.SetPosition (sprite (i), round (x (i)), round (y (i)), true)
    Sprite.SetHeight (sprite (i), i - 3)
    Sprite.Show (sprite (i))
end for

% Draw the background
cls
Draw.FillBox (0, 0, maxx, maxy, red)
View.SetTransparentColour (red)
for i : 1 .. 20 by 2
    Draw.FillBox (i * maxx div 20, 0, i * maxx div 20 + 10, maxy, brightblue)
    Draw.FillBox (0, i * maxx div 20, maxx, i * maxx div 20 + 10, brightgreen)
end for

loop
    for i : 1 .. maxSprites
	var dx, dy : real
	dx := sind (angle (i) * 10)
	dy := cosd (angle (i) * 10)
	if x (i) + dx < 20 or x (i) + dx > maxx - 20 then
	    angle (i) := (36 - angle (i)) mod 36
	    Sprite.ChangePic (sprite (i), pics (i, angle (i)))
	    dx := sind (angle (i) * 10)
	    dy := cosd (angle (i) * 10)
	end if
	if y (i) + dy < 20 or y (i) + dy > maxy - 20 then
	    angle (i) := (18 - angle (i)) mod 36
	    Sprite.ChangePic (sprite (i), pics (i, angle (i)))
	    dx := sind (angle (i) * 10)
	    dy := cosd (angle (i) * 10)
	end if
	x (i) += dx
	y (i) += dy
	locate (1, 1)
	Sprite.SetPosition (sprite (i), round (x (i)), round (y (i)), true)
    end for
    delay (10)
end loop
