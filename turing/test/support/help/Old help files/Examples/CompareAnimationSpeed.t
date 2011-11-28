% The "CompareAnimationSpeed" program.

% Set the screen black
Draw.FillBox (0, 0, maxx, maxy, black)

% Draw 100 stars
var x, y, size, clr, bg, dx, dy : int
for i : 1 .. 100
    size := Rand.Int (5, 20)
    x := Rand.Int (0, maxx - size)
    y := Rand.Int (0, maxy - size)
    clr := Rand.Int (8, 15)
    Draw.FillStar (x, y, x + size, y + size, clr)
end for

put "Press a key to switch between using View.Update and View.UpdateArea. "
put "Currently using: View.UpdateArea"

% Place the big maple leaf
size := Rand.Int (50, 100)
x := Rand.Int (0, maxx - size)
y := Rand.Int (0, maxy - size)
loop
    dx := Rand.Int (-3, 3)
    dy := Rand.Int (-3, 3)
    exit when dx not= 0 and dy not= 0
end loop

% Take a picture of the background behind the leaf
bg := Pic.New (x, y, x + size, y + size)
Draw.FillMapleLeaf (x, y, x + size, y + size, brightred)

View.Set ("offscreenonly")
var usingUpdateArea := true
loop
    if x + dx < 0 or x + dx > maxx - size then
	dx := -dx
    end if
    if y + dy < 0 or y + dy > maxy - size then
	dy := -dy
    end if
    Pic.Draw (bg, x, y, picCopy)
    Pic.Free (bg)
    x += dx
    y += dy
    bg := Pic.New (x, y, x + size, y + size)
    Draw.FillMapleLeaf (x, y, x + size, y + size, brightred)
    if usingUpdateArea then
	View.UpdateArea (x - 4, y - 4, x + size + 4, y + size + 4)
    else
	View.Update
    end if
    if hasch then
	usingUpdateArea := not usingUpdateArea
	locate (2, 1)
	if usingUpdateArea then
	    put "Currently using: View.UpdateArea"
	else
	    put "Currently using: View.Update"
	end if
	Input.Flush
    end if
end loop
