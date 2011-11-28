% The "Pic.DrawSpecial1" program.
var redID, blueID : int
var x, y : int
Draw.FillStar (50, 50, 150, 150, brightred)
redID := Pic.New (50, 50, 150, 150)
cls
Draw.FillOval (100, 100, 50, 50, brightblue)
blueID := Pic.New (50, 50, 150, 150)
Pic.DrawSpecial (redID, 50, 50, picCopy, picWipeLeftToRight, 500)
Pic.DrawSpecial (blueID, 50, 50, picCopy, picWipeTopToBottom, 500)
Pic.DrawSpecial (redID, 50, 50, picCopy, picSlideLeftToRight, 500)
Pic.DrawSpecial (blueID, 50, 50, picCopy, picSlideTopToBottom, 500)
Pic.DrawSpecial (redID, 50, 50, picCopy, picFadeIn, 1000)
Pic.DrawSpecial (blueID, 50, 50, picCopy, picBlend + 50, 0)
Pic.Free (redID)
Pic.Free (blueID)
