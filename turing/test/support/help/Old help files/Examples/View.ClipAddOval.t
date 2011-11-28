% The "View.ClipAddOval" program.
const c1 : int := maxy div 4
const c2 : int := 3 * maxy div 4
const radius : int := maxy div 4
View.ClipAddOval (c1, c1, radius, radius)
View.ClipAddOval (c1, c2, radius, radius)
View.ClipAddOval (c2, c1, radius, radius)
View.ClipAddOval (c2, c2, radius, radius)

% Draw the random squares in the box
var x, y, clr : int
loop
    x := Rand.Int (0, maxx - 30)             % Random x
    y := Rand.Int (0, maxy - 30)             % Random y
    clr := Rand.Int (0, maxcolor)            % Random color
    Draw.FillBox (x, y, x + 30, y + 30, clr)
end loop
