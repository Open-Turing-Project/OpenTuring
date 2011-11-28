% The "drawfill" program.
setscreen ("graphics")
const midx := maxx div 2
const midy := maxy div 2
drawarc (midx, midy + 49, 50, 50, 0, 180, brightgreen)
drawarc (midx, midy - 49, 50, 50, 180, 360, brightgreen)
drawarc (midx + 49, midy, 50, 50, 270, 90, brightgreen)
drawarc (midx - 49, midy, 50, 50, 90, 270, brightgreen)
drawfill (midx, midy, red, brightgreen)
