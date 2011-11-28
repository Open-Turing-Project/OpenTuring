% The "drawdot2" program.
var x, y : real
var dx, dy : real
var clr : int := 1
x := Rand.Int (1, maxx - 1) % x goes from 1 to maxx - 1
y := Rand.Int (1, maxy - 1) % y goes from 1 to maxx - 1
dx := Rand.Real - 0.5       % dx goes from -0.5 to +0.5
dy := Rand.Real - 0.5       % dy goes from -0.5 to +0.5
loop
    drawdot (round (x), round (y), clr)
    clr := (clr + 1) mod 16 % Brighter colors mostly in the first 16 colors
    x := x + dx
    y := y + dy
    if x <= 0 or x >= maxx then
	dx := -dx
    end if
    if y <= 0 or y >= maxy then
	dy := -dy
    end if
end loop
