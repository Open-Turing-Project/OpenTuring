% The "Pic.Scale" program.
const STAR_SIZE : int := 70

var pic : int
var newPic : int
var picWidth, picHeight : int

% Get the original picture
Draw.FillStar (0, 0, STAR_SIZE, STAR_SIZE, brightred)
Draw.Box (0, 0, STAR_SIZE, STAR_SIZE, green)
pic := Pic.New (0, 0, STAR_SIZE, STAR_SIZE)
cls

picWidth := Pic.Width (pic)
picHeight := Pic.Height (pic)

for x : 1 .. 3
    for y : 1 .. 3
	newPic := Pic.Scale (pic, x * picWidth div 2,
	    y * picHeight div 2)
	Pic.Draw (newPic, (x - 1) * maxx div 3 + 5,
	    (y - 1) * maxy div 3 + 20, picCopy)
	Pic.Free (newPic)
	Draw.Text (realstr (x / 2, 0) + " x " + realstr (y / 2, 0),
	    (x - 1) * maxx div 3 + 5, (y - 1) * maxy div 3 + 5,
	    defFontID, black)
    end for
end for
