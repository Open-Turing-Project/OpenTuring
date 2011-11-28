% The "View.UpdateArea" program.
View.Set ("offscreenonly")
var SIZE : int := 50
loop
    for x : 0 .. maxx - SIZE
	Draw.FillStar (x, 100, x + SIZE, 100 + SIZE, brightgreen)
	View.UpdateArea (x - 1, 100, x + SIZE, 100 + SIZE)
	Draw.FillStar (x, 100, x + SIZE, 100 + SIZE, colorbg)
    end for
    for decreasing x : maxx - SIZE .. 0
	Draw.FillStar (x, 100, x + SIZE, 100 + SIZE, brightgreen)
	View.UpdateArea (x, 100, x + SIZE + 1, 100 + SIZE)
	Draw.FillStar (x, 100, x + SIZE, 100 + SIZE, colorbg)
    end for
end loop
