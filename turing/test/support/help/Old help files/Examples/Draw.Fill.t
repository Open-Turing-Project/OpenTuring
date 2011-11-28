% The "Draw.Fill" program.
View.Set ("graphics")
const midx := maxx div 2
const midy := maxy div 2
Draw.Arc (midx, midy + 49, 50, 50, 0, 180, brightgreen)
Draw.Arc (midx, midy - 49, 50, 50, 180, 360, brightgreen)
Draw.Arc (midx + 49, midy, 50, 50, 270, 90, brightgreen)
Draw.Arc (midx - 49, midy, 50, 50, 90, 270, brightgreen)
Draw.Fill (midx, midy, red, brightgreen)
